//
// Created by woolfy on 14.03.18.
//

#include <unistd.h>

#ifndef LAB2_SYSOPERATION_H
#define LAB2_SYSOPERATION_H

int sys_generate(char* filename, int records, ssize_t size);

int sys_copy(char* file1, char* file2,size_t buffer_size);

int sys_sort(char* file,int records,ssize_t size);

//generator made to make our life a easier. It makes a file with numbers so comparing less miserable

int special_generator(char* filename, int records, ssize_t size);

#endif //LAB2_SYSOPERATION_H
