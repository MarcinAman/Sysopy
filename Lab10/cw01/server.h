//
// Created by woolfy on 6/9/18.
//

#ifndef LAB1_MAIN_H
#define LAB1_MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <sys/socket.h>
#include <pthread.h>
#include <string.h>
#include <stddef.h>
#include <netinet/udp.h>
#include <sys/un.h>
#include <signal.h>
#include <semaphore.h>

#define MAX_CLIENTS 20
const int BUFFER_LEN = 100;
const char* current_path = "/home/woolfy/Sysopy/Lab10/cw01/socket";
const int MAX_NAME_LEN = 20;

typedef struct Socket Socket;
typedef struct Epoll Epoll;
typedef struct Server Server;
typedef struct Client Client;
typedef struct message message;

struct Socket{
    int fd;
    int name;
};

struct Epoll{
    int fd;
    struct epoll_event* event;
};

struct Server{
    int port;
    char* socket_path;
};

struct Client{
    int fd;
    char name[30];
    int is_busy;
    int is_pinged;
};

enum connection_mode{
    local_unix,net
};

enum message_type{
    login,logout,add,sub,mul,res,error,ping
};

struct message{
    int id;
    enum message_type type;
    int content[2];
    char name[20];
};


#endif //LAB1_MAIN_H
