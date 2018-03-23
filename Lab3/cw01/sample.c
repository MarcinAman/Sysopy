#include <stdio.h>

int main(){

if(fork()){
  return 0;
}
printf("Sample: %d %d\n",(int)getpid(),getppid());
return 0;
}
