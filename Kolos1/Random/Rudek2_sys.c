#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char ** argv){
    int file1 = open(argv[1],O_RDONLY);
    int file2 = open(argv[2],O_WRONLY|O_CREAT);

    if(file1 == 0 || file2 == 0){
        close(file1);
        close(file2);
        return -1;
    }

    char buffer[200];
    size_t rw = 0;
    while((rw = read(file1,buffer,sizeof(buffer)))!=0){
        write(file2,buffer,strlen(buffer)*sizeof(char)); //check
        printf("%d\n",rw);
        // printf("%s\n",buffer);
        int i;
        for(i=0;i<strlen(buffer);i++){
            buffer[i]=0;
        }
    }

    close(file1);
    close(file2);
    
    return 0;
}