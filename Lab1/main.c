#include "PointerArray.h"
#include <sys/times.h>
#include <time.h>
#include <sys/resource.h>

char* random_string_generator(size_t size){
    char* charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    if(size < sizeof(char)){
        return NULL;
    }
    char* new_string = malloc(size);
    int char_number = size/sizeof(char);
    for(int i=0;i<char_number;i++){
        new_string[i]=charset[rand()%(char_number)];
    }
    return new_string;
}

char** create_sample_array(int size,size_t base_size){
    char** sample_array = malloc(sizeof(char*)*size);
    for(int i=0;i<size;i++){
        sample_array[i] = random_string_generator(base_size);
    }

    return sample_array;
}

void measure_cpu_time(char** sample_array, int size){
    pointer_array* array = createArray(2);
    clock_t start = clock();
    for(int i=0;i<size;i++){
        array = addToArray(sample_array[i],array);
    }
    clock_t mid = clock();
    for (int j = 0; j < size; ++j) {
        array = removeCharArray(sample_array[j],array);
    }
    clock_t end = clock();

    double add_time = ((double)(mid-start))/CLOCKS_PER_SEC;
    double full_time = ((double)(end-start))/CLOCKS_PER_SEC;
    printf("Adding %d elements took %f s and removing %f of CPU time\n",size,add_time,full_time);
}


//https://linux.die.net/man/2/getrusage
//http://www.gnu.org/software/libc/manual/html_node/Elapsed-Time.html
//void measure_time(char** sample_array, int size){
//    pointer_array* array = createArray(2);
//
//
//    struct timeval user;
//    struct timeval system;
//    struct rusage* usage = malloc(sizeof(struct timespec));
//    clock_t start = clock();
//    getrusage(RUSAGE_SELF,usage);
//    user = usage->ru_utime;
//    system = usage->ru_stime;
//
//
//    for(int i=0;i<size;i++){
//        array = addToArray(sample_array[i],array);
//    }
//
//}

int main() {
    int times = 100000;
    char **sample_array = create_sample_array(times,sizeof(char)*20);
    measure_cpu_time(sample_array,times);
    return 0;
}
