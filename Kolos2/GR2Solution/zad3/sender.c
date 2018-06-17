#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>


#define KEY "./queuekey"

struct buffer{
  long mtype;
  char mtext[1];
};

int main(int argc, char* argv[])
{

 if(argc !=2){
   printf("Not a suitable number of program parameters\n");
   return(1);
 }

 /******************************************************
 Utworz kolejke komunikatow systemu V "reprezentowana" przez KEY
 Wyslij do niej wartosc przekazana jako parametr wywolania programu
 Obowiazuja funkcje systemu V
 ******************************************************/
  key_t k = ftok(KEY, 1);
    int q = msgget(k, 0644);
    int val = atoi(argv[1]);

    struct buffer* buf = malloc(sizeof(struct buffer)-sizeof(long));

    buf->mtype = 1;

    memcpy(buf->mtext, &val, sizeof(val));

    if(msgsnd(q, &buf, sizeof(val), 0) < 0){
        perror("msgsnd");
    }


  return 0;
}



