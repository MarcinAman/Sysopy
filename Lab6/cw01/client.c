#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include "utils.h"
#include <errno.h>
#define FAILURE_EXIT(format, ...) { printf(format, ##__VA_ARGS__); exit(-1); }
#define SEND_AND_RECEIVE if(msgsnd(public_id, m, MESSAGE_SIZE, 0) == -1) FAILURE_EXIT("Request failed!"); if(msgrcv(private_id, m, MESSAGE_SIZE, 0, 0) == -1) FAILURE_EXIT("Response failed!");


int session_id = -1;
int public_id = -1;
int private_id = -1;

void handleSIGINT(int a) {
    FAILURE_EXIT("client: killed by INT\n");
}

void handleEXIT() {
    if(private_id <= -1) return;
    if (msgctl(private_id, IPC_RMID, 0) == -1) {
        printf("client: there was some error deleting queue\n");
        return;
    }
    printf("client: queue deleted successfully\n");
}
void register_client(key_t private_key) {
    Message m;
    m.type = LOGIN;
    m.sender_pid = getpid();
    sprintf(m.content, "%d", private_key);
    if(msgsnd(public_id, &m, MESSAGE_SIZE, 0) == -1){
        printf("client: LOGIN request failed\n");
    }
    if(msgrcv(private_id, &m, MESSAGE_SIZE, 0, 0) == -1)
        FAILURE_EXIT("client: receiving LOGIN response failed\n");
    if(sscanf(m.content, "%d", &session_id) < 1)
        FAILURE_EXIT("client: scanning LOGIN response failed\n");
    if(session_id < 0)
        FAILURE_EXIT("client: server reached maximum clients capacity\n");
    printf("client: registered with session no %i\n", session_id);
}
void mirror_handler(Message *m) {
    m->type = MIRROR;
    printf("Command to mirror:\n");
    if(fgets(m->content, CONTENT_SIZE, stdin) == 0) {
        printf("Error from fgets or empty input\n");
        return;
    }

    SEND_AND_RECEIVE

    printf("%s", m->content);
}

void calc_handler(Message *m) {
    m->type = CALC;
    printf("Enter expression to calculate: ");
    if(fgets(m->content, CONTENT_SIZE, stdin) == 0) {
        printf("Too many characters!\n");
        return;
    }
    SEND_AND_RECEIVE

    printf("%s", m->content);
}

void time_handler(Message *m) {
    m->type = TIME;

    SEND_AND_RECEIVE

    printf("%s\n", m->content);
}

void end_handler(Message *m) {
    m->type = END;

    if(msgsnd(public_id, m, MESSAGE_SIZE, 0) == -1)
        FAILURE_EXIT("client: END request failed\n");
}

int main(int argc, char** argv) {
    char* path;
    char command[100];
    Message m;
    if (atexit(handleEXIT) == -1)
        FAILURE_EXIT("client: failed to register atexit()\n")
    if(signal(SIGINT, handleSIGINT) == SIG_ERR)
        FAILURE_EXIT("client: failed to register INT handler\n")
    if((path=getenv("HOME")) == 0)
        FAILURE_EXIT("client: failed to obtain value of $HOME\n")

    key_t key = ftok(path, PROJECT_ID);
    if(key == -1)
        FAILURE_EXIT("client: failed to generate key\n")
    public_id = msgget(key, 0);
    if (public_id == -1)
        FAILURE_EXIT("client: failed to open server queue\n")
    key = ftok(path, getpid());
    if(key == -1)
        FAILURE_EXIT("client: failed to generate key\n")
    private_id = msgget(key, IPC_CREAT | IPC_EXCL | 0666);
    if (private_id == -1)
        FAILURE_EXIT("client: failed to open queue2\n")
    register_client(key);

    while(1) {
        m.sender_pid = getpid();
        printf("Command:\n");
        if(fgets(command, 100, stdin) == 0)
            printf("ERROR while reading command\n");

        if(command[strlen(command)-1] == '\n')
            command[strlen(command)-1] = 0;

        if(strcmp(command, "MIRROR") == 0)
            mirror_handler(&m);
        if (strcmp(command, "CALC") == 0)
            calc_handler(&m);
        if (strcmp(command, "TIME") == 0)
            time_handler(&m);
        if (strcmp(command, "END") == 0)
            end_handler(&m);
        if (strcmp(command, "STOP") == 0)
            exit(EXIT_SUCCESS);
    }
}
