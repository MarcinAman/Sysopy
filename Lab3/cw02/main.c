#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#define max_command_line_len 255
#define copy_substring(i,i_prev){int m = 0; for(int j=i_prev;j<i;j++){parsed_arguments[parsed_arguments_iterator][m++]=get_line_buffer[j];}parsed_arguments[parsed_arguments_iterator][m]='\0';}


struct program{
  char** argv;
  int argc;
};

int exec_command(struct program* to_execute){
  if(to_execute == NULL) return 0;

  pid_t fork_value = fork();
  if(fork_value==0){
        exit(0);
  }
  int exec_value = execvp(to_execute->argv[0],to_execute->argv);
  return 0;
}

struct program* parse_command_line_arguments(char* get_line_buffer){
  int spaces = 1;
  int get_line_buffer_len = strlen(get_line_buffer);

  if(get_line_buffer_len==0) return NULL;

  for(int i=0;i<get_line_buffer_len;i++){
    if(get_line_buffer[i]==' ' && i!=(get_line_buffer_len-1)) spaces ++;
  }

  char** parsed_arguments = (char**)malloc(sizeof(char*)*(spaces+1));
  int i_prev = 0;
  int parsed_arguments_iterator = 0;
  for(int i=0;i<get_line_buffer_len;i++){
    if(get_line_buffer[i]==' ' || i==(get_line_buffer_len-1)){

      parsed_arguments[parsed_arguments_iterator] = calloc(i-i_prev+1,sizeof(char));

      copy_substring(i,i_prev);

      parsed_arguments_iterator++;
      i_prev = i+1;
    }
  }

  parsed_arguments[spaces]=NULL;

  struct program* some_program = malloc(sizeof(struct program));
  some_program->argv = parsed_arguments;
  some_program->argc = spaces;

  for(int i=0;i<=spaces;i++) printf("%d.%s\n",i,parsed_arguments[i]);

  return some_program;
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

size_t characters = 0;

while(characters!=-1){
    struct program* current_program = parse_command_line_arguments(get_line_buffer);
    exec_command(current_program);
    for(int i=0;i<characters;i++) get_line_buffer[i]=0;
    characters = getline(&get_line_buffer,&buffer_size,handle);
}

  fclose(handle);
}

int main(int argc, char** argv){
  read_programs_from_file(argv[1]);
  return 0;
}
