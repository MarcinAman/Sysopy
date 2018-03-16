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

int print_file(int handle, ssize_t size){
    char* buffer = calloc(size, sizeof(char));
    while(read(handle,buffer, size)>=(size-1)){ //not sure if this 1 is ok
        printf("%s\n",buffer);
    }
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

    /*
    int read_elements = (int) read(handle1, buffer, size);
    read_elements = (int) read(handle1,buffer2,size);

    lseek(handle1,0,SEEK_SET);

    int wrote_elements = (int) write(handle1,buffer2,size);
    write(handle1,buffer,size);
*/


    int sorting_position_in_file = (int) size;
    lseek(handle1,size,SEEK_SET);
    while(sorting_position_in_file<records*size){
        ssize_t chars_read = read(handle1, buffer, size);
        //condition if not

        int loop_iterator = 0;
        lseek(handle1,0,SEEK_SET); // move to begging

        //find place for buffer
        do{
            read(handle1,buffer2,size);
            loop_iterator += size;
        }while(buffer[0]>buffer2[0] && loop_iterator < sorting_position_in_file);

        lseek(handle1,loop_iterator-size,SEEK_SET); //1 position behind

        write(handle1,buffer,size);

        //move others by swapping:

        while(loop_iterator<sorting_position_in_file){
            read(handle1,buffer,size);
            lseek(handle1,loop_iterator,SEEK_SET);
            write(handle1,buffer2,size);
            loop_iterator+=size;
            char* tmp = buffer2;
            buffer2 = buffer;
            buffer = tmp;
        }
        write(handle1,buffer,size);
        sorting_position_in_file+=size;
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
            printf(buffer);
            if(wrote!=buffer_size){
                printf("Error while writing to file in generate function\n");
                return 2;
            }
            buffer_index = 0;
        }
    }

    int wrote = (int) write(handle, buffer,chars_to_write%buffer_size);
    printf(buffer);

    if(wrote!=chars_to_write%buffer_size){
        printf("Error while writing to file in generate function\n");
    }

    close(handle);
    return 0;
}
