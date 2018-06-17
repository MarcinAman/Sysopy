#ifndef HAIRDRESSERS_H
#define HAIRDRESSERS_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <semaphore.h>
#include <sys/ipc.h>
#include <fcntl.h>
#include <sys/mman.h>

#define FAILURE_EXIT(format, ...) { printf(format, ##__VA_ARGS__); exit(-1); }

#define PROJECT_PATH "/BATAT3"

#define MAX_QUEUE_SIZE 33

#define CHECK_WITH_EXIT(value,error,sign,message){                       \
    if(strcmp(sign,"==")==0 && value == error){                          \
        FAILURE_EXIT(message);                                           \
    }                                                                    \
    else if(strcmp(sign,"!=")==0 && value != error){                     \
        FAILURE_EXIT(message);                                           \
    }                                                                    \
    else if(strcmp(sign,">")==0 && value > error){                       \
        FAILURE_EXIT(message);                                           \
    }                                                                    \
}

#define BARBER_SLEEP printf("%lo No customers -> barber is falling asleep\n",get_time()); barbershop->barber_status = SLEEPING;
#define BARBER_SERVE invite_client(); barbershop->barber_status = READY;
#define BARBER_READY printf("%lo Barber is awaken\n",get_time()); barbershop->barber_status = READY;
#define BARBER_BUSY cut_clients_head(); barbershop->barber_status = READY;

#define TRUC_FILE int xd = ftruncate(shared_memory_id, sizeof(*barbershop));CHECK_WITH_EXIT(xd,-1,"==","Error at truncating file\n");
#define GETBARBERSHOP(){ barbershop = mmap(NULL, sizeof(*barbershop), PROT_READ | PROT_WRITE, MAP_SHARED, shared_memory_id,0);}

#define get_semaphore(semaphore_id){int res = sem_wait(semaphore_id);CHECK_WITH_EXIT(res,-1,"==","Error while getting semaphore_id\n");}
#define release_semaphore(semaphore_id){int res = sem_post(semaphore_id);CHECK_WITH_EXIT(res,-1,"==","Error while releasing semaphore_id\n");}

#define enter_queue(pid){barbershop->queue[barbershop->client_count] = pid; barbershop->client_count += 1;}
#define pop_queue(){for (int i = 0; i < barbershop->client_count - 1; ++i) {\
                            barbershop->queue[i] = barbershop->queue[i + 1]; \
                            } \
                        barbershop->queue[barbershop->client_count - 1] = 0; \
                        barbershop->client_count -= 1;}

enum Barber_status {
    SLEEPING,
    AWOKEN,
    READY,
    IDLE,
    BUSY
};

enum Client_status {
    NEWCOMER,
    INVITED,
    SHAVED
};

struct Barbershop {
    enum Barber_status barber_status;
    int client_count;
    int waiting_room_size;
    pid_t selected_client;
    pid_t queue[MAX_QUEUE_SIZE];
} *barbershop;


//Creating memory but only for barber since it has O_CREAT flag
int get_shared_memory(){
    int so_much_memory = shm_open(PROJECT_PATH,O_RDWR | O_CREAT | O_EXCL,S_IRWXU | S_IRWXG);

    CHECK_WITH_EXIT(so_much_memory,-1,"==","Error at creating shared memory\n");

    return so_much_memory;
}

long get_time() {
    struct timespec asd;
    clock_gettime(CLOCK_MONOTONIC, &asd);
    return asd.tv_nsec / 1000;
}


int is_queue_full() {
    if (barbershop->client_count < barbershop->waiting_room_size) {
        return 0;
    }
    return 1;
}

int is_queue_empty() {
    if (barbershop->client_count == 0){
        return 1;
    }
    return 0;
}

#endif
