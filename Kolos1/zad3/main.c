#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>


int main ( int argc, char *argv[] )
{
    int i;

    printf("Pid glowego programu %d\n",getpid());

    if(argc != 2){
        printf("Not a suitable number of program arguments");
        exit(2);
    }else {
       for (i = 0; i < atoi(argv[1]); i++) {
           if(fork()==0){
               printf("%d. PID: %d,rodzic ma pid: %d\n",i+1,getpid(),getppid());
               fflush(stdout);
               exit(0);
           }
        //*********************************************************
        //Uzupelnij petle w taki sposob aby stworzyc dokladnie argv[1] procesow potomnych, bedacych dziecmi
        //   tego samego procesu macierzystego.
           // Kazdy proces potomny powinien:
               // - "powiedziec ktorym jest dzieckiem",
                //-  jaki ma pid,
                //- kto jest jego rodzicem
           //******************************************************
        }
        sleep(1);
    }
    return 0;
}
