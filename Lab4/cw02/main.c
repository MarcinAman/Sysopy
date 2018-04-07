#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>

struct program_info{
  pid_t childs_pid;
  int signal_send;
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

void au(int sig_no){
    if(currently_received_signals<M){
      currently_received_signals++;
    }
    else if(currently_received_signals==M){

      for(int i=0;i<N;i++){
        controled_kill(children[i].childs_pid,SIGCONT);
        children[i].signal_send = 1;
      }

    }
    else{
      /* send signal to all running programs */
      for(int i=0;i<N;i++){
        if(!children[i].signal_send){
          controled_kill(children[i].childs_pid,SIGCONT);
          children[i].signal_send = 1;
        }
      }
    }
}

void print_data(int sig_no){
  printf("%s %d\n","Received signal:",sig_no);
}

void create_and_accept_childs(){

   children = malloc(sizeof(struct program_info)*N);

   for(int i=0;i<N;i++){
     children[i].childs_pid = 0;
     children[i].signal_send = 0;
   }

   currently_received_signals = 0;

   signal(SIGUSR1,au);
   for(int i=0;i<32;i++){
     signal(SIGRTMIN+i,print_data);
   }

   for(int i=0;i<M;i++){
      children[i].childs_pid = fork();

      if(children[i].childs_pid==-1){
        printf("%s\n","Error while forking" );
        leave_no_man_behind(children[i].childs_pid);
        exit(EXIT_FAILURE);
      }
      else if(children[i].childs_pid==0){
        /*for child's process*/
        signal(SIGCONT,signal_initialized);
        int kill_value = kill(getppid(),SIGUSR1);

        if(kill_value == -1){
          printf("%s\n","Error while sending SIGUSR1" );
        }

        while(is_signal_received==0){
          sleep(1);
        }

        if(kill(getppid(),SIGRTMIN+rand()%32)==-1){
          printf("%s\n","Eror while sending random signal to parent");
        }

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
