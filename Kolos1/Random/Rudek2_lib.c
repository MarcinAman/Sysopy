#include <stdio.h>
#include <string.h>
//otwórz plik podany jako argument i skopiuj jego zawartość do pliku podanego jako drugi argument

int main(int argc, char** argv){

    FILE* file1 = fopen(argv[1],"r");

    if(file1 == NULL){
        printf("error while opening file\n");
        return -1;
    }

    FILE* file2 = fopen(argv[2],"a+");

    char buffer[200];

    while(fread(buffer,sizeof(char),200,file1)!=0){
        fwrite(buffer,sizeof(char),strlen(buffer),file2); //+ check 
        printf("%s\n",buffer);
        int i;
        for(i=0;i<strlen(buffer);i++){
            buffer[i]=0;
        }
    }

    return 0;
}