#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>

pid_t* childs_pid;
int currently_received_signals;
int M;            /*M is required amout to run */
int is_signal_received = 0;

void signal_initialized(int sig_no){
  is_signal_received = 1;
}

void au(int sig_no){
    if(currently_received_signals<M){
      currently_received_signals++;
    }
    else{
      /* send signal to all running programs */
      int kill_value = kill(0,SIGCONT);

      if(kill_value==-1){
        printf("%s\n","Error while sending SIGUSR1" );
      }

    }
}

void create_and_accept_childs(int N){
   /* N is a total amount of programs, */

   childs_pid = calloc(N,sizof(pid_t));
   currently_received_signals = 0;

   struct sigaction act;
   act.sa_handler = au;
   sigemptyset(&act.sa_mask);
   act.sa_flags = 0;
   sigaction(SIGUSR1, &act, NULL);

   for(int i=0;i<M;i++){
      childs_pid[i] = fork();

      if(childs_pid[i]==-1){
        printf("%s\n","Error while forking" );
        exit(EXIT_FAILURE); /* mby terminate others? */
      }
      else if(childs_pid[i]==0){
        /*for child's process*/
        signal(SIGCONT,signal_initialized);
        int kill_value = kill(getppid(),SIGUSR1);

        if(kill_value == -1){
          printf("%s\n","Error while sending SIGUSR1" );
        }

        while(is_signal_received==0){
          sleep(1);
        }
        
      }

   }

}

int main(int argc, char** argv){
  return 0;
}
