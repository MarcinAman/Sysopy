//
// Created by marci on 09/03/2018.
//

#include <stdlib.h>
#include <stdio.h>
#include "DynamicPointerArray.h"
#include "string.h"
#include "limits.h"


//base size if a size of a block, size is an amount of blocks
array_structure* create_array(int size, size_t base_size) {
    char** new_array = calloc(size,sizeof(char*));
    for(int i=0;i<size;i++){
        new_array[i] = dynamic_random_string_generator(base_size);
    }

    array_structure* new_object = malloc(sizeof(array_structure*));

    new_object->array = new_array;
    new_object->size = size;

    return new_object;
}

char *dynamic_random_string_generator(size_t base_size) {
    static char* base_string = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    int number_of_characters = base_size/sizeof(char);
    char* new_array = calloc(number_of_characters, sizeof(char));

    for(int i=0;i<number_of_characters;i++){
        new_array[i] = base_string[rand()%strlen(base_string)];
    }

    return new_array;
}

array_structure* remove_array(array_structure* array) {
    if(array!=NULL){
        for(int i=0;i<array->size;i++){
            free(array->array[i]);
            array->array[i]=NULL;
        }
    }
    free(array);
    return NULL;
}

array_structure* remove_array_element(array_structure* array, int to_delete) {

    if(array == NULL) return NULL;

    if(array->size<=to_delete){
        printf("Element out of range \n");
        return array;
    }
    else{
        free(array->array[to_delete]);
        array->array[to_delete]=NULL;
        return array;
    }
    return NULL;
}

array_structure* add_to_array(array_structure* array, char *to_add) {
    if(array==NULL) return NULL;

    for(int i=0;i<array->size;i++){
        if(array->array[i]==NULL){
            array->array[i] = to_add;
            return array;
        }
    }
    printf("No value was added, lack of space \n");

    return array;
}

void print_array(array_structure* array) {
    if(array == NULL){
        printf("Array is null\n");
        return;
    }
    for(int i=0;i<array->size;i++) {
        if (array->array[i] == NULL) {
            printf("%d.NULL\n", i);
        } else {
            printf("%d.%s %d\n", i, array->array[i],get_ascii_sum(array->array[i]));
        }
    }
}

char* search_for_closest_ascii_sum(array_structure *pointer_array, int index) {
    if(pointer_array == NULL || pointer_array->size <= index){
        return NULL;
    }
    int closest_diff = INT_MAX;
    int index_ascii_sum = get_ascii_sum(pointer_array->array[index]);
    int closest_index = -1;
    for(int i=0;i<pointer_array->size;i++){
        if(pointer_array->array[i]!=NULL){
            int current_diff = abs(index_ascii_sum-get_ascii_sum(pointer_array->array[i]));
            if(current_diff<closest_diff && i!=index){
                closest_index = i;
                closest_diff = current_diff;
            }
        }
    }

    return pointer_array->array[closest_index];
}

int get_ascii_sum(char *array) {
    if(array==NULL) return -1;

    int sum = 0;
    for(int i=0;i<strlen(array);i++){
        sum+=array[i];
    }

    return sum;
}



