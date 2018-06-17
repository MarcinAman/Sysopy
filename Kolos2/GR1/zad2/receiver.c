#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <mqueue.h>
#include <unistd.h>
#include <sys/mman.h>

#define SHM_NAME "/kol_shm"
#define MAX_SIZE 1024

int main(int argc, char **argv)
{

    sleep(1);
    int val =0;
    /*******************************************
    Utworz/otworz posixowy obszar pamieci wspolnej "reprezentowany" przez SHM_NAME
    odczytaj zapisana tam wartosc i przypisz ja do zmiennej val
    posprzataj
    *********************************************/

    int fd = shm_open(SHM_NAME,O_RDWR,S_IRWXU | S_IRWXG);
  if(fd ==-1){
      perror("Shm open");
      return 1;
  }

  int truc =  ftruncate(fd,MAX_SIZE);
  if(truc == -1){
      perror("Truncate");
      return 1;
  }

  char* map = (char*)mmap(NULL,MAX_SIZE,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);

  if(map == (void*)-1){
      perror("Map");
      return 1;
  }
  printf("Map_r: %s\n",map);

  val = atoi(map);

  shm_unlink(SHM_NAME);

	printf("%d square is: %d \n",val, val*val);
    return 0;
}
