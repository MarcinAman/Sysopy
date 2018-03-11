#include <stdio.h>
#include <time.h>
#include <sys/resource.h>
#include <stdlib.h>
#include "StaticArray.h"

void print_time(struct timeval start_sys,struct timeval end_sys, struct timeval start_u,
                struct timeval end_u,clock_t start_real,clock_t end_real){
    printf("System started at: %ld.%lds\n", start_sys.tv_sec, start_sys.tv_usec);
    printf("System Ended at: %ld.%lds\n", end_sys.tv_sec, end_sys.tv_usec);
    printf("System delta: %ld.%ld \n",(end_sys.tv_sec-start_sys.tv_sec),(end_sys.tv_usec-start_sys.tv_usec)); // that suxs
    printf("User started at: %ld.%lds\n", start_u.tv_sec, start_u.tv_usec);
    printf("User ended at: %ld.%lds\n", end_u.tv_sec, end_u.tv_usec);
    printf("User delta: %ld.%lds\n",(end_u.tv_sec-start_u.tv_sec),(end_u.tv_usec-start_u.tv_usec));
    printf("Real execution time: %f s\n",((double)(end_real-start_real))/CLOCKS_PER_SEC);
}

void init_test(int size, size_t block_size){
    struct rusage usage;
    struct timeval start_sys, end_sys,start_u,end_u;
    clock_t start_real = clock();

    getrusage(RUSAGE_SELF, &usage);
    start_sys = usage.ru_stime;
    start_u = usage.ru_utime;

    fill_array(size,block_size);

    getrusage(RUSAGE_SELF, &usage);
    end_sys = usage.ru_stime;
    end_u = usage.ru_utime;
    clock_t end_real = clock();
    print_time(start_sys,end_sys,start_u,end_u,start_real,end_real);
}

void search_test(int amount,int size,size_t block_size){
    fill_array(size,block_size);

    struct rusage usage;
    struct timeval start_sys, end_sys,start_u,end_u;
    clock_t start_real = clock();

    getrusage(RUSAGE_SELF, &usage);
    start_sys = usage.ru_stime;
    start_u = usage.ru_utime;

    for(int i=0;i<amount;i++){
        int x = get_closest_element(rand()%size);
    }

    getrusage(RUSAGE_SELF, &usage);
    end_sys = usage.ru_stime;
    end_u = usage.ru_utime;
    clock_t end_real = clock();
    print_time(start_sys,end_sys,start_u,end_u,start_real,end_real);
}

void alocate_groups_of_blocks(int amount,int size,size_t block_size){
    fill_array(size,block_size);

    struct rusage usage;
    struct timeval start_sys, end_sys,start_u,end_u;
    clock_t start_real = clock();

    getrusage(RUSAGE_SELF, &usage);
    start_sys = usage.ru_stime;
    start_u = usage.ru_utime;

    for(int i=0;i<amount;i++){
        remove_block(i);
    }

    for(int i=0;i<amount;i++){
        insert_memory_block(random_string_generator(block_size));
    }

    getrusage(RUSAGE_SELF, &usage);
    end_sys = usage.ru_stime;
    end_u = usage.ru_utime;
    clock_t end_real = clock();
    print_time(start_sys,end_sys,start_u,end_u,start_real,end_real);
}

void allocate_random_blocks(int amount,int size,size_t block_size){
    fill_array(size,block_size);

    struct rusage usage;
    struct timeval start_sys, end_sys,start_u,end_u;
    clock_t start_real = clock();

    getrusage(RUSAGE_SELF, &usage);
    start_sys = usage.ru_stime;
    start_u = usage.ru_utime;

    for(int i=0;i<amount;i++){
        int random = rand()%size;
        while(is_taken(random)!=1){
            random = rand()%size;
        }
        remove_block(random);
    }

    for(int i=0;i<amount;i++){
        insert_memory_block(random_string_generator(block_size));
    }

    getrusage(RUSAGE_SELF, &usage);
    end_sys = usage.ru_stime;
    end_u = usage.ru_utime;
    clock_t end_real = clock();
    print_time(start_sys,end_sys,start_u,end_u,start_real,end_real);
}

int main() {
    //init_test(100000, sizeof(char)*50);
    //search_test(1000,100000, sizeof(char)*50);
    //alocate_groups_of_blocks(1000,100000, sizeof(char)*50);
    allocate_random_blocks(1000,100000, sizeof(char)*50);
    return 0;
}