#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>

/* Zadanie 1a: */
int state = 1;

void signal_initialized(int signal_no){
  if(signal_no == SIGINT){
    printf("%s\n","SIGINT received, terminating..." );
    exit(0);
  }
  printf("\nSignal %d received, waiting for another one to continue\n",signal_no);
  state ++;
  state %= 2;
}

void  display_date(){
  time_t secs;
  struct tm *time_structure;

  signal(SIGTSTP,signal_initialized);
  signal(SIGINT,signal_initialized);

  while(1){
    if(state){
      time(&secs);
      time_structure = localtime(&secs);
      char buffer[100];
      strftime (buffer, 100, "Current time: %c",time_structure);
      printf("%s\n", buffer);
    }
    sleep(2);
  }
}

/*Zadanie 1b: */

void display_date_child();
pid_t child = -1;
char* date_program[] = {"./date.sh",NULL};

void kill_child(int signal_no){
  printf("\n%s\n","Signal received" );
  if(child == -1){
    child = fork();
    if(child==0){
        display_date_child();
    }
  }
  else{
    int kill_value = kill(child,SIGTERM);
    if(kill_value==-1){
      printf("%s %d\n","Error while sending SIGTERM signal to pid ",child );
    }
    child = -1;
  }
  /* we pass if child==0 because it is a child process itself */
}

void end_program(int signal_no){
  printf("%s\n","Terminating...");
  if(child!=-1){
    int kill_value = kill(child,SIGTERM);

    if(kill_value==-1){
      printf("%s %d\n","Error while sending SIGTERM signal to pid ",child );
    }
  }
  exit(0);
}

void display_date_child(){
  if(child==-1){
    child = fork();
  }
  if(child==-1){
    printf("Couldn't add new child process, terminating\n" );
    exit(EXIT_FAILURE);
  }

  if(child == 0){                /*for child process*/
    int exec_value = execvp("./date.sh",date_program);

    if(exec_value == -1){
      printf("%s\n","Error while executing script, terminating\n" );
      exit(EXIT_FAILURE);
    }
  }
  else{
    signal(SIGINT,end_program);
    signal(SIGTSTP,kill_child);
  }
  while(1){
    sleep(1);
  }
}

int main(int argc, char** argv){
  if(argc==1){
    printf("%s\n%s\n","\"a\"-> Non-child version","\"b\"-> Child version");
    return 0;
  }
  if(strcmp(argv[1],"a")==0){
    display_date();
  }
  else{
    display_date_child();
  }
  return 0;
}
