#include "Defines.h"

#define FAILURE_EXIT(format, ...) { printf(format, ##__VA_ARGS__); mq_unlink(SERVER_QUEUE_NAME); mq_close(public_id); exit(-1); }
#define CHECK(current, error,error_message){if(current==error){FAILURE_EXIT(error_message);}}
#define SEND(fd,value){if(mq_send(fd,value,strlen(value)+1,0)==-1){printf("Couldn't send message\n");}}
#define OPEN(what,flags,result){if((result=mq_open(what,flags))==-1){printf("Error while opening\n");}}
#define CLOSE(what){mq_close(what);}

int busy = 1;
mqd_t public_id = -1;
int clients_count = 0;

mqd_t clients_data_ids [CLIENTS_MAX];
char clients_names [CLIENTS_MAX][500];

struct sigevent notify_structure;

void swap(char* a, char* b){
    char buffer;
    buffer = *a;
    *a = *b;
    *b = buffer;
}

struct mq_attr fill_data(){
    struct mq_attr some_random_structure;
    some_random_structure.mq_flags = 0;
    some_random_structure.mq_maxmsg = MAX_MESSAGES;
    some_random_structure.mq_msgsize = MAX_MSG_SIZE;
    some_random_structure.mq_curmsgs = 0;

    return some_random_structure;
}

void Mlogin(char* m){
        char out_buffer [MSG_BUFFER_SIZE];
        mqd_t tmp;
        OPEN(m+1,O_WRONLY,tmp);
        if(clients_count > CLIENTS_MAX - 1){
            sprintf (out_buffer, "%c%d", INIT, -1);
        }else{
            clients_data_ids[clients_count] = tmp;
            strcpy(clients_names[clients_count], m+1);
            sprintf (out_buffer, "%c%d",INIT,clients_count);
            clients_count++;
        }
        SEND(tmp,out_buffer);
        printf("Send\n");
        CLOSE(tmp);
}

void invert(char * m){
    int i;
    int len = (int) strlen(m);
    if(m[len-1] == '\n') len--;
    for(i = 0; i<len/2; i++)
        swap(&m[i],&m[len-i-1]);
}
void Mmirror(char* m){
    int client_id = m[1] - 48;
    invert((m+2));
    char out_buffer [MSG_BUFFER_SIZE];
    mqd_t tmp;
    OPEN(clients_names[client_id],O_WRONLY,tmp);
    sprintf (out_buffer, "%s",m);
    SEND(tmp,out_buffer);
}
void Mcalc(char* m){
    FILE* calc;
    char command[4096];
    int client_id = (int)m[1] - 48;
    char out_buffer [MSG_BUFFER_SIZE];
    mqd_t tmp;
    OPEN(clients_names[client_id],O_WRONLY,tmp);

    sprintf(command, "echo '%s' | bc", m+2);
    calc = popen(command, "r");
    fgets(m+2, 240, calc);
    pclose(calc);

    sprintf (out_buffer, "%s",m);
    printf("%s\n", out_buffer);
    SEND(tmp,out_buffer);
    printf("%s\n", "Send");
}

void Mtime(char* m){

    int id = m[1]-48;
    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    sprintf(m,"%c%c%s",m[0],m[1],asctime(timeinfo));


    mqd_t mq_handler = mq_open (clients_names[id], O_WRONLY);
    CHECK(mq_handler,(mqd_t)-1,"Error while opening client queue\n");

    if(mq_send(mq_handler, m, strlen(m)+1, 0) == -1){
        perror("time:");
        FAILURE_EXIT("server: TIME response failed\n");
    }
    printf("%s\n", "Send");
}
void Mend(char * m){
    mqd_t tmp;
    char endmsg [2];
    sprintf(endmsg,"%c", END);
    int i=0;
    for(i=0;i<clients_count;i++){
        if(i!=(int)m[1] - 48){
            OPEN(clients_names[i],O_WRONLY,tmp);
            SEND(tmp,endmsg);
            CLOSE(tmp);
        }
    }
}
void executeMessage(char* m){
    switch (m[0]) {
        case LOGIN:
            Mlogin(m);
            break;
        case END:
            Mend(m);
            busy = 0; //stops server
            break;
        case MIRROR:
            Mmirror(m);
            break;
        case CALC:
            Mcalc(m);
           break;
        case TIME:
            Mtime(m);
            break;
        default:
            busy =0;
            break;
    }
}


void handleSIGINT(int a) {
    FAILURE_EXIT("client: killed by INT\n");
}
void handleEXIT() {
    mq_close(public_id);
    mq_unlink(SERVER_QUEUE_NAME);
}

void handleSIGUSR1(){
    char in_buffer0 [MSG_BUFFER_SIZE];

    if (mq_receive (public_id, in_buffer0, MSG_BUFFER_SIZE, NULL) == -1)
        FAILURE_EXIT("Server: mq_receive")
    printf ("Server: message received: %s\n", in_buffer0);

    executeMessage(in_buffer0);

    mq_notify(public_id,&notify_structure);
}

int main(int argc, char const *argv[]) {
    struct mq_attr attr = fill_data();

    if (atexit(handleEXIT) == -1)
        FAILURE_EXIT("server: failed to register atexit()\n")
    if(signal(SIGINT, handleSIGINT) == SIG_ERR)
        FAILURE_EXIT("server: failed to register INT handler\n")
    if(signal(SIGUSR1, handleSIGUSR1) == SIG_ERR)
        FAILURE_EXIT("server: failed to register USR1 handler\n")
    if ((public_id = mq_open (SERVER_QUEUE_NAME, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)) == -1)
        FAILURE_EXIT("Server: mq_open (server)")


    notify_structure.sigev_signo = SIGUSR1;
    mq_notify(public_id,&notify_structure);

    while(busy) {
        //do epic things!
    }
    CLOSE(public_id);
    mq_unlink(SERVER_QUEUE_NAME);
    return 0;
}
