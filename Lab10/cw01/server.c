//
// Created by woolfy on 6/9/18.
//

/*
 *
 * 2) another socket (client+server)
*/
#include "server.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

Epoll epoll;
Socket socket_data;
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

    close(socket_data.fd);

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

    sock = socket (type, SOCK_STREAM, 0);
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
    return sock;
}

void send_message(int socket,message* content){
    if(send(socket,content, sizeof(message),0)==-1){
        perror("Error at sending");
    }
    else{
        printf("Send message with id %d to Client with values: %d %d\n",content->id,content->content[0],content->content[1]);
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
        if(clients[i].fd != -1 && clients[i].is_busy == 0){
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

        send_message(clients[current_client].fd,&message);
    }
}

void* logging_thread_job(void* data){
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    int index = *(int*)data;

    struct sockaddr_in client_address;
    socklen_t addr_len = 0;
    message message_to_receive;

    while(1){
        if(clients[index].fd == -1){
            if((clients[index].fd = accept(socket_data.fd,(struct sockaddr*)&client_address,&addr_len))<0){
                perror("Error at accepting");
                exit(EXIT_FAILURE);
            }
        }

        while(recv(clients[index].fd, &message_to_receive,sizeof(message_to_receive), 0) > 0) {
            if(message_to_receive.type == login){
                sem_wait(&clients_sem);

                printf("Got login request from %s. \n",message_to_receive.name);
                fflush(stdout);
                int i;
                for(i=0;i<MAX_CLIENTS;i++){
                    if(strcmp(clients[i].name,message_to_receive.name)==0 && clients[i].fd!=-1){
                        printf("Already had a client with name: %s\nNot sending any response to client.\n",clients[i].name);
                        fflush(stdout);
                        clients[index].fd = -1;
                        sem_post(&clients_sem);
                        return NULL;
                    }
                }
                strcpy(clients[index].name,message_to_receive.name);
                clients[index].is_busy = 0;
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
                            clients[i].fd = -1;
                        }
                        break;
                    }
                }

                process_response(message_to_receive);
            }
        }
    }
}

void handle_loging_sessions(){
    int i;
    for(i=0;i<MAX_CLIENTS;i++){
        clients[i].fd = -1;
        clients[i].is_pinged = 0;
    }

    for(i=0;i<MAX_CLIENTS;i++){
        int* index = malloc(sizeof(int));
        *index = i;
        pthread_create(&logging_local_threads[i],NULL,logging_thread_job,index);
    }
}

void* ping_them_all(void* a){
    int i;
    while(1){
        sleep(10);
        for(i=0;i<MAX_CLIENTS;i++){
            if(clients[i].fd != -1 && clients[i].is_busy == 0){
                message* message = malloc(sizeof(message));
                message->type = ping;
                send_message(clients[i].fd,message);
                free(message);
            }
        }
        sleep(10);
        for(i=0;i<MAX_CLIENTS;i++){
            if(clients[i].fd != -1 && clients[i].is_pinged == 0){
                clients[i].fd = -1; /* remove client */
            }
        }
    }
}

int main(int argc, char** argv){
    if(argc != 3){
        printf("Wrong number of args \nNeed 2: TCP port number and UNIX socket path\n");
        exit(EXIT_FAILURE);
    }

//    server.socket_path = current_path;// argv[1];
    server.port = (int) strtol(argv[2], NULL, 10);
    printf("Path %s, port: %d\n",current_path,server.port);


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

    socket_data.fd = make_named_socket(current_path,AF_UNIX);

    if(listen(socket_data.fd,MAX_CLIENTS) == -1){
        perror("Listen");
        exit(EXIT_FAILURE);
    }

    handle_loging_sessions();
    pthread_create(&pinging_thread,NULL,ping_them_all,NULL);
    run();

    return 0;
}

#pragma clang diagnostic pop