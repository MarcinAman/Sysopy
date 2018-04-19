#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

#define max_command_line_len 255
#define max_commans 100;
#define copy_substring(i,i_prev){int m = 0; for(int j=i_prev;j<i+1;j++){parsed_arguments[parsed_arguments_iterator][m++]=get_line_buffer[j];}parsed_arguments[parsed_arguments_iterator][m]='\0';}
#define close_all_pipes(){close(pipes[0][0]);close(pipes[0][1]);close(pipes[1][0]);close(pipes[1][1]);}
#define CURRENT i%2
#define WRITING 1
#define READING 0
#define NEXT (i+1)%2

pid_t fork_value;
int pipe_len,command_len;

int get_pipes_amount(char* input){
  int pipes = 0;
  int i;
  for(i=0;i<strlen(input);i++){
    if(input[i]=='|') pipes++;
  }

  return pipes;
}


int get_spaces(char* input){
  int spaces = 0;
  int i;
  for(i=0;i<strlen(input);i++){
    if(input[i]=='|') spaces++;
  }
  return spaces;
}

char* trim_white(char *orig_str){
    char* buffer = malloc(sizeof(char) * max_command_line_len);
    char* i = orig_str;
    while(*i == ' ') i++;
    int j = 0;
    while(*i != 0){
        while((*i != ' ') && (*i != 0)){
            buffer[j++] = *i;
            i++;
        }
        if(*i == ' '){
            while(*i == ' ') i++;
            if (*i != 0)
                buffer[j++] = ' ';
            }
    }
    buffer[j+1] = 0;
    return buffer;
}

char** parse_pipes(char* input){
  char **arr = malloc(sizeof(char*)*(get_pipes_amount(input)+1));

  int iterator = 0;
  char* delim = "|";
  char* buff = strtok(input,delim);
  while(buff){
    arr[iterator++] = trim_white(buff);
    buff = strtok(NULL,delim);
  }

  pipe_len = iterator;

  return arr;
}

char** parse_program(char* input){
  int len = get_spaces(trim_white(input));
  char **arr = malloc(sizeof(char*)*(len+1));

  int iterator = 0;
  char* delim = " \n";
  char* buff = strtok(input,delim);
  while(buff){
    arr[iterator++] = trim_white(buff);
    buff = strtok(NULL, delim);
  }
  command_len = iterator;
  arr[iterator] = NULL;
  return arr;
}

void execute_pipes(char* input){
  if(!input) return;
  printf("Command:%s",input);
  printf("-----------------------------\n");
  char** parsed_pipes = parse_pipes(input);
  int pipes[2][2];
  int j,i;
  for(i = 0;i<pipe_len+1;i++){
    if (i > 0) {
      close(pipes[CURRENT][WRITING]); /* everything except first run */
      close(pipes[CURRENT][READING]); /* pipers are left from prev runs */
    }

    if(pipe(pipes[CURRENT]) == -1) { /* Open current pipe connection */
      printf("Error on pipe.\n");
      exit(EXIT_FAILURE);
    }

    pid_t cp = fork();
    if (cp == 0) {
      char ** exec_params = parse_program(parsed_pipes[i]);

    if ( i  !=  pipe_len-1) { /*everything except last because we want to have an output displayed on STDOUT*/
        close(pipes[CURRENT][READING]);
        if (dup2(pipes[CURRENT][WRITING], STDOUT_FILENO) < 0) {
            close_all_pipes();
            return;
        }
    }
    if (i != 0) {
        close(pipes[NEXT][WRITING]);
        if (dup2(pipes[NEXT][READING], STDIN_FILENO) < 0) { /*Without first run since we will write to STDIN */
            close_all_pipes();
            return;
        }
    }
      execvp(exec_params[0], exec_params);
      exit(0);
    }
  }

  wait(NULL);
}

void read_programs_from_file(char* path){
  FILE* handle = fopen(path,"r");

  if(handle==NULL){
    printf("%s\n","Couldnt open file" );
    exit(EXIT_FAILURE);
  }

  char* get_line_buffer  = (char*)calloc(255,max_command_line_len);

  size_t buffer_size = max_command_line_len*sizeof(char);

  if(get_line_buffer == NULL){
    printf("Couldnt allocate memory for buffer \n" );
    fclose(handle);
    exit(EXIT_FAILURE);
  }

  while(fgets(get_line_buffer,buffer_size,handle)){
    execute_pipes(get_line_buffer);

    int i;
    for(i=0;i<buffer_size;i++) get_line_buffer[i] = 0;
  }

  fclose(handle);
}

void kill_child(int signo){
  printf("%s\n","Killing child...");
  kill(fork_value,SIGTERM);
  exit(0);
}

int main(int argc, char** argv){
  signal(SIGINT,kill_child);
  read_programs_from_file(argv[1]);
  return 0;
}
