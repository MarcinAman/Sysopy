#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>
#include <stdio.h>

#define LEN 255

int main(int argc,char** argv){

  if(argc <= 2){
    printf("%s\n","Not enough args");
    return 0;
  }

  char* path = argv[1];
  int N = atoi(argv[2]);

  char buffer1[LEN];
  char buffer2[LEN];

  int pipe_value = open(path,O_WRONLY);

  int i;
  for(i=0;i<N;i++){
    FILE* popen_val = popen("date","r");
    fgets(buffer1,LEN,popen_val);
    pid_t pid_value = getpid();

    /* append date to pid and move to buffer1, because there is no itoi ;( */

    sprintf(buffer2,"Pid: %ld Date: %s \n",pid_value,buffer1);
    //printf("%s",buffer2);

    write(pipe_value,buffer2,strlen(buffer2));

    srand(time(NULL));
    sleep(rand()%4+2);
  }
  return 0;
}
