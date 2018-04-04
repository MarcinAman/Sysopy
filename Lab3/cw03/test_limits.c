#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>


void inf_loop(int i) {
    int *arr = malloc(100 * sizeof(int));
    for (int j=0; j<1000000; j++) {
	    arr[j]=j;
      if(j==100000){
         j =0;
         sleep(2);
         arr = malloc(100 * sizeof(int));
      }
    }
    free(arr);
}

void you_slept_too_long(){
  int i =5;
  while(i--){
    sleep(2);
  }
}

int main() {
  you_slept_too_long();
    inf_loop(0);
}
