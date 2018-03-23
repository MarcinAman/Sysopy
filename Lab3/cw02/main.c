#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#define check_positions(x,y) if(x==10 || y ==50) printf("Positions error \n" ); fclose(handle); exit(EXIT_FAILURE);

struct program{
  char* name;
  char** argv;
  int argc;
}

int exec_command(char** commands,int size){
  pid_t fork_value = fork(); /* we can use vfork as it will be nice to do not duplicate data */
  if(fork_value==0){
   //int exec_value = execvpe(commands[0],commands+50);
  }
  return 0;
}

void read_programs_from_file(char* path){
  FILE* handle = fopen(path,"r");

  if(handle==NULL){
    printf("%s\n","Couldnt open file" );
    exit(EXIT_FAILURE);
  }

  char** commands = (char**)malloc(sizeof(char*)*10);
  int buffer_size_per_read = 1;
  char base_buffer[1]; /* not the most efficient way but it works */

  int chars_read;
  int position_in_buffer=0;
  int commands_iterator = 0;

  do{
    chars_read = fread(base_buffer,sizeof(char),buffer_size_per_read,handle);
    if(base_buffer[0]==' '){
      /*i suppose that every command will be divided by a \n sign */
      check_positions(commands_iterator,position_in_buffer);
      commands[++commands_iterator] = (char*)calloc(50,sizeof(char));
      position_in_buffer = 0;
    }
    else if(base_buffer[0]=='\n'){
      /* exec command  and clear buffer*/
      for(int i=0;i<commands_iterator;i++) free(commands[i]);
      commands_iterator = 0;
      position_in_buffer = 0;
    }
    else{
      check_positions(commands_iterator,position_in_buffer);
      commands[commands_iterator][position_in_buffer++]=base_buffer[0];
    }
  }while(chars_read==buffer_size_per_read);

  fclose(handle);
}

int main(int argc, char** argv){
  read_programs_from_file(argv[1]);
  return 0;
}
