#include <stdio.h>
#include <sys/resource.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include <sys/time.h>
#include "DynamicPointerArray.h"
#include "StaticArray.h"

struct time_pair{
    struct timeval start;
    struct timeval end;
};

struct time_collection{
    struct time_pair user;
    struct time_pair sys;
    struct time_pair real;
    struct rusage usage;
};

struct time_collection time_measure;


void start_time(){
    getrusage(RUSAGE_SELF,&time_measure.usage);
    time_measure.real.start = time_measure.usage.ru_stime;
    time_measure.user.start = time_measure.usage.ru_utime;
    gettimeofday(&time_measure.real.start,NULL);
}

void end_time(){
    getrusage(RUSAGE_SELF,&time_measure.usage);
    time_measure.real.end = time_measure.usage.ru_stime;
    time_measure.user.end = time_measure.usage.ru_utime;
    gettimeofday(&time_measure.real.end,NULL);
}

void print_time(){
    printf("System: %lf s\n"
                   "User: %lf s\n"
                   "Real: %lf s\n",(double)(time_measure.sys.end.tv_sec-time_measure.sys.start.tv_sec)+(double)((time_measure.sys.end.tv_usec)-time_measure.sys.start.tv_usec)*pow(10,-6)
            ,(double)(time_measure.user.end.tv_sec-time_measure.user.start.tv_sec)+(double)((time_measure.user.end.tv_usec)-time_measure.user.start.tv_usec)*pow(10,-6)
            ,(double)(time_measure.real.end.tv_sec-time_measure.real.start.tv_sec)+(double)((time_measure.real.end.tv_usec)-time_measure.real.start.tv_usec)*pow(10,-6));
}


void init_test_stat(int size, size_t block_size){
    printf("\nStatic allocation of array with size= %d and block_size=%d\n",size,(int)block_size);

    start_time();

    fill_array(size,block_size);

    end_time();
    print_time();
}

void search_test_stat(int amount,int size,size_t block_size){
    printf("\nStatic search with size= %d, block_size=%d items = %d\n",size,(int)block_size,amount);
    fill_array(size,block_size);

    start_time();

    for(int i=0;i<amount;i++){
        int x = get_closest_element(rand()%size);
    }

    end_time();
    print_time();
}

void alocate_groups_of_blocks_stat(int amount,int size,size_t block_size){
    printf("\nStatic allocation and rm of block with size= %d, block_size=%d items = %d\n",size,(int)block_size,amount);
    fill_array(size,block_size);

    start_time();

    for(int i=0;i<amount;i++){
        remove_block(i);
    }

    //print_static_array();

    for(int i=0;i<amount;i++){
        insert_memory_block(random_string_generator(block_size));
    }

    //print_static_array();

    end_time();
    print_time();
}

void allocate_random_blocks_dyn(int amount,int size,size_t block_size){
    printf("\nStatic allocation and rm element with size= %d, block_size=%d items = %d\n",size,(int)block_size,amount);
    fill_array(size,block_size);

    start_time();

    for(int i=0;i<amount;i++){
        int random = rand()%size;
        while(is_taken(random)!=1){
            random = rand()%size;
        }
        remove_block(random);
        insert_memory_block(random_string_generator(block_size));
    }

    end_time();
    print_time();
}

void init_test(int size, size_t block_size){
    printf("\nDynamic allocation of array with size= %d and block_size=%d\n",size,(int)block_size);
    start_time();

    array_structure* new_array = create_array(size, block_size);

    end_time();
    print_time();

    remove_array(new_array);
}

void search_test_dyn(int amount, int size, size_t base_size){
    printf("\nDynamic search with size= %d, block_size=%d items = %d\n",size,(int)base_size,amount);
    array_structure* new_array = create_array(size, base_size);

    start_time();

    for(int i=0;i<amount;i++){
        char* x = search_for_closest_ascii_sum(new_array,rand()%size);
    }

    end_time();
    print_time();

    remove_array(new_array);
}

void alocate_groups_of_blocks(int amount,int size,size_t base_size){
    printf("\nDynamic allocation and rm element with size= %d, block_size=%d items = %d\n",size,(int)base_size,amount);
    array_structure* new_array = create_array(size, base_size);

    start_time();

    for(int i=0;i<amount;i++){
        new_array = remove_array_element(new_array,i);
    }

    for(int i=0;i<amount;i++){
        new_array = add_to_array(new_array,dynamic_random_string_generator(base_size));
    }

    end_time();
    print_time();

    remove_array(new_array);

}

void allocate_random_blocks(int amount,int size,size_t block_size){
    printf("\nDynamic allocation and rm element with size= %d, block_size=%d items = %d\n",size,(int)block_size,amount);
    array_structure* new_object = create_array(size,block_size);

    start_time();

    for(int i=0;i<amount;i++){
        int random = rand()%size;
        while(new_object->array[random]==NULL){
            random = rand()%size;
        }
        new_object = remove_array_element(new_object,random);
        new_object = add_to_array(new_object,dynamic_random_string_generator(block_size));
    }

    end_time();
    print_time();

    remove_array(new_object);
}


int main(int argc, char* argv[]) {
    if(argc==1)
        printf("Command line arguments:\n"
                       "[create_table_s] [int elements] [size_t element_size]\n"
                       "[create_table_d] [int_elements] [size_t element_size]\n"
                       "[search_element_s_test] [int to_search] [int size] [size_t base_size]\n"
                       "[search_element_d_test] [int to_search] [int size] [size_t base_size]\n"
                       "[rm_add_block_s_test] [int to_remove] [int size] [size_t base_size] \n"
                       "[rm_add_block_d_test] [int to_remove] [int size] [size_t base_size] \n"
                       "[rm_add_number_s_test] [int to_remove] [int size] [size_t base_size] \n"
                       "[rm_add_number_d_test] [int to_remove] [int size] [size_t base_size] \n"
        );
    if(argc>=2)
    {
        if(strcmp(argv[1],"create_table_d")==0) init_test(atoi(argv[2]),atoi(argv[3]));
        else if(strcmp(argv[1],"create_table_s")==0) init_test_stat(atoi(argv[2]),atoi(argv[3]));
        else if(strcmp(argv[1],"search_element_s_test")==0) search_test_stat(atoi(argv[2]),atoi(argv[3]), atoi(argv[4]));
        else if(strcmp(argv[1],"search_element_d_test")==0) search_test_dyn(atoi(argv[2]),atoi(argv[3]), atoi(argv[4]));
        else if(strcmp(argv[1],"rm_add_block_s_test")==0) alocate_groups_of_blocks_stat(atoi(argv[2]),atoi(argv[3]), atoi(argv[4]));
        else if(strcmp(argv[1],"rm_add_block_d_test")==0) alocate_groups_of_blocks(atoi(argv[2]),atoi(argv[3]), atoi(argv[4]));
        else if(strcmp(argv[1],"rm_add_number_s_test")==0) allocate_random_blocks(atoi(argv[2]),atoi(argv[3]), atoi(argv[4]));
        else if(strcmp(argv[1],"rm_add_number_d_test")==0) allocate_random_blocks_dyn(atoi(argv[2]),atoi(argv[3]), atoi(argv[4]));
        else{
            printf("Command not found \n");
            for(int i=1;i<argc;i++)
                printf("\nargv[%d]: %s",i,argv[i]);
        }
    }
    return 0;
}