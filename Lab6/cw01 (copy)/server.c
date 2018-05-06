//
// Created by marcinaman on 4/25/18.
//

#include "Defines.h"

#define setup_SIGINT(){if(signal(SIGINT,sigint_handler)==SIG_ERR){ printf("Failed to add response to SIGINT, Terminating...\n");exit(1);}}
#define setup_atexit(){if(atexit(atexit_function)==-1){printf("Failed to add atexit function, Terminatin...\n");exit(1);}}
#define setup(){setup_SIGINT();setup_atexit();}

#define CHECK_SENDER  if(sender_pid==-1){ printf("Couldn't find that client in array, passing that message\n"); return;}
#define SEND_MESSAGE  if(msgsnd(sender_pid,m,sizeof(m),0)==-1){printf("Failed to send message to %d,passing...\n",sender_pid);}

int public_queue_id = -1; //Undefined
int server_available = 0;

int clients_count = 0;
int clients_data [MAX_CLIENTS][2];

void atexit_function(){
    if(public_queue_id==-1){
        return;
    }
    if(msgctl(public_queue_id,IPC_RMID,0)==-1){
        printf("There was an error exiting queue\n");
    }
}

void sigint_handler(int a){
    atexit_function();
    exit(1);
}

pid_t get_client_from_message(struct message* m){
    int i=0;
    for(i=0;i<MAX_CLIENTS;i++) {
        if(clients_data[i][0]==m->pid) return clients_data[i][1];
    }
    return -1;
}

char *strrev(char str[])
{
    int i = (int) (strlen(str) - 1), j = 0;

    char ch;
    while (i > j)
    {
        ch = str[i];
        str[i] = str[j];
        str[j] = ch;
        i--;
        j++;
    }
    return str;
}

void mirror_handler(struct message* m,pid_t sender_pid){
    CHECK_SENDER

    //Reversing a string because strrev is available on linux ;F

    int i = (int) (strlen(m->mtext) - 1), j = 0;
    char ch;
    while (i > j)
    {
        ch = m->mtext[i];
        m->mtext[i] = m->mtext[j];
        m->mtext[j] = ch;
        i--;
        j++;
    }

    SEND_MESSAGE
}

void time_handler(struct message* m, pid_t sender_pid){
    CHECK_SENDER

    time_t* raw_time = NULL;
    time(raw_time);
    struct tm *info;
    info = localtime(raw_time);

    sprintf(m->mtext,"%s",asctime(info));

    SEND_MESSAGE
}

void calc_handler(struct message* m, pid_t sender_pid){
    CHECK_SENDER

    FILE* calc;
    char command[CONTENT_MAX];
    sprintf(command, "echo '%s' | bc", m->mtext); // #EMEJZING *o*
    calc = popen(command, "r");
    fgets(m->mtext, CONTENT_MAX, calc);
    pclose(calc);


    SEND_MESSAGE
}

void login_handler(struct message* m){
    key_t clientkey;
    int sender_pid;

    if(sscanf(m->mtext, "%d", &clientkey) < 0)
        printf("server: reading client_key failed\n");
    if((sender_pid =  msgget(clientkey, 0)) == -1)
        printf("server: reading client_queue_id failed\n");
    if(clients_count > MAX_CLIENTS - 1){ //dlaczego -1
        printf("Maximum amount of clients reached!\n");
        sprintf(m->mtext, "%d", -1);
    }else{
        clients_data[clients_count][0] = m->pid;
        clients_data[clients_count++][1] = sender_pid;
        sprintf(m->mtext, "%d", clients_count - 1); //dlaczego -1
    }
    m->mtype = INIT;
    m->pid = getpid();
    printf("CLENT Q ID: %d\n", sender_pid);

    SEND_MESSAGE
}


void parse_message(struct message* message1){
    if(message1->mtype == LOGIN){
        login_handler(
                message1
                );
    }
    else if(message1->mtype == MIRROR){

        mirror_handler(
                message1,
                get_client_from_message(message1)
        );
    }
    else if(message1->mtype == CALC){
        calc_handler(
                message1,
                get_client_from_message(message1)
        );
    }
    else if(message1->mtype == TIME){
        time_handler(
                message1,
                get_client_from_message(message1)
        );
    }
    else if(message1->mtype == END){
        server_available = 1;
    }
    else{
        //error
    }
}

int main(int argc, char** argv){
    setup();

    char* path = getenv("HOME");

    if(path == 0){
        printf("Fail at getting path, terminating ...\n");
        exit(1);
    }
    printf("%s\n",path);

    key_t key = ftok(path,ID);

    if(key==-1){
        printf("Failed to generate a key, terminating...\n");
        exit(1);
    }

    printf("Generated key: %d\n",key);

    if((public_queue_id = msgget(key, IPC_CREAT | IPC_EXCL | 0666))== -1){
        printf("Failed to create public queue, terminating...\n");
        exit(1);
    }

    printf("Created public queue with ID: %d\n",public_queue_id);

    while(1){
        struct msqid_ds current;
        struct message message1;

        if(server_available == 1){
            if(msgctl(public_queue_id,IPC_STAT,&current)==-1){
                printf("Failed to get current state");
                exit(1);
            }
            if(current.msg_qnum==0){
                break;
            }
        }
        if(msgrcv(public_queue_id,&message1,CONTENT_MAX,0,0)==-1){
            printf("Failed to fetch message\n");
        } else{
            parse_message(&message1);
        }

    }

    return 0;

}

