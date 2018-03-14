//
// Created by marci on 09/03/2018.
//

#ifndef LAB1_DYNAMICPOINTERARRAY_H
#define LAB1_DYNAMICPOINTERARRAY_H

typedef struct{
    char** array;
    int size;
}array_structure;

char* dynamic_random_string_generator(size_t);

int get_ascii_sum(char*);

char* search_for_closest_ascii_sum(array_structure*,int);

array_structure* create_array(int, size_t);

array_structure* remove_array(array_structure*);

array_structure* remove_array_element(array_structure*,int);

array_structure* add_to_array(array_structure*,char*);

array_structure* load_static_array(int,size_t);

void print_array(array_structure*);

#endif //LAB1_DYNAMICPOINTERARRAY_H
