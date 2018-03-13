#include <stdio.h>
#include <sys/resource.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include "DynamicPointerArray.h"
#include "StaticArray.h"


void print_time(struct timeval start_sys,struct timeval end_sys, struct timeval start_u,
                struct timeval end_u,struct timeval start_real,struct timeval end_real){
    printf("System started at: %fs\n", (float)start_sys.tv_sec+(float)start_sys.tv_usec*pow(10,-6));
    printf("System Ended at: %fs\n", (float)end_sys.tv_sec+(float)end_sys.tv_usec*pow(10,-6));
    printf("System delta: %fs \n",(float)(end_sys.tv_sec-start_sys.tv_sec)+(float)(end_sys.tv_usec-start_sys.tv_usec)*pow(10,-6));
    printf("User started at: %fs\n", (float)start_u.tv_sec+(float)start_u.tv_usec*pow(10,-6));
    printf("User ended at: %fs\n", (float)end_u.tv_sec+(float)end_u.tv_usec*pow(10,-6));
    printf("User delta: %fs\n",(float)(end_u.tv_sec-start_u.tv_sec)+(float)(end_u.tv_usec-start_u.tv_usec)*pow(10,-6));
    printf("Real execution time: %f s\n",(float)(end_real.tv_sec-start_real.tv_sec)+(float)(end_real.tv_usec-start_real.tv_usec)*pow(10,-6));
}


void init_test_dyn(int size, size_t block_size){
    printf("\nDynamic allocation of array with size= %d and block_size=%d\n",size,(int)block_size);

    struct rusage usage;
    struct timeval start_sys, end_sys,start_u,end_u,start_real,end_real;
    getrusage(RUSAGE_SELF, &usage);
    start_sys = usage.ru_stime;
    start_u = usage.ru_utime;
    gettimeofday(&start_real,NULL);

    fill_array(size,block_size);

    getrusage(RUSAGE_SELF, &usage);
    end_sys = usage.ru_stime;
    end_u = usage.ru_utime;
    gettimeofday(&end_real,NULL);

    print_time(start_sys,end_sys,start_u,end_u,start_real,end_real);
}

void search_test_dyn(int amount,int size,size_t block_size){
    printf("\nDynamic search with size= %d, block_size=%d items = %d\n",size,(int)block_size,amount);
    fill_array(size,block_size);

    struct rusage usage;
    struct timeval start_sys, end_sys,start_u,end_u,start_real,end_real;

    getrusage(RUSAGE_SELF, &usage);
    start_sys = usage.ru_stime;
    start_u = usage.ru_utime;

    gettimeofday(&start_real,NULL);

    for(int i=0;i<amount;i++){
        int x = get_closest_element(rand()%size);
    }

    getrusage(RUSAGE_SELF, &usage);
    end_sys = usage.ru_stime;
    end_u = usage.ru_utime;
    gettimeofday(&end_real,NULL);

    print_time(start_sys,end_sys,start_u,end_u,start_real,end_real);
}

void alocate_groups_of_blocks_dyn(int amount,int size,size_t block_size){
    printf("\nDynamic allocation and rm of block with size= %d, block_size=%d items = %d\n",size,(int)block_size,amount);
    fill_array(size,block_size);

    struct rusage usage;
    struct timeval start_sys, end_sys,start_u,end_u,start_real,end_real;

    getrusage(RUSAGE_SELF, &usage);
    start_sys = usage.ru_stime;
    start_u = usage.ru_utime;

    gettimeofday(&start_real,NULL);

    for(int i=0;i<amount;i++){
        remove_block(i);
    }

    for(int i=0;i<amount;i++){
        insert_memory_block(random_string_generator(block_size));
    }

    getrusage(RUSAGE_SELF, &usage);
    end_sys = usage.ru_stime;
    end_u = usage.ru_utime;
    gettimeofday(&end_real,NULL);

    print_time(start_sys,end_sys,start_u,end_u,start_real,end_real);
}

void allocate_random_blocks_dyn(int amount,int size,size_t block_size){
    printf("\nDynamic allocation and rm element with size= %d, block_size=%d items = %d\n",size,(int)block_size,amount);
    fill_array(size,block_size);

    struct rusage usage;
    struct timeval start_sys, end_sys,start_u,end_u,start_real,end_real;

    getrusage(RUSAGE_SELF, &usage);
    start_sys = usage.ru_stime;
    start_u = usage.ru_utime;

    gettimeofday(&start_real,NULL);

    for(int i=0;i<amount;i++){
        int random = rand()%size;
        while(is_taken(random)!=1){
            random = rand()%size;
        }
        remove_block(random);
        insert_memory_block(random_string_generator(block_size));
    }

    getrusage(RUSAGE_SELF, &usage);
    end_sys = usage.ru_stime;
    end_u = usage.ru_utime;
    gettimeofday(&end_real,NULL);

    print_time(start_sys,end_sys,start_u,end_u,start_real,end_real);
}

void init_test(int size, size_t block_size){
    printf("\nStatic allocation of array with size= %d and block_size=%d\n",size,(int)block_size);
    struct rusage usage;
    struct timeval start_sys, end_sys,start_u,end_u,start_real,end_real;

    getrusage(RUSAGE_SELF, &usage);
    start_sys = usage.ru_stime;
    start_u = usage.ru_utime;

    gettimeofday(&start_real,NULL);

    array_structure* new_array = create_array(size, block_size);

    getrusage(RUSAGE_SELF, &usage);
    end_sys = usage.ru_stime;
    end_u = usage.ru_utime;
    gettimeofday(&end_real,NULL);

    print_time(start_sys,end_sys,start_u,end_u,start_real,end_real);

    remove_array(new_array);
}

void search_test(int amount, int size, size_t base_size){
    printf("\nDynamic search with size= %d, block_size=%d items = %d\n",size,(int)base_size,amount);
    array_structure* new_array = create_array(size, base_size);

    struct rusage usage;
    struct timeval start_sys, end_sys,start_u,end_u,start_real,end_real;

    getrusage(RUSAGE_SELF, &usage);
    start_sys = usage.ru_stime;
    start_u = usage.ru_utime;

    gettimeofday(&start_real,NULL);

    for(int i=0;i<amount;i++){
        int x = search_for_closest_ascii_sum(new_array,rand()%size);
    }

    getrusage(RUSAGE_SELF, &usage);
    end_sys = usage.ru_stime;
    end_u = usage.ru_utime;
    gettimeofday(&end_real,NULL);

    print_time(start_sys,end_sys,start_u,end_u,start_real,end_real);

    remove_array(new_array);
}

void alocate_groups_of_blocks(int amount,int size,size_t base_size){
    printf("\nDynamic allocation and rm element with size= %d, block_size=%d items = %d\n",size,(int)base_size,amount);
    array_structure* new_array = create_array(size, base_size);

    struct rusage usage;
    struct timeval start_sys, end_sys,start_u,end_u,start_real,end_real;

    getrusage(RUSAGE_SELF, &usage);
    start_sys = usage.ru_stime;
    start_u = usage.ru_utime;

    gettimeofday(&start_real,NULL);

    for(int i=0;i<amount;i++){
        new_array = remove_array_element(new_array,i);
    }

    for(int i=0;i<amount;i++){
        new_array = add_to_array(new_array,dynamic_random_string_generator(base_size));
    }

    getrusage(RUSAGE_SELF, &usage);
    end_sys = usage.ru_stime;
    end_u = usage.ru_utime;
    gettimeofday(&end_real,NULL);

    print_time(start_sys,end_sys,start_u,end_u,start_real,end_real);

    remove_array(new_array);

};

void allocate_random_blocks(int amount,int size,size_t block_size){
    printf("\nDynamic allocation and rm element with size= %d, block_size=%d items = %d\n",size,(int)block_size,amount);
    array_structure* new_object = create_array(size,block_size);

    struct rusage usage;
    struct timeval start_sys, end_sys,start_u,end_u,start_real,end_real;

    getrusage(RUSAGE_SELF, &usage);
    start_sys = usage.ru_stime;
    start_u = usage.ru_utime;

    gettimeofday(&start_real,NULL);

    for(int i=0;i<amount;i++){
        int random = rand()%size;
        while(new_object->array[random]==NULL){
            random = rand()%size;
        }
        new_object = remove_array_element(new_object,random);
        new_object = add_to_array(new_object,dynamic_random_string_generator(block_size));
    }

    getrusage(RUSAGE_SELF, &usage);
    end_sys = usage.ru_stime;
    end_u = usage.ru_utime;
    gettimeofday(&end_real,NULL);

    print_time(start_sys,end_sys,start_u,end_u,start_real,end_real);

    remove_array(new_object);
}


int main(int argc, char* argv[]) {
    if(argc==1)
        printf("Command line arguments:\n"
                       "[init_static] [int elements] [size_t element_size]\n"
                       "[init_dynamic] [int_elements] [size_t element_size]\n"
                       "[search_static] [int to_search] [int size] [size_t base_size]\n"
                       "[search_dynamic] [int to_search] [int size] [size_t base_size]\n"
                       "[rm_add_block_static] [int to_remove] [int size] [size_t base_size] \n"
                       "[rm_add_block_dynamic] [int to_remove] [int size] [size_t base_size] \n"
                       "[rm_add_rand_static] [int to_remove] [int size] [size_t base_size] \n"
                       "[rm_add_rand_dynamic] [int to_remove] [int size] [size_t base_size] \n");
    if(argc>=2)
    {
        if(strcmp(argv[1],"init_static")==0) init_test(atoi(argv[2]),atoi(argv[3]));
        else if(strcmp(argv[1],"init_dynamic")==0) init_test_dyn(atoi(argv[2]),atoi(argv[3]));
        else if(strcmp(argv[1],"search_static")==0) search_test(atoi(argv[2]),atoi(argv[3]), atoi(argv[4]));
        else if(strcmp(argv[1],"search_dynamic")==0) search_test_dyn(atoi(argv[2]),atoi(argv[3]), atoi(argv[4]));
        else if(strcmp(argv[1],"rm_add_block_static")==0) alocate_groups_of_blocks(atoi(argv[2]),atoi(argv[3]), atoi(argv[4]));
        else if(strcmp(argv[1],"rm_add_block_dynamic")==0) allocate_random_blocks_dyn(atoi(argv[2]),atoi(argv[3]), atoi(argv[4]));
        else if(strcmp(argv[1],"rm_add_rand_static")==0) allocate_random_blocks(atoi(argv[2]),atoi(argv[3]), atoi(argv[4]));
        else if(strcmp(argv[1],"rm_add_rand_dynamic")==0) allocate_random_blocks_dyn(atoi(argv[2]),atoi(argv[3]), atoi(argv[4]));
        else{
            printf("Command not found \n");
            for(int i=1;i<argc;i++)
                printf("\nargv[%d]: %s",i,argv[i]);
        }
    }
    return 0;
}