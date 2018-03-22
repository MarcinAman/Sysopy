//
// Created by woolfy on 14.03.18.
//

#ifndef LAB2_LIBOPERATION_H
#define LAB2_LIBOPERATION_H

#include <unistd.h>

int lib_generate(char* filename, int records, ssize_t size);

int lib_copy(char* file1, char* file2,int buffer_size);

int lib_sort(char *file, int records, ssize_t size);

void perform_fseek_check(FILE* handle, int fseek_return);

#endif //LAB2_LIBOPERATION_H
