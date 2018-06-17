#include<stdio.h>
#include<sys/types.h> 
#include<netinet/in.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<string.h> 
#include<unistd.h> 
#include<stdlib.h>

#define PORT 7844
#define local "127.0.0.1"

int main(int argc,char *argv[]) {

int sockfd,len;

struct sockaddr_in serv,cliaddr;

char buff[2000]; 
if((sockfd=socket(AF_INET,SOCK_DGRAM,0))<0)

{

    perror("ERROR creating socket");

    exit(0);

}

printf("UDP Client Socket Created Successfully.\n"); 

memset(&serv,0,sizeof(serv)); 
serv.sin_family=AF_INET; 

serv.sin_port=htons(PORT);

serv.sin_addr.s_addr=inet_addr(local);

do

{

    printf("Client : ");

    fgets(buff,sizeof(buff),stdin);

    if((sendto(sockfd,buff,sizeof(buff),0,(struct sockaddr *)&serv,sizeof(serv)))<0) {

        perror("ERROR IN SENDTO");

    }

    if(strcmp(buff," ")==0) {

        exit(0);

    } 

    strcpy(buff," ");

    if((recvfrom(sockfd,buff,sizeof(buff),0,(struct sockaddr *)&cliaddr,&len))<0) {

        perror("ERROR IN RECVFROM"); 

        exit(0);

    }

    fputs("From Server : ",stdout); 

    fputs(buff,stdout); 

} while(strcmp(buff," ")!=0);



close(sockfd);

return 0;

}