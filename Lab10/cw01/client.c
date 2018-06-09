//
// Created by woolfy on 6/9/18.
//

#include "server.h"

char* name;
enum connection_mode mode;
char* unix_path;
int socket_fd;
const char* xd = "Klient do servera!";
char buffer[100];


char* process_data(char* data){
    int i,spaces = 0;
    for(i=0;i<strlen(data);i++){
        if(data[i]==' ') spaces++;
    }

    char** expression_array = malloc(spaces*sizeof(char*));
    i=0;
    char* buffer;
    buffer = strtok(data," ");
    while(buffer!=NULL){
        expression_array[i] = malloc(sizeof(char)*)
        buffer = strtok(NULL," ");
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


    while(1){
        if(recv(socket_fd,buffer, sizeof(buffer),0)>0){
            printf("Server send: %s\n",buffer);

            if(send(socket_fd,xd,strlen(xd),0) == -1){
                perror("Error at sending");
                return 1;
            }

        }
    }

    return 0;
}