#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <math.h>

#define FAILURE_EXIT(format, ...) { printf(format, ##__VA_ARGS__); exit(-1); }
#define CHECK_WITH_EXIT(value,error,message){if(value==error) FAILURE_EXIT(message);}

struct general_data{
    int** input_file;
    int** output_file;
    float** filter;
    int image_w;
    int image_h;
    int pixel_max;
    int filter_s;
    int threads;
};

int round2(double a){
    if((a - (int)a)>0.5){
        return (int)(a+1);
    }
    return (int)a;
}

int ceil2(int a){
    if(a%2==0){
        return a/2;
    }
    else{
        return a/2 +1;
    }
}

int min(double a, double b){
    return (int) (a < b ? a : b);
}

int max(double a, double b){
    return (int) (a > b ? a : b);
}

struct general_data data;

void* thread_handler(void* argv){
    int thread_no = *(int*)argv;

    int x_start = data.image_w*thread_no/data.threads;
    int x_end = data.image_w*(thread_no+1)/data.threads;

    int i,j;
    for(i=x_start;i<x_end;i++){
        for(j=0;j<data.image_h;j++){
            double s_xy = 0;

            int k,m;
            for(k=0;k<data.filter_s;k++){
                for(m=0;m<data.filter_s;m++){
                    s_xy += data.input_file[min(data.image_w-1,max(0,i-ceil2(data.filter_s)+k))]
                            [min(data.image_h-1,max(0,j-ceil2(data.filter_s)+m))]*data.filter[k][m];
                }
            }
            data.output_file[i][j]=round2(s_xy);
        }
    }

    return NULL; //because it doesnt matter
}

int main(int argc, char** argv){
    if(argc != 5){
        FAILURE_EXIT("Wrong args. Need: <threads no> <input file> <filter file> <result file>");
    }

    data.threads = atoi(argv[1]);

    char b[20];
    FILE* input = fopen(argv[2],"r");
    CHECK_WITH_EXIT(input,NULL,"Error while opening input file\n");
    /* i assume that file doesnt contain comments */
    fscanf(input,"%s %d %d %d",b,&data.image_w,&data.image_h,&data.pixel_max);


    /* load input file and allocate output*/
    int i,j;
    data.input_file = malloc(sizeof(int*)*data.image_w);
    CHECK_WITH_EXIT(data.input_file,NULL,"Error while allocating data for input array\n");
    data.output_file = malloc(sizeof(int*)*data.image_w);
    CHECK_WITH_EXIT(data.output_file,NULL,"Error while allocating data for output array\n");

    for(i=0;i<data.image_w;i++){
        data.input_file[i] = malloc(sizeof(int)*data.image_h);
        CHECK_WITH_EXIT(data.input_file[i],NULL,"Error while allocating data for input array\n");
        data.output_file[i] = malloc(sizeof(int*)*data.image_h);
        CHECK_WITH_EXIT(data.output_file[i],NULL,"Error while allocating data for output array\n");
        for(j=0;j<data.image_h;j++){
            int val;
            fscanf(input,"%d",&val);
            data.input_file[i][j] = val;
            data.output_file[i][i] = 0;
        }
    }

    fclose(input);

    /*load filter file: */

    FILE* filter = fopen(argv[3],"r");

    fscanf(filter,"%d",&data.filter_s);

    data.filter = malloc(sizeof(float*)*data.filter_s);
    CHECK_WITH_EXIT(data.filter,NULL,"Error while allocating data for output array\n");

    for(i=0;i<data.filter_s;i++){
        data.filter[i] = malloc(sizeof(float)*data.filter_s);
        CHECK_WITH_EXIT(data.filter[i],NULL,"Error while allocating data for output array\n");
        for(j=0;j<data.filter_s;j++){
            float val;
            fscanf(filter,"%f",&val);
            data.filter[i][j] = val;
        }
    }

    fclose(filter);

    pthread_t* thread = malloc(data.threads * sizeof(pthread_t));
    CHECK_WITH_EXIT(thread,NULL,"Error while allocating data for output array\n");

    struct timeval start,end;

    gettimeofday(&start,NULL);

    for(i=0;i<data.threads;i++){
        int* arg_i = malloc(sizeof(int));
        *arg_i = i;
        pthread_create(
                &thread[i], //fill the ID
                NULL, //Because we can have a default values, doesn't matter
                thread_handler,
                arg_i);
    }

    for(i=0;i<data.threads;i++){
        void* xd;
        pthread_join(thread[i],&xd); //also, we return cont null so it doesn't matter
    }

    gettimeofday(&end,NULL);

    free(thread);

    for(i=0;i<data.filter_s;i++){
        free(data.filter[i]);
    }

    for(i=0;i<data.image_w;i++){
        free(data.input_file[i]);
    }

    FILE* result = fopen(argv[4],"w");

    fprintf(result,"P2\n%d %d\n255\n",data.image_w,data.image_h);

    for(i=0;i<data.image_w;i++){
        for(j=0;j<data.image_h;j++){
            fprintf(result,"%d ",data.output_file[i][j]);
        }
        fprintf(result,"\n");
    }

    fclose(result);

    printf("Time: %fs using %d threads and %d x %d map with %d filter\n",
           (end.tv_sec-start.tv_sec)+(end.tv_usec-start.tv_usec)*pow(10,-6),
    data.threads,data.image_w,data.image_h,data.filter_s);

    return 0;
}