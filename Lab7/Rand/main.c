//
// Created by marcinaman on 4/27/18.
//
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>

pid_t pid;

void handle(int a){
    kill(pid,SIGINT);
    exit(0);
}

#define print_array(){ int i; for(i=0;i<10;i++){printf("%d \t",shem[i]);}printf("\n");}

int main(int argc, char** argv){

    int *shem = mmap(NULL, sizeof(int) * 10, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0);

    pid = fork();
    if(pid==0){
        sleep(3);
        int pid = fork();

        if(pid==0){
            signal(SIGINT,handle);
            sleep(3);
            int i;
            for(i=0;i<10;i++){
                shem[i]=getpid();
                printf("Getpid:%d\n",getpid());
                print_array();
                fflush(stdout);
                sleep(1);
            }
        }
        else{
            signal(SIGINT,handle);

            int i;
            for(i=0;i<10;i++){
                shem[i] = getpid();
                printf("Getpid:%d\n",getpid());
                print_array();
                fflush(stdout);
                sleep(1);
            }
            kill(pid,SIGINT);
        }
    }
    else{
        signal(SIGINT,handle);

        int i = 0;
        for(i;i<10;i++){
            shem[i] = (int)getpid();
            printf("Getpid:%d\n",getpid());
            print_array();
            fflush(stdout);
            sleep(1);
        }
    }
    kill(pid,SIGINT);

    int i;
    for(i=0;i<10;i++){
        printf("%d.%d\n",i,shem[i]);
        fflush(stdout);
    }
    return 0;
}