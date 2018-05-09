#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void sighandler(int signo, siginfo_t *siginfo, void *cont){
    printf("Child: %d\n", siginfo->si_value.sival_int);
}


int main(int argc, char* argv[]) {

    if(argc != 3){
        printf("Not a suitable number of program parameters\n");
        return 1;
    }

    struct sigaction action;
    action.sa_sigaction = &sighandler;
    sigemptyset(&action.sa_mask);
    sigaction(SIGUSR1,&action,NULL);

    union sigval sv;
    sv.sival_int = atoi(argv[1]);
    sv.sival_ptr = NULL;

    int child = fork();
    if(child == 0) {
        sigset_t some;
        sigfillset(&some);
        sigdelset(&some, SIGUSR1);
        if(sigprocmask(SIG_BLOCK,&some,NULL)<0){
            printf("error while addig lsmask\n");
            fflush(stdout);
        }

        sleep(3);
        //zablokuj wszystkie sygnaly za wyjatkiem SIGUSR1
        //zdefiniuj obsluge SIGUSR1 w taki sposob zeby proces potomny wydrukowal
        //na konsole przekazana przez rodzica wraz z sygnalem SIGUSR1 wartosc
    }
    else {
        sleep(1);
        sigqueue(child, atoi(argv[2]), sv);

        //wyslij do procesu potomnego sygnal przekazany jako argv[2]
        //wraz z wartoscia przekazana jako argv[1]
    }

    return 0;
}
