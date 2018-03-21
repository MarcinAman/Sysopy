//
// Created by woolfy on 21.03.18.
//
#define _XOPEN_SOURCE 500

#include "CatalogDisplay.h"

/*
struct stat {
    dev_t     st_dev;          ID of device containing file
    ino_t     st_ino;          Inode number
    mode_t    st_mode;         File type and mode
    nlink_t   st_nlink;        Number of hard links
    uid_t     st_uid;          User ID of owner
    gid_t     st_gid;          Group ID of owner
    dev_t     st_rdev;         Device ID (if special file)
    off_t     st_size;         Total size, in bytes
    blksize_t st_blksize;      Block size for filesystem I/O
    blkcnt_t  st_blocks;       Number of 512B blocks allocated


    struct timespec st_atim;   Time of last access
    struct timespec st_mtim;   Time of last modification
    struct timespec st_ctim;   Time of last status change

#define st_atime st_atim.tv_sec
#define st_mtime st_mtim.tv_sec
#define st_ctime st_ctim.tv_sec
};

struct tm {
    int     tm_sec;          sekundy od pełnej minuty
    int     tm_min;          minuty  od pełnej godziny
    int     tm_hour;         godzina na 24-godzinnym zegarze
    int     tm_mday;         dzień miesiąca
    int     tm_mon;          miesiąc licząc od zera
    int     tm_year;         rok - 1900
    int     tm_wday;         dzień tygodnia niedziela ma numer 0
    int     tm_yday;         dzień roku licząc od zera
    int     tm_isdst;        znacznik czasu letniego
};
*/

struct tm sorting_time;

char comparing_sign;

int ls_operation(char *path, int is_sys) {
    char* res_path = realpath(path,NULL);
    // NULL because it will automatically allocate a sufficient char array

    if(res_path==NULL){
        res_path = path; //it is already a absolute path
    }

    if(is_sys==0){
        //int x = nftw(res_path,display_info,20, FTW_PHYS | FTW_DEPTH);
    }

    int x = nftw(res_path,display_info,20, FTW_PHYS | FTW_DEPTH);

    return 0;
}

char* stat_to_string_converter(const struct stat *sb){
    char* permision_string = calloc(10, sizeof(char));

    if(permision_string == NULL){
        printf("Error while creating a permission string\n");
        exit(EXIT_FAILURE);
    }

    //converting flags returned by stat->st_mode to string:

    if (S_ISDIR(sb->st_mode)){
        permision_string[0] = 'd';
    }
    else{
        permision_string[0] = '-';
    }

    if (sb->st_mode & S_IRUSR){
        permision_string[1] = 'r';
    }
    else{
        permision_string[1] = '-';
    }

    if (sb->st_mode & S_IWUSR){
        permision_string[2] = 'w';
    }
    else{
        permision_string[2] = '-';
    }

    if (sb->st_mode & S_IXUSR){
        permision_string[3] = 'x';
    }
    else{
        permision_string[3] = '-';
    }

    if (sb->st_mode & S_IRGRP){
        permision_string[4] = 'r';
    }
    else{
        permision_string[4] = '-';
    }

    if (sb->st_mode & S_IWGRP){
        permision_string[5] = 'w';
    }
    else{
        permision_string[5] = '-';
    }

    if (sb->st_mode & S_IXGRP){
        permision_string[6] = 'x';
    }
    else{
        permision_string[6] = '-';
    }

    if (sb->st_mode & S_IROTH){
        permision_string[7] = 'r';
    }
    else{
        permision_string[7] = '-';
    }

    if (sb->st_mode & S_IWOTH){
        permision_string[8] = 'w';
    }
    else{
        permision_string[8] = '-';
    }

    if (sb->st_mode & S_IXOTH){
        permision_string[9] = 'x';
    }
    else{
        permision_string[9] = '-';
    }

    return permision_string;
}

struct tm * get_time_of_file(const struct stat *sb){
    __time_t file_modification_time = sb->st_mtime;

    struct tm* some = localtime(&file_modification_time);
    some->tm_mon ++;
    some->tm_year += 1900;

    return some;
}

int compare_dates(struct tm* T){

    //1 if it is earlier, 0 if equal, -1 if later
    if(T->tm_year<sorting_time.tm_year) return 1;
    if(T->tm_year>sorting_time.tm_year) return -1;

    //year is equal
    if(T->tm_mon<sorting_time.tm_mon) return 1;
    if(T->tm_mon>sorting_time.tm_mon) return -1;

    //month and year is equal
    if(T->tm_mday<sorting_time.tm_mday) return 1;
    if(T->tm_mday>sorting_time.tm_mday) return -1;

    //so they are equal
    return 0;
}

int display_info(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf) {

    struct tm* T = get_time_of_file(sb);

    char* privileges = stat_to_string_converter(sb);

    int to_print = 0;

    switch(comparing_sign){
        case '<': //earlier than
            if(compare_dates(T)==1) to_print = 1;
            break;
        case '>':
            if(compare_dates(T)==-1) to_print = 1;
            break;
        case '=':
            if(compare_dates(T)==0) to_print = 1;
            break;
        default:
            printf("Wrong sign\n");
            exit(EXIT_FAILURE);
    }

    if(to_print==1){
        printf("%s | %d/%d/%d | %ld \t %s\n",privileges,T->tm_mday, T->tm_mon, T->tm_year, sb->st_size, fpath);

    }

    return 0;
}

void set_time_and_sign(char sign, int year, int month, int day) {
    comparing_sign = sign;

    sorting_time.tm_mday = day;
    sorting_time.tm_mon = month;
    sorting_time.tm_year = year;
}
