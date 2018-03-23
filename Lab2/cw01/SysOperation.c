//
// Created by woolfy on 14.03.18.
//

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <values.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "SysOperation.h"

static const int buffer_size = 500;
static char* base_string = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

void perform_lseek_check(int handle, __off_t lseek_return){
    if(lseek_return<0){
        close(handle);
        printf("Error while performing lseek\n");
        exit(EXIT_FAILURE);
    }
}

int sys_generate(char *filename, int records, ssize_t size) {
    //int handle = open(filename,O_RDWR|O_CREAT|O_EXCL,S_IROTH|S_IRGRP|S_IRUSR);
    //http://codewiki.wikidot.com/c:system-calls:open

    int handle = open(filename,O_RDWR|O_CREAT|O_EXCL,0777);

    int char_size = size/ sizeof(char);

    char buffer[buffer_size];

    if(handle==-1){
        printf("Error while opening file in generate function\n");
        return 2;
    }

    ssize_t chars_to_write = records * char_size;

    int buffer_index = 0;
    for(ssize_t i=1;i<=chars_to_write;i++){
        if(buffer_index<buffer_size && i%size == 0 && i!=0){
            buffer[buffer_index] = '\n';
            buffer_index++;
        }
        else if(buffer_index<buffer_size){
            buffer[buffer_index] = base_string[rand()%strlen(base_string)];
            buffer_index++;
        }
        else{
            int wrote = (int) write(handle, buffer, buffer_size);
            if(wrote!=buffer_size){
                printf("Error while writing to file in generate function\n");
                return 2;
            }
            buffer_index = 0;
        }
    }

    int wrote = (int) write(handle, buffer,chars_to_write%buffer_size);

    if(wrote!=chars_to_write%buffer_size){
        printf("Error while writing to file in generate function\n");
    }

    close(handle);
    return 0;
}

int sys_copy(char *file1, char *file2,size_t buffer_s) {
    int handle1 = open(file1,O_RDONLY);
    int handle2 = open(file2,O_CREAT|O_WRONLY|O_EXCL,S_IROTH|S_IRGRP|S_IRUSR);/*
 *
 * we want to have at least read access to this file ;)
 * plus it would be nice to get an error if file exists
*/
    if(handle1 == -1 || handle2 == -1){
        printf("Error while opening files \n");
        return 1;
    }

    char* buffer = calloc(buffer_s,sizeof(char));

    if(buffer==NULL){
        printf("Error while allocating memory in sys_copy\n");
        return 2;
    }

    size_t read_chars = 1;

    while(read_chars!=0){
        read_chars = (size_t) read(handle1, buffer, buffer_s);

        size_t wrote_chars = (size_t) write(handle2, buffer, read_chars);

        if(read_chars!=wrote_chars){
            printf("Error while writing to file\n");
            return 1;
        }

    }

    free(buffer);

    close(handle1);
    close(handle2);

    return 0;
}

int sys_sort(char *file, int records, ssize_t size) {
    //right now it overrides some records but it is the closest to working

    int handle1 = open(file,O_RDWR);

    if(handle1==-1){
        printf("Error while opening file in sys_sort\n");
    }

    char* buffer = calloc(size,sizeof(char));
    char* buffer2 = calloc(size,sizeof(char));

    if(buffer == NULL || buffer2 == NULL){
        close(handle1);
        printf("Error while allocating memory block in sys_sort\n");
        return 2;
    }


    for(int i=1;i<records;i++){
        __off_t value = lseek(handle1,i*size,SEEK_SET);
        perform_lseek_check(handle1,value);

        if(!read(handle1,buffer,size)) return 0;

        value = lseek(handle1,(i-1)*size,SEEK_SET);
        perform_lseek_check(handle1,value);

        if(!read(handle1,buffer2,size)) return 0;;
        //w buffer jest aktualny a w buffer 2 poprzedni

        int j=i-1;
        while(j>=0 && buffer2[0]>buffer[0]){
            value = lseek(handle1,j*size,SEEK_SET);
            perform_lseek_check(handle1,value);

            if(!read(handle1,buffer2,size)) return 0;

            value = lseek(handle1,(j+1)*size,SEEK_SET);
            perform_lseek_check(handle1,value);

            if(write(handle1,buffer2,size)!=size){
                printf("Error while writting to file in sys_sort\n");
                close(handle1);
                return 2;
            }

            j--;
            value = lseek(handle1,(j+1)*size,SEEK_SET);
            perform_lseek_check(handle1,value);

            if(write(handle1,buffer,size)!=size){
                printf("Error while writting to file in sys_sort\n");
                close(handle1);
                return 2;
            }

        }
    }

    return 0;
}


int special_generator(char *filename, int records, ssize_t size) {
    char* some_numbers = "0123456789";

    int handle = open(filename,O_RDWR|O_CREAT|O_EXCL,0777); // 777 cuz i can and no sysadmin will tell me i cant

    int char_size = size/ sizeof(char);

    char buffer[buffer_size];

    if(handle==-1){
        printf("Error while opening file in generate function\n");
        return 2;
    }

    ssize_t chars_to_write = records * char_size;

    int buffer_index = 0;
    char char_to_insert = '5';
    for(ssize_t i=1;i<=chars_to_write;i++){
        if(buffer_index<buffer_size && i%size == 0 && i!=0){
            buffer[buffer_index] = '\n';
            //buffer[buffer_index] = char_to_insert;
            buffer_index++;
            char_to_insert = some_numbers[rand()%strlen(some_numbers)];
        }
        else if(buffer_index<buffer_size){
            buffer[buffer_index] = char_to_insert;
            buffer_index++;
        }
        else{
            int wrote = (int) write(handle, buffer, buffer_size);
            if(wrote!=buffer_size){
                printf("Error while writing to file in generate function\n");
                return 2;
            }
            buffer_index = 0;
        }
    }

    int wrote = (int) write(handle, buffer,chars_to_write%buffer_size);

    if(wrote!=chars_to_write%buffer_size){
        printf("Error while writing to file in generate function\n");
    }

    close(handle);
    return 0;
}
