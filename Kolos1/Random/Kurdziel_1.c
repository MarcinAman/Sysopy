// Wtorek 9:35 Kurdziel
// 1. Podłączyć bibliotekę dzielona dynamicznie i zwrócić funkcje z daną nazwą
// 2. była funkcja przyjmująca deskryptory, trzeba było zrobić childa i parenta w jednym przekierować STDIN na pierwszy deskryptor i go zamknąć, w drugim analogicznie, i pozwracać wartość forka() (0 i pid childa)
// 3. Ustawić przechwytywanie sygnału i dodatkowo blokować inny sygnał

#include<stdio.h>

int main(int argc, char** argv){
    //Ad1:
    /*
    int (*jakasfunkcja)(int);

    void* handle = dlopen("./lib.so",RTDL_LAZY);
    if(handle==NULL){
        printf("Error");
    }

    jakasfunkcja = dlsym(handle,"jakasfunkcja");
    */

   /*
    Ad2:
    Generalna zasada to read na 0 write na 1
    no i jeszcze dup2 przydaloby sie znac
   */

   /*
    Ad3:
    struct sigaction act;
    sigfillset(&act.sa_mask);
    sigdelset(&act.sa_mask,SIGUSR1);
    act.sa_handker = somefunction;
    sigaction(SIGUSR2,&act,NULL)
   */

    return 0;
}