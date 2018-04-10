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

/*M is required amount to run */
/* N is a total amount of programs, */

#define controled_kill(pid,signal_value){if(kill(pid,signal_value)==-1){printf("Error while sending signal %d to %d\n",signal_value,pid);}}

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
    printf("PARENT CONT:%s %d %s %d\n", "Signal: ",sig_no," received from: ",sig_info->si_pid);
    fflush(stdout);

    if(currently_received_signals<M){

      currently_received_signals++;

      for(int i=0;i<N;i++){
        if(children[i].childs_pid == sig_info->si_pid){
          children[i].signal_received = 1;
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

    }
    else{
      for(int i=0;i<N;i++){

        if(children[i].childs_pid==sig_info->si_pid){

          children[i].signal_received = 1;

          if(!children[i].signal_send){
            controled_kill(sig_info->si_pid,SIGCONT);
          }
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
    }

  }
}

void create_and_accept_childs(){

   children = malloc(sizeof(struct program_info)*N);

   for(int i=0;i<N;i++){
     children[i].childs_pid = 0;
     children[i].signal_send = 0;
     children[i].signal_send = 0;
     children[i].received_signal_number = -1;
     //children[i].status = -1;
   }

   currently_received_signals = 0;

   struct sigaction act;
   act.sa_sigaction = au;
   act.sa_flags = SA_SIGINFO;
   sigemptyset(&act.sa_mask);
   sigaction(SIGUSR1,&act,NULL);

   struct sigaction act_real;
   act_real.sa_sigaction = print_data;
   act_real.sa_flags = SA_SIGINFO;
   sigemptyset(&act_real.sa_mask);

   for(int i=SIGRTMIN;i<SIGRTMAX;i++){
     sigaction(i,&act_real,NULL);
   }

   for(int i=0;i<N;i++){
      children[i].childs_pid = fork();

      waitpid(children[i].childs_pid,&children[i].status,WNOHANG);
      printf("Child: %d ended with exit code: %d\n",children[i].childs_pid,WIFEXITED(children[i].status));
      fflush(stdout);

      if(children[i].childs_pid==-1){
        printf("%s\n","Error while forking" );
        leave_no_man_behind(children[i].childs_pid);
        exit(EXIT_FAILURE);
      }
      else if(children[i].childs_pid==0){
        /*for child's process*/
        signal(SIGCONT,signal_initialized);
        int kill_value = kill(getppid(),SIGUSR1);

        //printf("CHILD:Signal %d was sent to %d with result %d\n",SIGUSR1,getppid(),kill_value);
        //fflush(stdout);

        if(kill_value == -1){
          printf("%s\n","Error while sending SIGUSR1" );
        }

        while(is_signal_received==0){
          sleep(1);
        }

        int rand_signal = rand()%32;
        if(kill(getppid(),SIGRTMIN+rand_signal)==-1){
          printf("%s\n","Eror while sending random signal to parent");
        }

        //printf("CHILD RAND: Signal %d was sent to %d\n",rand_signal+SIGRTMIN,getppid());
        srand(time(NULL));
        int seconds = rand()%10;

        sleep(seconds);
        exit(seconds);
      }
   }
   while(1){
    sleep(1);
   }
}


int main(int argc, char** argv){
  M = atoi(argv[1]);
  N = atoi(argv[2]);

  signal(SIGINT,leave_no_man_behind);
  create_and_accept_childs();

  return 0;
}
