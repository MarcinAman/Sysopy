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

#define GET -1
#define RELEASE 1

#define FAILURE_EXIT(format, ...) { printf(format, ##__VA_ARGS__); exit(-1); }

#define PROJECT_PATH getenv("HOME")

#define PROJECT_ID 69

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

key_t generate_key(){
    key_t key = ftok(PROJECT_PATH, PROJECT_ID);
    CHECK_WITH_EXIT(key,-1,"==","Error at generating key\n");

    return key;
}

int get_shared_memory(key_t kij){
    int so_much_memory = shmget(kij,sizeof(struct Barbershop),S_IRWXU | IPC_CREAT);

    CHECK_WITH_EXIT(so_much_memory,-1,"==","Error at creating shared memory\n");

    return so_much_memory;
}

long get_time() {
    struct timespec buffer;
    clock_gettime(CLOCK_MONOTONIC, &buffer);
    return buffer.tv_nsec / 1000;
}

struct sembuf prepare_semaphore(short mode){
    struct sembuf xd;
    xd.sem_num = 0;
    xd.sem_op = mode;
    xd.sem_flg = 0;
    return xd;
}

void get_semaphore(int semaphore_id) {
    struct sembuf xd = prepare_semaphore(GET);
    int res = semop(semaphore_id, &xd, 1);
    CHECK_WITH_EXIT(res,-1,"==","Error while getting semaphore\n");
}

void release_semaphore(int semaphore_id) {
    struct sembuf xd = prepare_semaphore(RELEASE);
    int res = semop(semaphore_id, &xd, 1);
    CHECK_WITH_EXIT(res,-1,"==","Error while releasing semaphore\n");
}

int is_queue_full() {
    if (barbershop->client_count < barbershop->waiting_room_size) return 0;
    return 1;
}

int is_queue_empty() {
    if (barbershop->client_count == 0) return 1;
    return 0;
}

void enter_queue(pid_t pid) {
    barbershop->queue[barbershop->client_count] = pid;
    barbershop->client_count += 1;
}

void pop_queue() {
    for (int i = 0; i < barbershop->client_count - 1; ++i) {
        barbershop->queue[i] = barbershop->queue[i + 1];
    }

    barbershop->queue[barbershop->client_count - 1] = 0;
    barbershop->client_count -= 1;
}

#endif
