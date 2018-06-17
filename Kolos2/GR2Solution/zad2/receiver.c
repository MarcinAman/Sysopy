#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#define UNIX_PATH_MAX 108
#define SOCK_PATH "/home/woolfy/Sysopy/Kolos2/GR2Solution/zad2/"


int main() {
    int fd = -1;

    /*********************************************
    Utworz socket domeny unixowej typu datagramowego
    Utworz strukture adresowa ustawiajac adres/sciezke komunikacji na SOCK_PATH
    Zbinduj socket z adresem/sciezka SOCK_PATH
    **********************************************/
    struct sockaddr_un addr = {
        .sun_family = AF_UNIX,
        .sun_path = SOCK_PATH
    };

    fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    bind(fd,(const struct sockaddr *) &addr, sizeof(struct sockaddr_un));


    char buf[20];
    if(read(fd, buf, 20) == -1)
        perror("Error receiving message");
    
    printf("R: %s",buf);

    // if(connect(fd, (const struct sockaddr *) &addr, sizeof(struct sockaddr_un))==-1){
    //     perror("connect");
    // }

    socklen_t len = 0;
    if(sendto(fd,buf,sizeof(buf),0,(struct sockaddr*)&addr,len)==-1){
        perror("send");
    }

    /***************************
    Posprzataj po sockecie
    ****************************/
    shutdown(fd, SHUT_RDWR);
    close(fd);
    unlink(SOCK_PATH);

    return 0;
}

