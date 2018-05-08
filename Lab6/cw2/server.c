#include "Defines.h"

#define CLIENTS_MAX 10
#define FAILURE_EXIT(format, ...) { printf(format, ##__VA_ARGS__); mq_unlink(SERVER_QUEUE_NAME); mq_close(public_id); exit(-1); }

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

void Mlogin(char* m){
        char out_buffer [MSG_BUFFER_SIZE];
        mqd_t tmp;
        if ((tmp = mq_open ((m+1), O_WRONLY)) == 1) {
            perror ("Server: Not able to open client queue");
        }
        if(clients_count > CLIENTS_MAX - 1){
            sprintf (out_buffer, "%c%d", INIT, -1);
        }else{
            clients_data_ids[clients_count] = tmp;

            strcpy(clients_names[clients_count], m+1);

            printf("%s\n",clients_names[clients_count] );
            sprintf (out_buffer, "%c%d",INIT,clients_count);
            clients_count++;
        }
        printf("%s\n", out_buffer);
        if( mq_send (tmp , out_buffer, strlen(out_buffer)+1, 0)<0)
            FAILURE_EXIT("CANT SEND ;-;");
        printf("send\n");
        mq_close(tmp);
}
void invert(char * m){
    int i;
    int len = (int) strlen(m);
    if(m[len-1] == '\n') len--;
    for(i = 0; i<len/2; i++)
        swap(&m[i],&m[len-i-1]);
}
void Mmirror(char* m){
    int client_id = (int)m[1] - 48;
    invert((m+2));
    char out_buffer [MSG_BUFFER_SIZE];
    mqd_t tmp;
    if ((tmp = mq_open (clients_names[client_id], O_WRONLY)) == 1) {
        perror ("Server: Not able to open client queue");
    }
    sprintf (out_buffer, "%s",m);
    printf("%s\n", out_buffer);
    if(mq_send(tmp, out_buffer, strlen(out_buffer)+1, 0) == -1){
        perror("mirror:");
        FAILURE_EXIT("server: MIRROR response failed\n");
    }
    printf("%s\n", "Send");
}
void Mcalc(char* m){
    FILE* calc;
    char command[4096];
    int client_id = (int)m[1] - 48;
    char out_buffer [MSG_BUFFER_SIZE];
    mqd_t tmp;
    if ((tmp = mq_open (clients_names[client_id], O_WRONLY)) == 1) {
        perror ("Server: Not able to open client queue");
    }
    sprintf(command, "echo '%s' | bc", m+2); // #EMEJZING *o*
    calc = popen(command, "r");
    fgets(m+2, 240, calc);
    pclose(calc);

    sprintf (out_buffer, "%s",m);
    printf("%s\n", out_buffer);
    if(mq_send(tmp, out_buffer, strlen(out_buffer)+1, 0) == -1){
        perror("clac:");
        FAILURE_EXIT("server: CALC response failed\n");
    }
    printf("%s\n", "Send");
}

char* convert_time(const time_t* time) {
    char* buffer = malloc(sizeof(char) * 30);
    struct tm* time_info;
    time_info = localtime(time);
    strftime(buffer, 20, "%b %d %H:%M", time_info);
    return buffer;
}
void Mtime(char* m){
    time_t timer;
    int client_id = (int)m[1] - 48;
    time(&timer);
    sprintf(m, "%c%c%s", m[0],m[1], convert_time(&timer));
    mqd_t tmp;
    if ((tmp = mq_open (clients_names[client_id], O_WRONLY)) == 1) {
        perror ("Server: Not able to open client queue");
    }
    if(mq_send(tmp, m, strlen(m)+1, 0) == -1){
        perror("time:");
        FAILURE_EXIT("server: TIME response failed\n");
    }
    printf("%s\n", "Send");
}
void Mend(char * m){
    mqd_t tmp;
    char endmsg [2];
    int i=0;
    sprintf(endmsg,"%c", END);
    int w = (int)m[1] - 48;
    printf("IGNORE: %d\n", w);
    for(;i<clients_count;i++){
        if(i!=w){
            if ((tmp = mq_open (clients_names[i], O_WRONLY)) == 1) {
                perror ("Server: Not able to open client queue");
            }
            if(mq_send(tmp, endmsg, strlen(endmsg)+1, 0) == -1){
                perror("end:");
            }
            mq_close(tmp);
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


//////////////////HANDLERS///////////////////////////////
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
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MESSAGES;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

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
    while(busy) {}
    mq_close(public_id);
    mq_unlink(SERVER_QUEUE_NAME);
    return 0;
}
