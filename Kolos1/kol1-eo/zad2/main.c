#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

int didChildEnd = 0;

void randomHandler(int signo){
  printf("Child ended\n");
  didChildEnd = 1;
}

int main()
{
  pid_t child;
  signal(SIGCHLD,randomHandler);
  if((child = fork()) < 0) {
    perror("fork");
    exit(EXIT_FAILURE);
  }
  if(child == 0) {
    sleep(100);
    exit(EXIT_SUCCESS);
  }
  else {
/* Proces macierzysty pobiera status  zakończenie potomka child,
 * nie zawieszając swojej pracy. Jeśli proces się nie zakończył, wysyła do dziecka sygnał SIGKILL.
 * Jeśli wysłanie sygnału się nie powiodło, ponownie oczekuje na zakończenie procesu child,
 * tym razem zawieszając pracę do czasu zakończenia sygnału
 * jeśli się powiodło, wypisuje komunikat sukcesu zakończenia procesu potomka z numerem jego PID i statusem zakończenia. */

  sleep(1);

  int exit_code;

  if(didChildEnd==0){
    kill(child,SIGKILL);
  }

  sleep(1);

  if(waitpid(child,&exit_code,WNOHANG)==-1){
    printf("error on wait pid\n");
    exit(1);
  }

  printf("Exit code:%d\n",WEXITSTATUS(exit_code));


/* koniec*/ 
 } //else
  exit(EXIT_SUCCESS);
}
