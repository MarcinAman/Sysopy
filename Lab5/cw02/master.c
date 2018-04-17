#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define LEN 255

#define check_fifo(){if(fifo_result==-1) {printf("%s\n","Error while making fifo\n"); return 1;}}
#define check_pipe(){if(fopen_result==NULL) {printf("%s\n","Error while opening\n"); return 1;}}

int main(int argc,char** argv){
  if(argc < 1){
    printf("Not enough args to master\n");
    return 1;
  }

  printf("%s\n","Kurwa1");
  char* path = argv[1];
  char buffer[LEN];
  printf("%s %s\n","Kurwa2",path);
  int fifo_result = mkfifo(path,S_IWUSR | S_IRUSR);
  printf("%s\n","Kurwa2.5");
  check_fifo();

  printf("%s\n","Kurwa3");
  FILE* fopen_result = fopen(path,"r");

  check_pipe();
  printf("%s\n","Kurwa4");

  while(fgets(buffer,LEN,fopen_result)){
    printf("Read from %ld: %s\n",getpid(),buffer);
    write(STDOUT_FILENO,buffer,LEN);
  }
  printf("%s\n","Kurwa5");

  fclose(fopen_result);

  return 0;
}
