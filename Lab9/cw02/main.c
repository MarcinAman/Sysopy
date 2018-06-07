//
// Created by woolfy on 6/7/18.
//
#include "main.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

sem_t mutex;
Data global_data;

int greater(int a, int b) {
    return a > b ? 1 : 0;
}
int lower(int a, int b) {
    return a < b ? 1 : 0;
}
int equal(int a, int b) {
    return a == b ? 1 : 0;
}

void handleSignal(int sig){
    int i;
    for (i = 0; i < (global_data.producers + global_data.consumers); ++i) {
        pthread_cancel(global_data.pthread[i]);
    }

    for(i=0;i<global_data.N;i++){
        sem_destroy(&(global_data.mutexes[i]));
    }

    sem_destroy(&mutex);

    free(global_data.arr);
    free(global_data.mutexes);
    fclose(global_data.file);
    exit(EXIT_SUCCESS);
}

void cleanup_data(Data* data) {
    int i;
    for(i=0;i<data->N;i++){
        sem_destroy(&(data->mutexes[i]));
    }
    sem_destroy(&mutex);

    free(data->arr);
    free(data->mutexes);
    fclose(data->file);
}

void *producer(void *args) {
    fflush(stdout);
    Data *data = (Data*)(args);

    int is_finished = 0;

    while(is_finished==0){
        if (data->is_extended) {
            printf("P: %ld Starting new producer task\n",pthread_self());
        }

        /* take mutex for file */
        if(sem_wait(&mutex)!=0){
            perror("error while waiting for sem mutex");
            exit(EXIT_FAILURE);
        }

        int x=0,i;
        for(i=0;i<data->N;i++){
            if(data->arr[i]==NULL) x++;
        }

        if(x == 0){
            data->full = 1;
        }

        while(data->full == 1){
            sem_post(&mutex);
            sem_wait(&mutex);
        }

        if (data->is_extended) {
            printf("P: %ld Got a mutex\n",pthread_self());
        }

        char* line = NULL;
        size_t linecap = 0;

        /*get new line from file */
        if ((getline(&line, &linecap, data->file)) < 0) {
            printf("P: %ld got eof\n",pthread_self());
            is_finished = 1;
        }

        if (data->is_extended) {
            printf("P: %ld read from file: %s", pthread_self(), line);
        }

        int index = data->producer_index;

        /*get mutex for current element */
        if(sem_wait(&(data->mutexes[data->producer_index]))!=0){
            perror("Error while waiting for array sem");
            exit(EXIT_FAILURE);
        }

        /* save data */
        data->arr[index] = malloc(strlen(line) +1 );
        strcpy(data->arr[index], line);

        if ((index+1) % data->N == data->consumer_index) {
            data->full = 1;
        }

        data->count++;

        data->producer_index = (index + 1) % data->N;

        /*release mutex for element */
        if(sem_post(&(data->mutexes[index]))!=0){
            perror("Error while releasing sem in array");
            exit(EXIT_FAILURE);
        }

        /*release global mutex for file */
        if(sem_post(&(mutex))!=0){
            perror("Error while releasing global sem");
            exit(EXIT_FAILURE);
        }

        if(data->is_extended) {
            printf("P: %ld released a mutex\n",pthread_self());
        }
    }

    return 0;
}

void *consumer(void *args) {
    Data *data = (Data*)(args);
    while(1){
        if(data->is_extended) {
            printf("C: %ld Start \n",pthread_self());
        }

        /*get a mutex for whole structure to get index safely */
        if(sem_wait(&mutex)!=0){
            perror("error while waiting for sem mutex");
            exit(EXIT_FAILURE);
        }

        while(data->consumer_index == data->producer_index && data->full == 0){
            sem_post(&mutex);
            sem_wait(&mutex);
        }

        /*lock element for yourself */
        if(sem_wait(&(data->mutexes[data->consumer_index]))!=0){
            perror("Error while waiting for array sem");
            exit(EXIT_FAILURE);
        }

        if(data->is_extended) {
            printf("C: %ld got a mutex\n",pthread_self());
        }

        /* process element */
        if(data->cmp((int) strlen(data->arr[data->consumer_index]), data->length) == 1){
            printf("C: %ld index: %d value: %s",pthread_self(),data->consumer_index, data->arr[data->consumer_index]);
            fflush(stdout);
        }

        free(data->arr[data->consumer_index]);
        data->arr[data->consumer_index] = NULL;
        int ind = data->consumer_index;
        data->consumer_index = (data->consumer_index + 1) % data->N;

        if(data->full == 1){
            data->full = 0;
        }

        /*unlock element */
        if(sem_post(&(data->mutexes[ind]))!=0){
            perror("Error while releasing sem in array");
            exit(EXIT_FAILURE);
        }

        if(sem_post(&(mutex))!=0){
            perror("Error while releasing global sem");
            exit(EXIT_FAILURE);
        }

        if(data->is_extended){
            printf("C: %ld released a mutex\n",pthread_self());
            fflush(stdout);
        }
    }
}

void parse_file(Data *var,char* file_name){

    FILE* file = fopen(file_name,"r");

    if(file==NULL) {
        perror("Error while opening conf file\n");
        exit(-1);
    }

    fscanf(file, "PROD = %d\n", &var->producers);
    fscanf(file, "CONS = %d\n", &var->consumers);
    fscanf(file, "ARR_S = %d\n", &var->N);

    char input_file[50];

    fscanf(file, "FILE = %s\n", input_file);

    FILE* input = fopen(input_file,"r");

    if(input==NULL) {
        perror("Error while opening input file\n");
        exit(-1);
    }

    var->file = input;

    fscanf(file, "LEN = %d\n", &var->length);

    char search_mode;
    fscanf(file, "SEARCH_MODE = %c\n", &search_mode);

    switch(search_mode){
        case '>':
            var->cmp = &greater;
            break;
        case '=':
            var->cmp = &equal;
            break;
        case '<':
            var->cmp = &lower;
            break;
        default:break;
    }

    char disp;
    fscanf(file, "PRINT_MODE = %c\n", &disp);
    if(disp == 'S'){
        var->is_extended = 0;
    }
    else{
        var->is_extended = 1;
    }

    fscanf(file, "THREAD_LIFE = %d\n", &var->nk);

    var->producer_index = 0;
    var->consumer_index = 0;
    var->arr = malloc(sizeof(char*) * var->N);
    if(var->arr == NULL){
        perror("Error at mem allocation for arr");
    }
    int i;
    for(i=0;i<var->N;i++) var->arr[i] = NULL;

    var->full = 0;
    var->count = 0;

    var->mutexes = malloc(sizeof(sem_t)*var->N);

    for(i=0;i<var->N;i++){
        if(sem_init(&var->mutexes[i],0,1)!=0){
            perror("Error at creating sem buffers");
            exit(EXIT_FAILURE);
        }
    }
}


int main(int argc, char** argv) {
//    if (argc != 2) {
//        printf("Not enough arguments\n"); exit(EXIT_FAILURE);
//    }


    // Initialize data

    parse_file(&global_data,"conf.txt");

    if(sem_init(&mutex,0,1)!=0){
        perror("Error at init of a global sem");
        exit(EXIT_FAILURE);
    }

    // Start threads
    global_data.pthread = malloc(sizeof(pthread_t) * (global_data.producers + global_data.consumers));

    int i;
    for (i = 0; i < global_data.producers; ++i)
        pthread_create(&(global_data.pthread[i]), NULL, producer, &global_data);

    for (i = 0; i < global_data.consumers; ++i)
        pthread_create(&(global_data.pthread[global_data.producers + i]), NULL, consumer, &global_data);

    struct sigaction sigact;
    sigemptyset(&sigact.sa_mask);
    sigact.sa_handler = handleSignal;
    sigaction(SIGINT, &sigact, NULL);

    sleep((unsigned int) global_data.nk);

    for (i = 0; i < (global_data.producers + global_data.consumers); ++i) {
        pthread_cancel(global_data.pthread[i]);
    }

    free(global_data.pthread);
    cleanup_data(&global_data);

    return 0;
}

#pragma clang diagnostic pop