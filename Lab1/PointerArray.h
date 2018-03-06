//
// Created by marci on 06/03/2018.
//

#ifndef LAB1_POINTERARRAY_H
#define LAB1_POINTERARRAY_H

#include "stdio.h"
#include <stdlib.h>
#include <limits.h>
#include <string.h>

typedef struct{
    char** array;
    int size_max;
    int size_used;
}pointer_array;

//Basic funcionality:

pointer_array* createArray(int); //creating an array with pointers with a specific size
pointer_array* addToArray(char*,pointer_array*); //adding another pointer to an array
pointer_array* removeCharArray(char*, pointer_array*); //remove certain array from structure
char* findClosest(int,pointer_array*); //find a char* with length closest to int
void remove_array_with_content(pointer_array*);

//Additional funcions:
pointer_array* resizeArray(pointer_array*,int); //resizing an array to allow user to add new pointers
void printArray(pointer_array*);

#endif //LAB1_POINTERARRAY_H
