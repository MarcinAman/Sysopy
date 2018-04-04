#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <math.h>


void some_random_cpu_operations(int amount){
  double x = 3.1415926;
  double y = 55555;
  for (int i = 0; i < amount; ++i) {
      for (int j = 0; j < 100000; ++j) {
          x = sin(y);
          y = sin(x);
      }
  }
}

void me_sleeping_on_saturday(int times){
  int* arr;
  for(int i=0;i<times;i++){
    for(int j=0;j<i;j++){
      arr = malloc(sizeof(int)*1024*1024);
    }
  }
}

int main(int argc, char** argv) {
  some_random_cpu_operations(atoi(argv[1]));
  me_sleeping_on_saturday(atoi(argv[2]));
}
