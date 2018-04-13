/* i cant get exit codes properly */

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>

struct program_info{
  pid_t childs_pid;
  int signal_send;
  int signal_received;
  int received_signal_number;
  int status;
};

struct program_info* children;
int currently_received_signals;
int M,N;
int running_amount;

/*M is required amount to run */
/* N is a total amount of programs, */

#define controled_kill(pid,signal_value){\
  if(getpgid(pid)>=0 && kill(pid,signal_value)==-1){ \
    printf("Error while sending signal %d to %d\n",signal_value,pid); }}

/* to check if a process is running we can either kill(pid,0) since it doesnt do anything or (more reliable method) get his group id */

#define create_sigaction(){\
  struct sigaction act;\
  act.sa_sigaction = au;\
  act.sa_flags = SA_SIGINFO;\
  sigemptyset(&act.sa_mask);\
  sigaction(SIGUSR1,&act,NULL);\
  struct sigaction act_real;\
  act_real.sa_sigaction = print_data;\
  act_real.sa_flags = SA_SIGINFO;\
  sigemptyset(&act_real.sa_mask);\
  for(int i=SIGRTMIN;i<SIGRTMAX;i++){\
    sigaction(i,&act_real,NULL);\
  }\
}

/*for children*/

int is_signal_received = 0;

void signal_initialized(int sig_no){
  is_signal_received = 1;
}

void leave_no_man_behind(int sig_no){
  printf("%s\n","Clearing...");
  for(int i=0;i<N;i++){
    kill(children[i].childs_pid,SIGTERM);
  }
}

void au(int sig_no,siginfo_t *sig_info,void *ucontext){
    if(currently_received_signals<M){
      for(int i=0;i<N;i++){
        if(children[i].childs_pid == sig_info->si_pid && !children[i].signal_received){
          children[i].signal_received = 1;
          currently_received_signals++;
          printf("PARENT CONT:%s %d %s %d\n", "Signal: ",sig_no," received from: ",sig_info->si_pid);
          fflush(stdout);
        }
      }
    }
    else if(currently_received_signals==M){
      for(int i=0;i<N;i++){
        if(children[i].signal_received){
          controled_kill(children[i].childs_pid,SIGCONT);
          children[i].signal_send = 1;
        }
      }

      printf("Parent sent signals to all childs that reported till now\n");
      currently_received_signals++;
      fflush(stdout);
    }
    else{
      printf("PARENT CONT ADDITIONAL:%s %d %s %d\n", "Signal: ",sig_no," received from: ",sig_info->si_pid);
      fflush(stdout);
      for(int i=0;i<N;i++){
        if(children[i].childs_pid == sig_info->si_pid){
          children[i].signal_received = 1;
          currently_received_signals++;

          controled_kill(children[i].childs_pid,SIGCONT);
          children[i].signal_send=1;
        }
      }
    }
}

void print_data(int sig_no,siginfo_t *sig_info,void *ucontext){
  printf("PARENT RAND:%s %d from: %d\n","Received signal: ",sig_no,sig_info->si_pid);
  fflush(stdout);

  for(int i=0;i<N;i++){

    if(children[i].childs_pid==sig_info->si_pid){
      children[i].received_signal_number = sig_no;
      children[i].signal_received = 1;
    }

  }
}

int create_and_accept_childs(){

   children = malloc(sizeof(struct program_info)*N);

   for(int i=0;i<N;i++){
     children[i].childs_pid = 0;
     children[i].signal_send = 0;
     children[i].signal_send = 0;
     children[i].received_signal_number = -1;
     children[i].status = -1;
   }

   currently_received_signals = 0;

   for(int i=0;i<N;i++){
      children[i].childs_pid = fork();

      if(children[i].childs_pid==-1){
        printf("%s\n","Error while forking" );
        leave_no_man_behind(children[i].childs_pid);
        exit(EXIT_FAILURE);
      }
      else if(children[i].childs_pid){

        create_sigaction();

        wait(&children[i].status);

        printf("Child: %d ended with exit code: %d\n",children[i].childs_pid,WEXITSTATUS(children[i].status));
        fflush(stdout);
        running_amount--;
      }
      if(children[i].childs_pid==0){
        /*for child's process*/
        srand(time(NULL));

        int seconds = 2+rand()%8;

        printf("Sleeping for %d seconds\n",seconds);
        fflush(stdout);

        sleep(seconds);

        if(signal(SIGCONT,signal_initialized)==SIG_ERR) printf("Error while creating signal\n");
        int kill_value = kill(getppid(),SIGUSR1);

        printf("CHILD %d :Signal %d was sent to %d (parent) with result %d\n",getpid(),SIGUSR1,getppid(),kill_value);
        fflush(stdout);

        if(kill_value == -1){
          printf("%s\n","Error while sending SIGUSR1" );
        }

        while(!is_signal_received){
          sleep(1);
        }

        int rand_signal = rand()%32;
        if(kill(getppid(),SIGRTMIN+rand_signal)==-1){
          printf("%s\n","Error while sending random signal to parent");
        }

        printf("CHILD RAND: Signal %d was sent to %d\n",rand_signal+SIGRTMIN,getppid());
        fflush(stdout);

        return seconds;
      }
   }
   while (running_amount) {
      sleep(3);
   }
   leave_no_man_behind(0);

   return 0;
}


int main(int argc, char** argv){
  M = atoi(argv[1]);
  N = atoi(argv[2]);
  running_amount = N;

  if(signal(SIGINT,leave_no_man_behind)==SIG_ERR) printf("Error while creating signal\n"); //error handling -> SIG_ERR
  return create_and_accept_childs();
}
