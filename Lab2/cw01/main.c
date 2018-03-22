#include <stdio.h>
#include <stdlib.h>
#include "SysOperation.h"
#include "LibOperation.h"
#include <unistd.h>
#include <limits.h>
#include <math.h>
#include <sys/time.h>
#include <time.h>
#include <sys/resource.h>
#include <memory.h>

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

void generate_test(char* file_name,int records, ssize_t block,int is_sys){

    start_time();

    if(is_sys==1){
        sys_generate(file_name,records,block);
    }
    else{
        lib_generate(file_name,records,block);
    }

    end_time();
    print_time();
}

void copy_test(char* file1, char* file2,int buffer,int is_sys){
    start_time();

    if(is_sys==1){
        sys_copy(file1, file2,buffer);
    }
    else{
        lib_copy(file1, file2,buffer);
    }

    end_time();
    print_time();
}

void sort_test(char* file,int records,ssize_t block, int is_sys){
    special_generator(file,records,block);
    
    start_time();

    if(is_sys==1){
        sys_sort(file,records,block);
    }
    else{
        lib_sort(file,records,block);
    }

    end_time();
    print_time();
}

int main(int argc, char** argv) {

    if(strcmp(argv[1],"generate")==0){
        generate_test(argv[2],atoi(argv[3]),atoi(argv[4]),atoi(argv[5]));
    }
    else if(strcmp(argv[1],"sort")==0){
        sort_test(argv[2],atoi(argv[3]),atoi(argv[4]),atoi(argv[5]));
    }
    else if(strcmp(argv[1],"copy")==0){
        copy_test(argv[2],argv[3],atoi(argv[4]),atoi(argv[5]));
    }
    else{
        printf("Command not found\n");
    }

    return 0;
}