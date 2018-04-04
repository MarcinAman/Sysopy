//
// Created by Marcin Aman on 21.03.18.
//

#include <math.h>
#include <sys/time.h>
#include "CatalogDisplay.h"

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

void test_library_speed(char* path){
    start_time();

    ls_operation(path,0,1);

    end_time();
    printf("For ntfw it took: \n");
    print_time();

    start_time();

    ls_operation(path,1,1);

    end_time();
    printf("\nFor my ntfw it took:\n");
    print_time();
}

int main(int argc, char** argv){

    set_time_and_sign(argv[2][0],atoi(argv[3]),atoi(argv[4]),atoi(argv[5]));

    if(atoi(argv[6])==2){
        test_library_speed(argv[1]);
        return 0;
    }

    ls_operation(argv[1],atoi(argv[6]),0);

    return 0;
}
