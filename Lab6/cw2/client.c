#include "Defines.h"
//TODO when server dies there should be an error on the client site

#define FAILURE_EXIT(format, ...) { printf(format, ##__VA_ARGS__);  mq_close(public_id); mq_close(private_id); mq_unlink(key); exit(-1);}
#define CHECK(current, error,error_message){if(current==error){FAILURE_EXIT(error_message);}}
#define SEND_MESSAGE(){if (mq_send(public_id, m.msg, strlen(m.msg) + 1, 0) == -1) {printf("Error while sending message to server\n");}}
#define GET_COMMAND(){if (fgets(m.content, 240, stdin) == NULL) {printf("Error while getting input\n");}}


int session_id = -1;
int public_id = -1;
int private_id = -1;

int busy = 1;
char key [64];
struct sigevent notify_structure;

struct mq_attr fill_data(){
    struct mq_attr some_random_structure;
    some_random_structure.mq_flags = 0;
    some_random_structure.mq_maxmsg = MAX_MESSAGES;
    some_random_structure.mq_msgsize = MAX_MSG_SIZE;
    some_random_structure.mq_curmsgs = 0;

    return some_random_structure;
}

void login_handler(char* message){
    char* buffer = NULL;
    int current_session_id = (int) strtol(message, &buffer, 0);

    if(current_session_id==0){
        printf("Error while parsing session ID\n");
    }
    else{
        session_id = current_session_id;
    }
}


void executeMessage(char* m){
//    printf("GOT MSG: %s\n",m ); //message is in format 2digits+message 2 digits are commands
    //first digit is a char with command, second one is a client number

    switch (m[0]) {
        case END:
            mq_close(public_id);
            mq_close(private_id);
            mq_unlink(key);
            kill(getpid(),SIGKILL);
            break;
        case INIT:
            login_handler(m);
            break;
        case MIRROR:
            printf("Reversed string: %s\n",m+2);
            break;
        case CALC:
            printf("CALCULATED EXPR: %s\n", m+2);
            break;
        case TIME:
            printf("TIME: %s\n", m+2);
            break;
        default:
            printf("Couldn't match command\n");
    }
}

void handleSIGINT(int a) {
    FAILURE_EXIT("client: killed by INT\n");
}

void handleEXIT() {
    mq_close(public_id);
    mq_close(private_id);
    mq_unlink(key);
}

void handleSIGUSR2(int a){
    char in_buffer0 [MSG_BUFFER_SIZE];

    if (mq_receive (private_id, in_buffer0, MSG_BUFFER_SIZE, NULL) == -1)
        FAILURE_EXIT("Server: mq_receive")

    executeMessage(in_buffer0);

    mq_notify(private_id,&notify_structure);
}

void loop_it(){
    char register_msg[100];

    sprintf (register_msg, "%c%s", LOGIN, key);
    printf("%s\n", register_msg);
    CHECK(mq_send (public_id, register_msg, strlen (register_msg)+1, 0),-1,"Error while sending message to private queue\n")

    notify_structure.sigev_signo = SIGUSR2;
    mq_notify(private_id,&notify_structure);

    struct message m;

    while(busy) {
        printf("COMMAND:\n");
        if(fgets(m.command, 20, stdin) == 0){
            printf("Error while getting command\n");
        }
        else{
            int is_correct = 1;
            int i;
            for(i=0;i<strlen(m.command);i++){
                if(m.command[i]=='\n'){
                    m.command[i]='\0';
                }
                else if(m.command[i]>90 || m.command[i] < 40){
                    printf("Incorrect command, only upper case letters are allowed\n");
                    is_correct = 0;
                }
            }

            if(is_correct==1) {
                if (strcmp(m.command, "MIRROR") == 0) {
                    GET_COMMAND()

                    sprintf(m.msg, "%c%d%s", MIRROR, session_id%CLIENTS_MAX, m.content);

                    SEND_MESSAGE()

                } else if (strcmp(m.command, "CALC") == 0) {
                    printf("Enter expression to calculate: ");

                   GET_COMMAND()

                    sprintf(m.msg, "%c%d%s", CALC, session_id%CLIENTS_MAX, m.content);

                    SEND_MESSAGE()
                } else if (strcmp(m.command, "TIME") == 0) {
                    sprintf(m.msg, "%c%d", TIME, session_id%CLIENTS_MAX);
                    SEND_MESSAGE()

                } else if (strcmp(m.command, "END") == 0) {
                    sprintf(m.msg, "%c%d", END, session_id%10);
                    SEND_MESSAGE()

                    busy = 0;
                }
            }
            else{
                printf("Command not found\n");
            }
        }
    }
}
int main(int argc, char const *argv[]) {
    sprintf (key, "/c%d", getpid());
    struct mq_attr attr = fill_data();

    CHECK(atexit(handleEXIT),-1,"Failed at atexit\n");
    CHECK(signal(SIGINT, handleSIGINT),SIG_ERR,"Failed at SIG_INT handler\n");
    CHECK(signal(SIGUSR2, handleSIGUSR2),SIG_ERR,"Failed at USR_1 hander\n");
    private_id = mq_open (key, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr);
    CHECK(private_id,-1,"Failed at mq_open\n");
    public_id = mq_open (SERVER_QUEUE_NAME, O_WRONLY);
    CHECK(public_id,-1,"Failed at mq_open\n")

    loop_it();

    mq_close(public_id);
    mq_close(private_id);
    mq_unlink(key);
    return 0;
}
