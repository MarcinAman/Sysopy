//
// Created by Marcin Aman on 21.03.18.
//

#ifndef LAB2_CATALOGDISPLAY_H
#define LAB2_CATALOGDISPLAY_H

#include <unistd.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <ftw.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <limits.h>

void set_time_and_sign(char sign, int year, int month, int day);

int ls_operation(char *path, int is_sys,int dont_print) ;

int fn(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf);

char* stat_to_string_converter(const struct stat *sb);

int sys_nftw(char *path, int (*fn)(const char *, const struct stat *, int, struct FTW *), int fd_limit);

#endif //LAB2_CATALOGDISPLAY_H
