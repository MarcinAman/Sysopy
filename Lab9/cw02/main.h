//
// Created by woolfy on 6/7/18.
//

#ifndef LAB1_MAIN_H
#define LAB1_MAIN_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <semaphore.h>

typedef struct Data Data;

struct Data {
    int    is_extended;
    int    length;
    int    N;
    int    producers;
    int    consumers;
    int    nk;
    int    consumer_index;    // read pointer
    int    producer_index;    // write pointer
    char** arr;
    FILE*  file;
    int    (*cmp)(int, int);
    int    full;
    int    count;
    sem_t* mutexes;
    pthread_t *pthread;
};

void* producer(void*);
void* consumer(void*);

int greater(int,int);
int lower(int,int);
int equal(int, int);

void cleanup_data(Data*);
void parse_file(Data*,char*);

#endif //LAB1_MAIN_H
