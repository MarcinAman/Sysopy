//
// Created by marcinaman on 4/25/18.
//

#ifndef CW01_DEFINES_H
#define CW01_DEFINES_H

#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <errno.h>


#define INIT 7
#define LOGIN 1
#define MIRROR 2
#define CALC 3
#define TIME 4
#define STOP 5
#define END 6

#define ID 12
#define CONTENT_MAX 255

struct message{
    int mtype;
    pid_t pid;
    char mtext[CONTENT_MAX];
};


//Server defines:
#define MAX_CLIENTS 100

#endif //CW01_DEFINES_H
