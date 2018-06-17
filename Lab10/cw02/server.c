//
// Created by woolfy on 6/9/18.
//

#include "server.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

Epoll epoll;
Socket local_socket_data;
Socket remote_socket_data;
Client clients[MAX_CLIENTS];
pthread_t logging_local_threads[MAX_CLIENTS];
pthread_t pinging_thread;
sem_t clients_sem;
Server server;

void clean(){

    int i;
    for(i=0;i<MAX_CLIENTS;i++){
        pthread_cancel(logging_local_threads[i]);
    }

    close(local_socket_data.fd);

    sem_destroy(&clients_sem);

    if(epoll.fd != -1){
        close(epoll.fd);
    }

    exit(EXIT_SUCCESS);
}

void handleSignal(int singno){
    clean();
}

/*ftp://ftp.gnu.org/old-gnu/Manuals/glibc-2.2.3/html_chapter/libc_16.html*/

int make_named_socket (const char *filename,sa_family_t type){
    struct sockaddr_un name;
    int sock;
    /* Create the socket. */

    /* Normally only a single protocol exists to support a particular socket type within a given protocol  family,  in  which  case
   protocol  can be specified as 0. */

    sock = socket (type, SOCK_DGRAM, 0);
    if (sock < 0){
        perror ("socket");
        exit (EXIT_FAILURE);
    }

    /* Bind a name to the socket. */
    name.sun_family = AF_UNIX;
    strcpy (name.sun_path, filename);

    unlink(filename); /* we dont care about error since on first run this file wont exist */

    if (bind (sock, (struct sockaddr *) &name, sizeof(name)) < 0){
        perror ("bind");
        exit (EXIT_FAILURE);
    }

    if(connect(sock,(struct sockaddr *) &name,sizeof(name)) == -1){
        perror("Connect");
    }

    return sock;
}

void send_message(int socket,struct sockaddr_in addr_remote,
        struct sockaddr_un addr_local,enum connection_mode mode,message content){
    int send = 0;
    if(mode ==local_unix){
        if(sendto(local_socket_data.fd,&content,sizeof(message),0,(struct sockaddr*)&addr_local,sizeof(addr_local))==-1){
            perror("Error at sending local");
        }
        else{
            send = 1;
        }
    }
    else{
        if(sendto(remote_socket_data.fd,&content,sizeof(message),0,(struct sockaddr*)&addr_remote,sizeof(addr_remote))==-1){
            perror("Error at sending remote");
        }
        else{
            send = 1;
        }
    }
    if(send){
        printf("Send message with id %d to Client with values: %d %d\n",content.id,content.content[0],content.content[1]);
    }
}

void get_message(message* msg){
    char buffer[20];
    printf("Provide command:\n");
    scanf("%s %d %d",buffer,&msg->content[0],&msg->content[1]);
    printf("Command provided\n");
    fflush(stdout);

    if(strcmp(buffer,"add")==0){
        msg->type = add;
    }
    else if(strcmp(buffer,"mul")==0){
        msg->type = mul;
    }
    else if(strcmp(buffer,"sub")==0){
        msg->type = sub;
    }
    else if(strcmp(buffer,"stop")==0){
        exit(EXIT_SUCCESS);
    }
    else{
        printf("Command not found\n");
        msg->type = error;
    }
    msg->id = rand()%20;
}

int are_free_clients_available(){
    int i;
    for(i=0;i<MAX_CLIENTS;i++){
        if(clients[i].is_logged == 1 && clients[i].is_busy == 0){
            printf("Free client: %d \n",i);
            return i;
        }
    }

    return -1;
}

void process_response(message message_to_receive){
    if(message_to_receive.type == res){
        printf("Got result from client: %d\n to request %d ",message_to_receive.content[0],message_to_receive.id);
    }
    else if(message_to_receive.type == error){
        printf("Error occurred with values: %d %d\n",
               message_to_receive.content[0],message_to_receive.content[1]);
    }
    else if(message_to_receive.type == ping){
        printf("Got ping back from %s\n",message_to_receive.name);
    }
    else if(message_to_receive.type == logout){
        printf("Client %s just logged out\n",message_to_receive.name);
    }
    else{
        printf("Got sthing weird\n");
    }
}

void run(){
    while(1){
        message message;
        get_message(&message);


        int current_client = -1;

        sem_wait(&clients_sem);

        while((current_client = are_free_clients_available())==-1){
            sem_post(&clients_sem);
            printf("waiting for available clients\n");
            fflush(stdout);
            sleep(1);
            sem_wait(&clients_sem);
        }

        clients[current_client].is_busy = 1;
        sem_post(&clients_sem);

        if(clients[current_client].mode == local_unix){
            send_message(local_socket_data.fd,clients[current_client].fd_remote,
                         clients[current_client].fd_local,clients[current_client].mode,message);
        }
        else{
            send_message(remote_socket_data.fd,clients[current_client].fd_remote,
                         clients[current_client].fd_local,clients[current_client].mode,message);

        }
    }
}

void* logging_thread_job(void* data){
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    pair *pair_v;
    pair_v = (pair*)(data);
    enum connection_mode mode = pair_v->mode;
    int index = pair_v->index;

    struct sockaddr_in client_address;
    socklen_t addr_len = sizeof(client_address);
    message message_to_receive;
    int socket = -1;
    if(mode == local_unix){
        socket = local_socket_data.fd;
    }
    else{
        socket = remote_socket_data.fd;
    }

    while(1){
        if(mode == local_unix){
            while(recvfrom(socket, &message_to_receive,sizeof(message_to_receive), 0,(struct sockaddr*)&clients[index].fd_local,&addr_len) > 0) {
                if(message_to_receive.type == login){
                    sem_wait(&clients_sem);

                    printf("Got login request from %s. \n",message_to_receive.name);
                    fflush(stdout);
                    int i;
                    for(i=0;i<MAX_CLIENTS;i++){
                        if(strcmp(clients[i].name,message_to_receive.name)==0 && clients[i].is_logged==1){
                            printf("Already had a client with name: %s\nNot sending any response to client.\n",clients[i].name);
                            fflush(stdout);
                            sem_post(&clients_sem);
                            return NULL;
                        }
                    }
                    strcpy(clients[index].name,message_to_receive.name);
                    clients[index].is_busy = 0;
                    clients[index].mode = local_unix;
                    clients[index].is_logged = 1;

                    printf("New client logged using name %s\n", message_to_receive.name);
                    fflush(stdout);

                    sem_post(&clients_sem);
                }
                else{
                    int i;
                    for(i=0;i<MAX_CLIENTS;i++){
                        if(strcmp(clients[i].name,message_to_receive.name)==0){
                            clients[i].is_busy = 0;
                            if(message_to_receive.type == ping){
                                clients[i].is_pinged = 1;
                            }
                            if(message_to_receive.type == logout){
                                clients[i].is_logged = 1;
                            }
                            break;
                        }
                    }

                    process_response(message_to_receive);
                }
            }
        }
        else{
            while(recvfrom(socket, &message_to_receive,sizeof(message_to_receive), 0,(struct sockaddr*)&clients[index].fd_remote,&addr_len) > 0) {
            if(message_to_receive.type == login){
                sem_wait(&clients_sem);

                printf("Got login request from %s. \n",message_to_receive.name);
                fflush(stdout);
                int i;
                for(i=0;i<MAX_CLIENTS;i++){
                    if(strcmp(clients[i].name,message_to_receive.name)==0 && clients[i].is_logged==1){
                        printf("Already had a client with name: %s\nNot sending any response to client.\n",clients[i].name);
                        fflush(stdout);
                        sem_post(&clients_sem);
                        return NULL;
                    }
                }
                strcpy(clients[index].name,message_to_receive.name);
                clients[index].is_busy = 0;
                clients[index].is_logged = 1;
                clients[index].mode = net;

                printf("New client logged using name %s\n", message_to_receive.name);
                fflush(stdout);

                message message1;
                message1.mode = net;
                message1.type = add;
                message1.content[1] = 1;
                message1.content[0] = 2;
                send_message(0,clients[index].fd_remote,clients[index].fd_local,net,message1);
                sem_post(&clients_sem);
            }
            else{
                int i;
                for(i=0;i<MAX_CLIENTS;i++){
                    if(strcmp(clients[i].name,message_to_receive.name)==0){
                        clients[i].is_busy = 0;
                        if(message_to_receive.type == ping){
                            clients[i].is_pinged = 1;
                        }
                        if(message_to_receive.type == logout){
                            clients[i].is_logged = 0;
                        }
                        break;
                    }
                }

                process_response(message_to_receive);
            }
        }
        }
    }
}

void handle_loging_sessions(){
    int i;
    for(i=0;i<MAX_CLIENTS;i++){
        clients[i].is_logged = 0;
        clients[i].is_pinged = 0;
    }

    for(i=0;i<MAX_CLIENTS/2;i++){
        pair* index = malloc(sizeof(pair));
        index->index = i;
        index->mode = local_unix;
        pthread_create(&logging_local_threads[i],NULL,logging_thread_job,index);
    }
    for(;i<MAX_CLIENTS;i++){
        pair* index = malloc(sizeof(pair));
        index->index = i;
        index->mode = net;
        pthread_create(&logging_local_threads[i],NULL,logging_thread_job,index);
    }
}

void* ping_them_all(void* a){
    int i;
    while(1){
        sleep(10);
        for(i=0;i<MAX_CLIENTS;i++){
            if(clients[i].is_logged == 1 && clients[i].is_busy == 0){
                message* message = malloc(sizeof(message));
                message->type = ping;
                send_message(local_socket_data.fd,clients[i].fd_remote,clients[i].fd_local,clients[i].mode,*message);
                free(message);
            }
        }
        sleep(10);
        for(i=0;i<MAX_CLIENTS;i++){
            if(clients[i].is_logged != 0 && clients[i].is_pinged == 0){
//                clients[i].fd = NULL; /* remove client */
                clients[i].is_logged = 0;
            }
        }
    }
}

int main(int argc, char** argv){
    if(argc != 3){
        printf("Wrong number of args \nNeed 2: TCP port number and UNIX socket path\n");
        exit(EXIT_FAILURE);
    }

    server.socket_path = argv[1];
    server.port = (int) strtol(argv[2], NULL, 10);
    printf("Path %s, port: %d\n",argv[1],server.port);


    struct sigaction sigact;
    sigemptyset(&sigact.sa_mask);
    sigact.sa_handler = handleSignal;
    sigaction(SIGINT, &sigact, NULL);

    if(sem_init(&clients_sem,0,1)!=0){
        perror("Sem_init:");
        exit(EXIT_FAILURE);
    }

    atexit(clean);
    epoll.fd = epoll_create1(0);
    if(epoll.fd == -1){
        perror("Error at epoll_create1");
        exit(EXIT_FAILURE);
    }

    local_socket_data.fd = make_named_socket(server.socket_path,AF_UNIX);


    /*web socket */

    if((remote_socket_data.fd = socket(AF_INET,SOCK_DGRAM,0))<0){
        perror("Remote socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in addr;
    memset(&addr,'\0',sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons((uint16_t) server.port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(remote_socket_data.fd, (struct sockaddr *)&(addr), sizeof(addr)) != 0){
        perror("failed to bind inet socket");
        return 1;
    }


    handle_loging_sessions();
    pthread_create(&pinging_thread,NULL,ping_them_all,NULL);
    run();

    return 0;
}

#pragma clang diagnostic pop