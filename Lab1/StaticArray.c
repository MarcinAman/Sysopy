//
// Created by marci on 10/03/2018.
//

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "StaticArray.h"
#include <limits.h>

char static_array[1000000][1000];

int size;
size_t block_size;
int max_size = 1000000;

void fill_array(int array_size, size_t block_array_size){
    size = array_size;
    block_size = block_array_size;

    for(int i=0;i<max_size;i++) static_array[i][0]=0;

    int chars = block_array_size/sizeof(char);
    static char* base_string = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

    for(int i=0;i<size;i++){
        for(int j=0;j<chars-1;j++){
            static_array[i][j] = base_string[rand()%strlen(base_string)];
        }
        static_array[i][chars-1] = '\0';
    }
}

void print_static_array(){
    for(int i=0;i<size;i++){
        printf("%d.%s %d\n",i,static_array[i],get_static_ascii_sum(i));
    }
}

int get_static_ascii_sum(int index){
    if(index >= size) return -1;

    int sum = 0;
    for(int i=0;i<block_size/sizeof(char)-1;i++){
        sum += static_array[index][i];
    }

    return sum;
}

void remove_block(int index){
    if(index >= size) return;

    for(int i=0;i<(block_size/sizeof(char));i++){
        static_array[index][i] = 0;
    }
}

int get_closest_element(int index){
    if(index >= size) return -1;

    int sum = get_static_ascii_sum(index);
    int min_diff = INT_MAX;
    int min_index = -1;

    for(int i=0;i<size;i++){
        int current_sum = get_static_ascii_sum(i);
        if(min_diff > abs(current_sum-sum) && i != index){
            min_diff = abs(current_sum-sum);
            min_index = i;
        }
    }

    return min_index;
}

void insert_memory_block(char *block) {
    for(int i=0;i<size;i++){
        if(static_array[i][0]==0){
            for(int j=0;j<strlen(block);j++){
                static_array[i][j] = block[j];
            }
            return;
        }
    }

    printf("Lack of space to insert block\n");
}

char* random_string_generator(size_t base_size){
    static char* base_string = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    int number_of_characters = base_size/sizeof(char);
    char* new_array = calloc(number_of_characters, sizeof(char));

    for(int i=0;i<number_of_characters;i++){
        new_array[i] = base_string[rand()%strlen(base_string)];
    }

    return new_array;
}

int is_taken(int index){
    if(static_array[index][0]==0) return 0;
    return 1;
}

void clear_array(){
    for(int i=0;i<max_size;i++){
        remove_block(i);
    }
}





