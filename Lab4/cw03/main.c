#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>

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

void au(int sig_no,siginfo_t *sig_info,void *ucontext){
  if(childs_pid==0){
    /* child */
    signals_received++;
    printf("Child received a signal %d\n",sig_no);
    fflush(stdout);

    if(signals_received == signals_total){
      send_signal(getppid(),SIGUSR2);
      printf("Child send a signal %d\n",SIGUSR2);
      printf("Child is terminating with %d signals send and %d signals received\n",signals_send,signals_received);
      exit(0);
    }

    send_signal(getppid(),SIGUSR1);
    printf("Child send a signal %d\n",SIGUSR1);
    fflush(stdout);
  }
  else{
    /* parent */
    if(mode==2 && signals_send < signals_total){
      send_signal(childs_pid,SIGUSR1);

      printf("Parent send a signal %d\n",SIGUSR1);
      fflush(stdout);

      signals_send++;
    }
    else if(mode==3 && signals_send<signals_total){
      send_signal(childs_pid,signals_to_send.sig1);
      send_signal(childs_pid,signals_to_send.sig2);

      printf("Parent send a signal %d + %d\n",signals_to_send.sig1,signals_to_send.sig2);
      fflush(stdout);

      signals_send++;
    }
  }
}

void leave_no_man_behind(int sig_no){
  if(childs_pid){
    send_signal(childs_pid,SIGUSR2);
    printf("Killing child after sending %d signals and receiving %d ...\n",signals_send,signals_received);
    fflush(stdout);
  }
  exit(0);
}

void au_usr2(int sig_no,siginfo_t *sig_info,void *ucontext){
  if(childs_pid==0){
    /*child*/
    exit(0);
    printf("Child is terminating with %d signals send and %d signals received\n",signals_send,signals_received);
    fflush(stdout);
  }
  else{
    exit(0); /* ??? */
    printf("Parent is terminating with %d signals send and %d signals received\n",signals_send,signals_received);
    fflush(stdout);
  }
}

void multiprocess_communication(int kill_mode, int signals){
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
  act.sa_sigaction = au_usr2;
  act.sa_flags = SA_SIGINFO;
  sigemptyset(&act_usr2.sa_mask);
  sigaction(SIGUSR2,&act_usr2,NULL);

  signal(SIGINT,leave_no_man_behind);

  if(childs_pid!=0){
    /* parent */
    if(mode==1){
      for(int i=0;i<signals;i++){
          send_signal(childs_pid,SIGUSR1);
          signals_send++;
      }
      printf("Parent send package of signals %d to %d\n",SIGUSR1,childs_pid);
      fflush(stdout);
    }
    else if(mode==2){
      send_signal(childs_pid,SIGUSR1);
      printf("Parent send first signal %d to %d\n",SIGUSR1,childs_pid);
      fflush(stdout);
      signals_send++;
    }
    else{
      send_signal(childs_pid,signals_to_send.sig1);
      send_signal(childs_pid,signals_to_send.sig1);
      signals_send++;
      printf("Parent send signal %d and %d to %d\n",SIGRTMIN,SIGRTMAX,childs_pid);
      fflush(stdout);
    }
  }

  while(1){
    sleep(1);
  }
}

int main(int argc, char** argv){
  signals_to_send.sig1 = SIGRTMIN;
  signals_to_send.sig2 = SIGRTMAX;

  if(argc==1){
    printf("./main [mode] [signals]\n");
    return 0;
  }

  multiprocess_communication(atoi(argv[1]),atoi(argv[2]));
  return 0;
}
