#ifndef __UTILS__
#define __UTILS__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <mqueue.h>
#include <sys/ipc.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

#define SERVER_QUEUE_NAME "/ASD"
#define QUEUE_PERMISSIONS 0660
#define MAX_MESSAGES 10
#define CLIENTS_MAX 10
#define MAX_MSG_SIZE 256
#define MSG_BUFFER_SIZE MAX_MSG_SIZE + 10

#define LOGIN  '1'
#define MIRROR '2'
#define CALC   '3'
#define TIME   '4'
#define END    '5'
#define INIT   '6'

#define MAX_COMMAND 20
#define MAX_MSG 100

struct message{
    char command[MAX_COMMAND];
    char msg[MAX_MSG];
    char content[MAX_MSG_SIZE];
};

struct IDs{
    int session_id;
    int public_id;
    int private_id;
};

#endif
