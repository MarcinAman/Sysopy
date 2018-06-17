#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define UNIX_PATH_MAX 108
#define SOCK_PATH "/home/woolfy/Sysopy/Kolos2/GR2Solution/zad2/"

int main(int argc, char *argv[]) {


   if(argc !=2){
    printf("Not a suitable number of program parameters\n");
    return(1);
   }
    sleep(1);


    /*********************************************
    Utworz socket domeny unixowej typu datagramowego
    Utworz strukture adresowa ustawiajac adres/sciezke komunikacji na SOCK_PATH
    Polacz sie korzystajac ze zdefiniowanych socketu i struktury adresowej
    ***********************************************/
    int fd = -1;

    struct sockaddr_un addr = {
        .sun_family = AF_UNIX,
        .sun_path = SOCK_PATH
    };

    fd = socket(AF_UNIX, SOCK_DGRAM, 0);

    if(connect(fd, (const struct sockaddr *) &addr, sizeof(struct sockaddr_un))==-1){
        perror("Sender connect");
    }


    char buff[20];
    int to_send = sprintf(buff, "echo kurwa\n");

    if(sendto(fd, buff, to_send+1,0,(struct sockaddr*)&addr,0) == -1) {
        perror("Error sending msg to server");
    }

    if(read(fd,buff,strlen(buff)+1)==-1){
        perror("recv");
    }

    printf("S: %s",buff);
    sleep(10);


    /*****************************
    posprzataj po sockecie
    ********************************/
    shutdown(fd, SHUT_RDWR);
    close(fd);
    return 0;
}

