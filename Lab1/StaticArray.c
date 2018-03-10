//
// Created by marci on 10/03/2018.
//

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "StaticArray.h"

array* create_static_array(int size) {
    int block_size = 10;
    static char* base_string = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

    char new_array[size][block_size+1];

    for(int i=0;i<size;i++){
        for(int j=0;j<block_size;j++){
            new_array[i][j] = base_string[rand()%strlen(base_string)];
        }
        new_array[i][block_size] = '\0'; //because string has to end with \0
                                        // otherwise it wont be displayed as string
        printf("F:%d.%s\n",i,new_array[i]);
    }

    return &new_array;
}



