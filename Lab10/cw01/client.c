//
// Created by woolfy on 6/9/18.
//

#include "server.h"

char* name;
enum connection_mode mode;
char* unix_path;
int socket_fd;

void send_last_message(){
    printf("Sending logout message to server\n");
    message message;
    message.type = logout;
    strcpy(message.name,name);

    if(send(socket_fd,&message,sizeof(message),0)==-1){
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


int main(int argc, char** argv){
    if(argc!=4){
        printf("Wrong number of args");
        return 1;
    }

//    atexit(send_last_message);

    struct sigaction sigact;
    sigemptyset(&sigact.sa_mask);
    sigact.sa_handler = handleSignal;
    sigaction(SIGINT, &sigact, NULL);

    name = argv[1];
    if(strcmp(argv[2],"net")==0){
        mode = net;
    }
    else{
        mode = local_unix;
    }

    socket_fd = socket(AF_UNIX,SOCK_STREAM,0);
    if(socket_fd == -1){
        perror("Not able to create socket");
        return 1;
    }

    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strncpy (addr.sun_path, current_path, sizeof (addr.sun_path));

    if(connect(socket_fd,(struct sockaddr*)&(addr),sizeof(struct sockaddr_un)) == -1){
        perror("Couldnt connect to server");
        return 1;
    }
    message logging_message;
    message message_to_receive;

    logging_message.type = login;
    strcpy(logging_message.name,name);
    printf("Connected new client with name: %s\n",message_to_receive.name);

    if(send(socket_fd,&logging_message,sizeof(logging_message),0) == -1){
        perror("Error at sending");
        return 1;
    }

    printf("Client send a message to log in\n");

    while(1){
        while(recv(socket_fd,&message_to_receive, sizeof(message_to_receive),0)>0){
            printf("Client got: %d %d\n",message_to_receive.content[0],message_to_receive.content[1]);

            process_data(&message_to_receive);
            strcpy(message_to_receive.name,name);

            if(send(socket_fd,&message_to_receive,sizeof(message_to_receive),0) == -1){
                perror("Error at sending");
                return 1;
            }
        }
    }
}