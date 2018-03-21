//
// Created by woolfy on 14.03.18.
//

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "LibOperation.h"

const static int buffer_size = 500;
static char* base_string = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

int lib_generate(char *filename, int records, ssize_t size) {
    FILE* file = fopen(filename,"wb");
    if(file==NULL){
        printf("Error while opening file in lib_generate\n");
        return 1;
    }

    char buffer[buffer_size];

    int chars = (records*size)/sizeof(char);
    int buffer_index = 0;
    for(int i=0;i<chars;i++){
        if(buffer_index==buffer_size){
            //size_t x = fwrite(buffer, sizeof(char), sizeof(char)*buffer_size, file);
            int x = fputs(buffer, file);
            if(x != buffer_size){
                printf("Error while writting to file in lib_generate \n");
                return 1;
            }
        }
        else if(i!=0 && i%size==0){
            buffer[buffer_index] = '\n';
            buffer_index++;
        }
        else{
            buffer[buffer_index] = base_string[rand()%strlen(base_string)];
            buffer_index++;
        }
    }

    size_t  x = fwrite(buffer, sizeof(char), sizeof(char)*buffer_index, file);

    fclose(file);
    return 0;
}

int lib_copy(char *file1, char *file2, int size) {
    FILE* file_o = fopen(file1,"rb");
    FILE* file_w = fopen(file2,"wba+");

    if(file_o == NULL || file_w == NULL){
        printf("Error while opening files in lib_copy\n");
        return 1;
    }

    char* buffer = calloc(size,sizeof(char));

    int iterator = 1;

    do{
        size_t result = fread(buffer,sizeof(char),size,file_o);
        if(result == 0 || buffer[result]==EOF){
            iterator = 0;
        }
        int x = fputs(buffer,file_w);
        //printf("%s\n",buffer);
        if(x != 1){
            printf("Error while writing to file in lib_copy %d %ld %s\n",x,result,buffer);
            return 1;
        }
    }while(iterator!=0);

    fclose(file_w);
    fclose(file_o);

    return 0;
}

int lib_sort(char *file, int records, ssize_t size) {
    FILE* handle1 = fopen(file,"rwba+");

    char* buffer = calloc(size,sizeof(char));
    char* buffer2 = calloc(size,sizeof(char));


    for(int i=1;i<records;i++){
        fseek(handle1,i*size,SEEK_SET);
        fread(buffer, sizeof(char),size,handle1);
        fseek(handle1,(i-1)*size,SEEK_SET);
        fread(buffer2, sizeof(char),size,handle1);
        //w buffer jest aktualny a w buffer 2 poprzedni

        int j=i-1;
        while(j>=0 && buffer2[0]>buffer[0]){
            fseek(handle1,j*size,SEEK_SET);
            fread(buffer2, sizeof(char),size,handle1);
            fseek(handle1,(j+1)*size,SEEK_SET);
            fputs(buffer2,handle1);
            j--;
            fseek(handle1,(j+1)*size,SEEK_SET);
            fputs(buffer,handle1);
        }
    }

    return 0;
}
