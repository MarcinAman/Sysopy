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

struct dirent{
 ino_t d_ino;               file's serial number
 char d_name[];             name


*/

struct tm sorting_time;
char comparing_sign;
int dont_print_flag = 0; /* if we are benchmarking we dont want to print those directories */

int ls_operation(char *path, int is_sys,int dont_print) {

    dont_print_flag = dont_print;

    char* res_path = realpath(path,NULL);
    // NULL because it will automatically allocate a sufficient char array

    if(res_path==NULL){
        res_path = path; //it is already a absolute path
    }

    if(is_sys==0){
        return nftw(res_path,fn,20, FTW_PHYS | FTW_DEPTH);
    }
    else{
        return sys_nftw(res_path,fn,20);
    }
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
    some->tm_mon ++;                        /* months are indexed from 0 not 1 */
    some->tm_year += 1900;                  /* years are indexed from 1900 not 0 */

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

int fn(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
	if(S_ISLNK(sb->st_mode)) return 0;

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

    if(dont_print_flag==1){
        to_print=0;
    }
    if(to_print==1 && privileges[0]!='d'){
        printf("pid: %d %d %s | %d/%d/%d | %ld \t %s \n",(int)getpid(),(int)getppid(),privileges,T->tm_mday, T->tm_mon, T->tm_year, sb->st_size, fpath);
    }

    return 0;
}

void set_time_and_sign(char sign, int year, int month, int day) {
    comparing_sign = sign;

    sorting_time.tm_mday = day;
    sorting_time.tm_mon = month;
    sorting_time.tm_year = year;
}

int sys_nftw(char *path, int (*fn)(const char *, const struct stat *, int, struct FTW *), int fd_limit) {

    // struct ftw is not used by us but it would be nice to be able to use same printing function as before

    size_t path_length = strlen(path);

    struct stat st;

    int lstat_return = lstat(path,&st);

    if(lstat_return==-1){
        printf("Lstat returned -1 \n");
        exit(EXIT_FAILURE);
    }

    if(S_ISDIR(st.st_mode)!=0 && access(path,R_OK)>=0 && fd_limit>0 ){

        /* if it is a directory and we have a read access and have depth "fields" to open it */

        pid_t fork_result = fork();         /*have no idea why this code doesnt produce a new process and kill parent one */
        if(fork_result<0){
          printf("%s\n","Fork failure" );
          exit(EXIT_FAILURE);
        }

        if(fork_result!=0) return 0;

        DIR* some_dir;
        if((some_dir = opendir(path))!=NULL){
            struct dirent *structdirent;

            while((structdirent = readdir(some_dir))!=NULL){
                if(strcmp(structdirent->d_name,"..")!=0 && strcmp(structdirent->d_name,".")!=0){

                    path[path_length] = '/';

                    strcpy(path+path_length+1,structdirent->d_name); //append structdirent->d_name to path

                    /* since we are returning a const 0 from our display function
                     * it doesnt matter that this return value goes straight to dev/null
                     */
                    sys_nftw(path,fn,fd_limit-1);
                }
            }
            closedir(some_dir);
        }
    }

    struct FTW ftw;
    path[path_length]=0;

    return fn(path,&st,0,&ftw);
}
