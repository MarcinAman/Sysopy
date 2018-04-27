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


#define INIT 0
#define LOGIN 1
#define MIRROR 2
#define CALC 3
#define TIME 4
#define STOP 5
#define END 6

#define ID 12345
#define CONTENT_MAX

struct message{
    int command;
    pid_t pid;
    char content[CONTENT_MAX];
};

#endif //CW01_DEFINES_H
