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
    
   if(argc !=2){
    printf("Not a suitable number of program parameters\n");
    return(1);
   }

   /***************************************
   Utworz/otworz posixowy obszar pamieci wspolnej "reprezentowany" przez SHM_NAME
   zapisz tam wartosc przekazana jako parametr wywolania programu
   posprzataj
   *****************************************/

  int fd = shm_open(SHM_NAME,O_CREAT|O_RDWR,S_IRWXU | S_IRWXG);
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
    sprintf(map,argv[1]);

    sleep(3);
    printf("Map: %s\n",map);
    munmap(map,MAX_SIZE);
  shm_unlink(SHM_NAME);

    return 0;
}
