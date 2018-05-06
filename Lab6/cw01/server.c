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

#define CLIENTS_MAX 100
#define FAILURE_EXIT(format, ...) { printf(format, ##__VA_ARGS__); exit(-1); }
#define CHECK_SENDER  if(sender_pid==-1){ printf("Couldn't find that client in array, passing that message\n"); return;}
#define SEND_MESSAGE  if(msgsnd(sender_pid,m,MESSAGE_SIZE,0)==-1){printf("Failed to send message to %d,passing...\n",sender_pid);}


int busy = 1;
int public_id = -1;
int clients_count = 0;
int clients_data [CLIENTS_MAX][2];

void handleSIGINT(int a) {
    FAILURE_EXIT("client: killed by INT\n");
}
void handleEXIT() {
    if(public_id <= -1) return;
    if (msgctl(public_id, IPC_RMID, 0) == -1) {
        printf("An error occured while sending IPC_RMID\n");
    }else{
        printf("Queue deleted from handleEXIT\n");
    }
}

pid_t get_client_from_message(Message* m){
    int i=0;
    for(i=0;i<CLIENTS_MAX;i++) {
        if(clients_data[i][0]==m->sender_pid) return clients_data[i][1];
    }
    return -1;
}

void mirror_handler(Message* m,pid_t sender_pid){
    CHECK_SENDER

    //Reversing a string because strrev is available on linux ;F

    int i = (int) (strlen(m->content) - 1), j = 0;
    char ch;
    while (i > j)
    {
        ch = m->content[i];
        m->content[i] = m->content[j];
        m->content[j] = ch;
        i--;
        j++;
    }

    SEND_MESSAGE
}

void time_handler(Message* m, pid_t sender_pid){
    CHECK_SENDER

    time_t rawtime;
    struct tm * timeinfo;

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );

    sprintf(m->content,"%s",asctime(timeinfo));

    SEND_MESSAGE
}

void calc_handler(Message* m, pid_t sender_pid){
    CHECK_SENDER

    FILE* calc;
    char command[CONTENT_SIZE];
    sprintf(command, "echo '%s' | bc", m->content); // #EMEJZING *o*
    calc = popen(command, "r");
    fgets(m->content, CONTENT_SIZE, calc);
    pclose(calc);


    SEND_MESSAGE
}

void login_handler(Message* m){
    key_t clientkey;
    int sender_pid;

    if(sscanf(m->content, "%d", &clientkey) < 0)
        printf("server: reading client_key failed\n");
    if((sender_pid =  msgget(clientkey, 0)) == -1)
        printf("server: reading client_queue_id failed\n");
    if(clients_count > CLIENTS_MAX - 1){ //dlaczego -1
        printf("Maximum amount of clients reached!\n");
        sprintf(m->content, "%d", -1);
    }else{
        clients_data[clients_count][0] = m->sender_pid;
        clients_data[clients_count++][1] = sender_pid;
        sprintf(m->content, "%d", clients_count - 1); //dlaczego -1
    }
    m->type = INIT;
    m->sender_pid = getpid();
    printf("CLENT Q ID: %d\n", sender_pid);

    SEND_MESSAGE
}

void executeMessage(Message* message1){
    if(message1->type == LOGIN){
        login_handler(
                message1
        );
    }
    else if(message1->type == MIRROR){

        mirror_handler(
                message1,
                get_client_from_message(message1)
        );
    }
    else if(message1->type == CALC){
        calc_handler(
                message1,
                get_client_from_message(message1)
        );
    }
    else if(message1->type == TIME){
        time_handler(
                message1,
                get_client_from_message(message1)
        );
    }
    else if(message1->type == END){
        busy = 0;
    }
    else{
        //error
    }
}
int main(int argc, char const *argv[]) {
    char * path;
    key_t key;
    Message message;
    struct msqid_ds currentState;
    if (atexit(handleEXIT) == -1)
        FAILURE_EXIT("server: failed to register atexit()\n")
    if(signal(SIGINT, handleSIGINT) == SIG_ERR)
        FAILURE_EXIT("server: failed to register INT handler\n")
    if((path = getenv("HOME")) == 0)
        FAILURE_EXIT("server: failed to obtain value of $HOME\n");
    if((key= ftok(path, PROJECT_ID)) == -1)
        FAILURE_EXIT("server: failed to generate public_key\n");
    if((public_id = msgget(key, IPC_CREAT | IPC_EXCL | 0666))== -1)
        FAILURE_EXIT("server: failed to create public queue\n");

    while(1) {
        if(busy == 0) {
            if(msgctl(public_id, IPC_STAT, &currentState) == -1)
                FAILURE_EXIT("server: getting current state of failed\n");
            if(currentState.msg_qnum == 0) break;
        }
        if(msgrcv(public_id, &message, MESSAGE_SIZE, 0, 0) < 0)
            FAILURE_EXIT("server: failed to receive message\n");
        executeMessage(&message);
    }
    return 0;
}
