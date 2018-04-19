#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#define LEN 255

#define check_fifo(){if(fifo_result==-1) {printf("%s\n","Error while making fifo\n"); return 1;}}
#define check_pipe(){if(fopen_result==NULL) {printf("%s\n","Error while opening\n"); return 1;}}

int main(int argc,char** argv){

  if(argc != 2){
    printf("Wrong args to master, we need 1\n");
    return 1;
  }

  char* path = argv[1];
  char buffer[LEN];

  int fifo_result = mkfifo(path,S_IWUSR | S_IRUSR);
  check_fifo();

  FILE* fopen_result = fopen(path,"r");
  check_pipe();

  while(fgets(buffer,LEN,fopen_result)){
    if(buffer[0]!=32) printf("Read from %ld (Master): %s",getpid(),buffer);
    fflush(stdout);
  }

  fclose(fopen_result);

  return 0;
}
