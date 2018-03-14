//
// Created by marci on 10/03/2018.
//

#ifndef LAB1_STATICARRAY_H
#define LAB1_STATICARRAY_H

void fill_array(int array_size, size_t block_array_size);

void print_static_array();

int get_static_ascii_sum(int index);

void remove_block(int index);

int get_closest_element(int index);

void insert_memory_block(char *block);

char* random_string_generator(size_t);

int is_taken(int index);

void clear_array();

#endif //LAB1_STATICARRAY_H
