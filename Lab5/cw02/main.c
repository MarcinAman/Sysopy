#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <memory.h>
#include <time.h>
#include <sys/wait.h>

#define SET_SIGACTION struct sigaction act; act.sa_flags = SA_SIGINFO; act.sa_sigaction = &au; sigfillset(&act.sa_mask); sigaction(SIGINT, &act, NULL);
#define CHILDREN_MAX 4096

static void au(int sigNum, siginfo_t* info, void* vp){
    printf("Termination after getting SIGINT\n");
    killpg(0, SIGINT);
    exit(EXIT_SUCCESS);
}

int childPid;
int masterPid;
int childPids[CHILDREN_MAX];


int main(int argc, char *argv[]) {

    char buffer[512];
    int slaveNumber;
    int N;

    if (argc != 4){
        printf("Wrong main arguments! We need [fifo_name] [slaves_number] [N]\n");
        exit(1);
    }

    SET_SIGACTION

    slaveNumber = atoi(argv[2]);
    N = atoi(argv[3]);

    printf("Program started with parameters slaves = %d, N = %d\n",slaveNumber,N);
    fflush(stdout);

    for (int i = 0; i < slaveNumber; ++i) {
        childPids[i] = fork();
        if (childPids[i] == 0){
            /* child */
            execlp("./slave","slave",argv[1], argv[3], 0);
        }
    }

    masterPid = fork();
    if (masterPid == 0){
        execlp("./master", "master", argv[1], (char*)NULL);
    }
    else{
      for (int i = 0; i < slaveNumber; ++i) {
        waitpid(childPids[i], NULL, WUNTRACED);
      }
      printf("All children slaves died...\n");
    }

    kill(masterPid, SIGINT);
    printf("Master died\n");
    killpg(0, SIGINT);

    return 0;
}
