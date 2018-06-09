//
// Created by woolfy on 6/9/18.
//

#include "server.h"

char* name;
enum connection_mode mode;
char* unix_path;
int socket_fd;


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

    data->content[1] = -1;
    if(data->type != error){
        printf("Client computed %d\n",data->content[0]);
    }
}


int main(int argc, char** argv){
    if(argc!=4){
        printf("Wrong number of args");
        return 1;
    }

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
    message message_to_receive;

    while(1){
        while(recv(socket_fd,&message_to_receive, sizeof(message_to_receive),0)>0){
            printf("Client got: %d %d\n",message_to_receive.content[0],message_to_receive.content[1]);

            process_data(&message_to_receive);

            if(send(socket_fd,&message_to_receive,sizeof(message_to_receive),0) == -1){
                perror("Error at sending");
                return 1;
            }

        }
    }

    return 0;
}