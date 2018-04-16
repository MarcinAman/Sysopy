#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <signal.h>

#define send_signal(pid,sig_no){ \
    if(kill(pid,sig_no)==-1) printf("Error while sending signal %d to %d",sig_no,pid);\
}

struct signals_pair{
  int sig1;
  int sig2;
};

pid_t childs_pid;
int signals_received = 0;
int signals_send = 0;
int signals_total;
int mode;
struct signals_pair signals_to_send;

void leave_no_man_behind(int sig_no){
  if(childs_pid){
    send_signal(childs_pid,SIGUSR2);
    printf("Finishing parent after sending %d signals and receiving %d ...\n",signals_send,signals_received);
    fflush(stdout);
  }
  exit(0);
}

void au(int sig_no,siginfo_t *sig_info,void *ucontext){
  if(childs_pid==0){
    /* child */
    signals_received++;
    printf("Child received a signal %d\n",sig_no);
    fflush(stdout);

    if(sig_no==signals_to_send.sig1 || sig_no == signals_to_send.sig2){
      send_signal(getppid(),sig_no);
      printf("Child send a signal %d\n",sig_no);
      fflush(stdout);

      signals_send++;
      if(signals_send==signals_total){
        printf("Child is finishing work with %d signals\n",signals_send);
        fflush(stdout);
      }
      return;
    }

    if(signals_received == signals_total){
      send_signal(getppid(),SIGUSR2);
      printf("Child send a signal %d\n",SIGUSR2);
      printf("Child is terminating with %d signals send and %d signals received\n",signals_send+1,signals_received);
      exit(0);
    }

    send_signal(getppid(),SIGUSR1);
    signals_send++;
    printf("Child send a signal %d\n",SIGUSR1);
    fflush(stdout);
  }
  else{
    if(signals_received==signals_total){
      leave_no_man_behind(0);
    }
    /* parent */
    signals_received++;
    printf("Parent received a signal %d\n",sig_no);
    fflush(stdout);

    if(mode==3 && signals_send<signals_total){

      send_signal(childs_pid,sig_no);

      printf("Parent send a signal %d\n",sig_no);
      fflush(stdout);

      signals_send++;
    }
    else if(mode==2 && signals_send < signals_total){
      send_signal(childs_pid,SIGUSR1);

      printf("Parent send a signal %d\n",SIGUSR1);
      fflush(stdout);

      signals_send++;
    }
  }
}

void au_usr2(int sig_no,siginfo_t *sig_info,void *ucontext){
  if(childs_pid==0){
    /*child*/
    exit(0);
    printf("Child is terminating with %d signals send and %d signals received\n",signals_send,signals_received);
    fflush(stdout);
  }
  else{
    printf("Parent is terminating with %d signals send and %d signals received\n",signals_send,signals_received);
    fflush(stdout);
    exit(0); /* ??? */
  }
}

void multiprocess_communication(int kill_mode, int signals){
  printf("Signals total: %d MODE: %d\n",signals,kill_mode);
  fflush(stdout);
  signals_total = signals;
  mode = kill_mode;
  childs_pid = fork();

  if(childs_pid==-1){
    printf("%s\n","Error while creating new child, terminating...");
    fflush(stdout);
    exit(1);
  }

  struct sigaction act;
  act.sa_sigaction = au;
  act.sa_flags = SA_SIGINFO;
  sigemptyset(&act.sa_mask);
  sigaction(SIGUSR1,&act,NULL);

  struct sigaction act_usr2;
  act_usr2.sa_sigaction = au_usr2;
  act_usr2.sa_flags = SA_SIGINFO;
  sigemptyset(&act_usr2.sa_mask);
  sigaction(SIGUSR2,&act_usr2,NULL);

  signal(SIGINT,leave_no_man_behind);

  if(childs_pid==0){

    sigaction(signals_to_send.sig1,&act,NULL);
    sigaction(signals_to_send.sig2,&act,NULL);
    /* keep the child alive */
    while(1){
      sleep(1);
    }
  }
  else{
    /* parent */
    sleep(1);
    if(mode==1){
      for(int i=0;i<signals_total;i++){
          send_signal(childs_pid,SIGUSR1);
          sleep(1); /* if we dont do it majority of signals will be lost */
          printf("Parent send signal to %d\n",childs_pid);
          fflush(stdout);
          signals_send++;
      }
      printf("Parent ended sending package of signals %d to %d\n",SIGUSR1,childs_pid);
      fflush(stdout);
    }
    else if(mode==2){
      send_signal(childs_pid,SIGUSR1);
      printf("Parent send first signal %d to %d\n",SIGUSR1,childs_pid);
      fflush(stdout);
      signals_send++;
    }
    else{
      sigaction(signals_to_send.sig1,&act,NULL);
      sigaction(signals_to_send.sig2,&act,NULL);

      send_signal(childs_pid,signals_to_send.sig1);
      sleep(1);
      send_signal(childs_pid,signals_to_send.sig2);
      signals_send+=2;
      printf("Parent send signal %d and %d to %d\n",signals_to_send.sig1,signals_to_send.sig2,childs_pid);
      fflush(stdout);
    }
  }

  while(signals_received<signals_total){
    sleep(1);
  }

  leave_no_man_behind(0);
}

int main(int argc, char** argv){
  signals_to_send.sig1 = SIGRTMAX-6;
  signals_to_send.sig2 = SIGRTMAX-2;

  if(argc==1){
    printf("./main [mode] [signals]\n");
    return 0;
  }

  multiprocess_communication(atoi(argv[1]),atoi(argv[2]));
  return 0;
}
