//
// Created by woolfy on 6/9/18.
//

#include "server.h"

char* name;
enum connection_mode mode;
char* unix_path;
int socket_fd;
int port;
struct sockaddr* addr;

void send_last_message(){
    printf("Sending logout message to server\n");
    message message;
    message.type = logout;
    strcpy(message.name,name);

    if(sendto(socket_fd,&message,sizeof(message),0
            ,addr,sizeof(addr))==-1){
        perror("While sending last msg");
    }

    close(socket_fd);
    exit(0);
}

void handleSignal(int x){
    send_last_message();
}


void process_data(message* data){
    if(data->type == add){
        data->content[0] = data->content[0]+data->content[1];
        data->type = res;
    }
    else if(data->type == mul){
        data->content[0] = data->content[0]*data->content[1];
        data->type = res;
    }
    else if(data->type == sub){
        data->content[0] = data->content[0]-data->content[1];
        data->type = res;
    }
    else if(data->type == error){
        printf("Got error from server -> sending error\n");
    }
    else if(data->type == ping){
        printf("Omg, i got pinged\n");
        return;
    }
    else{
        printf("got random stuff\n");
    }

    data->content[1] = -1;
    if(data->type != error){
        printf("Client computed %d\n",data->content[0]);
    }

    /* for the ping we simply pass the same message */
}

void run(){
    message logging_message;
    message message_to_receive;

    logging_message.type = login;
    strcpy(logging_message.name,name);
    printf("Connected new client with name: %s\n",message_to_receive.name);

    if(sendto(socket_fd,&logging_message,sizeof(logging_message),0,
              addr,sizeof(addr)) == -1){
        perror("Error at sending first msg");
        exit(1);
    }

    printf("Client send a message to log in\n");

    socklen_t len = 0;

    while(1){
        while(recvfrom(socket_fd, &message_to_receive, sizeof(message_to_receive), 0, addr,
                       &len) > 0){
            printf("Client got: %d %d\n",message_to_receive.content[0],message_to_receive.content[1]);

            process_data(&message_to_receive);
            strcpy(message_to_receive.name,name);

            if(sendto(socket_fd,&message_to_receive,sizeof(message_to_receive),0,
                      addr,sizeof(addr)) == -1){
                perror("Error at sending");
                exit(1);
            }
        }
    }
}


int main(int argc, char** argv){
    if(argc!=5){
        printf("Wrong number of args");
        return 1;
    }

//    atexit(send_last_message);

    struct sigaction sigact;
    sigemptyset(&sigact.sa_mask);
    sigact.sa_handler = handleSignal;
    sigaction(SIGINT, &sigact, NULL);

    name = argv[1];
    port = (int) strtol(argv[4], NULL, 10);
    if(strcmp(argv[2],"net")==0){
        mode = net;
        printf("Mode == local_unix| port: %d | path: %s \n",port, argv[3]);
    }
    else{
        printf("Mode == local_unix| port: %d | path: %s \n",port, argv[3]);
        mode = local_unix;
    }

    if(mode == local_unix){

        socket_fd = socket(AF_UNIX,SOCK_DGRAM,0);
        if(socket_fd == -1){
            perror("Not able to create socket");
            return 1;
        }
        struct sockaddr_un addr;
        memset(&addr,'\0',sizeof(addr));
        addr.sun_family = AF_UNIX;
        strncpy (addr.sun_path, argv[3], sizeof (addr.sun_path));


        run();
    }
    else{
        socket_fd = socket(AF_INET,SOCK_DGRAM,0);
        if(socket_fd == -1){
            perror("Not able to create socket");
            return 1;
        }
        struct sockaddr_in addr;
        memset(&addr,'\0',sizeof(addr));
        addr.sin_family = AF_INET;

        if(inet_pton(AF_INET, argv[3], &addr.sin_addr) == 0){
            perror("Inet_pton");
            exit(1);
        }
        addr.sin_port = htons((uint16_t) port);

        if(connect(socket_fd, (const struct sockaddr *) &addr,  sizeof(addr)) == -1){
            perror("Connection");
            exit(1);
        }
        run();
    }

}