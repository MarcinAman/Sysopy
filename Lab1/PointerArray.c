//
// Created by marci on 06/03/2018.
//

#include "PointerArray.h"

pointer_array *createArray(int size_max) {
    if(size_max <= 0) return NULL;

    pointer_array* new_array = malloc(sizeof(struct pointer_array*));
    new_array->array = (char**)malloc(sizeof(char*)*(size_max));
    new_array->size_max = size_max;
    new_array->size_used = 0;

    return new_array;
}

pointer_array *addToArray(char *to_add, pointer_array *array) {
    if(array==NULL){
        pointer_array* new_array = createArray(1);
        return addToArray(to_add,new_array);
    }
    else{
        if(array->size_used < array->size_max){
            for(int i=0;i<array->size_max;i++){
                if(array->array[i]==NULL){
                    array->array[i]=to_add;
                    array->size_used++;
                    return array;
                }
            }
        }
        else{
            pointer_array* new_array = resizeArray(array,2*array->size_max);
            new_array->array[array->size_max] = to_add;
            new_array->size_used++;
            free(array);
            return new_array;
        }
        return array;
    }
}

pointer_array *removeCharArray(char *to_remove, pointer_array *array) {
    for(int i=0;i<array->size_max;i++){
        if(array->array[i]==to_remove){ //we can also use strcmp == 0 if we want to compare meaning not address
            //free(array->array[i]);
            array->array[i] = NULL;
            array->size_used--;
            return array;
        }
    }
    printf("Char array not found\n");
    return array;
}

void printArray(pointer_array *array) {
    if(array == NULL){
        printf("Array is null");
    }
    else{
        printf("Array size = %d \n",array->size_max);
        for(int i=0;i<array->size_max;i++){
            if(array->array[i]==NULL){
                printf("%d. NULL\n",i);
            }
            else{
                printf("%d. %s \n",i,array->array[i]);
            }
        }
    }
}

pointer_array *resizeArray(pointer_array *array, int new_size) {
    if(array == NULL){
        return createArray(new_size);
    }
    else if(new_size < array->size_max){
        return  array;
    }
    else{
        pointer_array* new_array = createArray(new_size);
        for(int i =0;i<array->size_max;i++){
            new_array->array[i] = array->array[i];
        }
        for(int i=array->size_max;i<new_array->size_max;i++){
            new_array->array[i] = NULL;
        }
        new_array->size_used = array->size_used;

        return new_array;
    }

}

char *findClosest(int size, pointer_array *array) {
    if(array == NULL){
        return NULL;
    }
    int min_diff = INT_MAX;
    char* closest_string = NULL;
    for(int i=0;i<array->size_max;i++){
        if(array->array[i]!=NULL){
            int diff = abs(size-strlen(array->array[i]));
            if(diff<min_diff){
                closest_string = array->array[i];
                min_diff = diff;
            }
        }
    }
    return closest_string;
}

void remove_array_with_content(pointer_array *array) {
    for(int i=0;i<array->size_max;i++){
        if(array->array[i]!=NULL){
            free(array->array[i]);
        }
    }
    free(array);
}




