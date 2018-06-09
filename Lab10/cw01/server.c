//
// Created by woolfy on 6/9/18.
//


#include <signal.h>
#include "server.h"

Epoll epoll;
Socket socket_data;
Client clients[MAX_CLIENTS];
int clients_number = 0;
Server server;

const char* test_string = "Server do Klienta, elo!\n";


void clean(){
    close(socket_data.fd);

    if(epoll.fd != -1){
        close(epoll.fd);
    }
}

void handleSignal(int singno){

    clean();
}

/*ftp://ftp.gnu.org/old-gnu/Manuals/glibc-2.2.3/html_chapter/libc_16.html*/

int make_named_socket (const char *filename){
    struct sockaddr_un name;
    int sock;
    /* Create the socket. */

    /* Normally only a single protocol exists to support a particular socket type within a given protocol  family,  in  which  case
   protocol  can be specified as 0. */

    sock = socket (AF_UNIX, SOCK_STREAM, 0);
    if (sock < 0)
    {
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
}


void run(){
    struct sockaddr_in client_address;
    socklen_t addr_len = 0;

    while(1){
        int acc;
        if((acc = accept(socket_data.fd,(struct sockaddr*)&client_address,&addr_len))<0){
            perror("Error at accepting");
            exit(EXIT_FAILURE);
        }

        if(send(acc,test_string,strlen(test_string),0)==-1){
            perror("Error at sending");
        }

        ssize_t n = 0;
        char buffer[BUFFER_LEN];

        if ((n = recv(acc, &buffer,BUFFER_LEN, 0)) > 0) {

            printf("received: '%s'\n", buffer);
        }

        close(acc);
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


    epoll.fd = epoll_create1(0);
    if(epoll.fd == -1){
        perror("Error at epoll_create1");
        exit(EXIT_FAILURE);
    }

    socket_data.fd = make_named_socket(current_path);

    if(listen(socket_data.fd,MAX_CLIENTS) == -1){
        perror("Listen");
        exit(EXIT_FAILURE);
    }

    run();

    return 0;
}
