#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define THREAD_CT 4

typedef struct {
     int *array;
    int start;
    int end;
    int result;
} ThreadInfo;

void *compute_max(void *arg){
    ThreadInfo *threadInfo = (ThreadInfo *)arg;
    int i;
    int *max = malloc(sizeof(int));
    *max = threadInfo->array[threadInfo->start];

    for (i = threadInfo->start + 1; i < threadInfo->end; i++) {
        if (threadInfo->array[i] > *max) {
            *max = threadInfo->array[i];
        }
    }
    return max;
}

void *compute_sum(void *arg){
    int i;
    ThreadInfo *info = (ThreadInfo *)arg;
    int *sum = malloc(sizeof(int));;
    
    
    for(i = info->start; i < info->end; i++){
        *sum = (*sum + info->array[i]) % 1000000;
    }

    info->result = *sum;
    return sum;
}


int main(int argc, char *argv[]){
    pthread_t tids[THREAD_CT + 1];
    int i, arr_size, final_result;
    int num_elements = atoi(argv[1]);
    int num_threads = atoi(argv[2]);
    ThreadInfo threads[THREAD_CT + 1];
    int seed = atoi(argv[3]);
    int task = atoi(argv[4]);
    char print_results = argv[5][0];
    int *arr = (int *)malloc(num_elements * sizeof(int));

    if(argc < 6){
        exit(1);
    }
    
    /*create array of random ints*/
    if (num_threads > THREAD_CT) {
        fprintf(stderr, "Max thread count exceeded");
        exit(1);
    }

    srand(seed);
    
    for (i = 0; i < num_elements; i++) {
        arr[i] = rand() % 1000;
    }

    /*display clock time start*/
    arr_size = num_elements / num_threads;

    for(i = 0; i < num_threads; i++){

        threads[i].start = arr_size * i;
        threads[i].array = arr;
        threads[i].result = 0;

        if(i == num_threads - 1){
            threads[i].end = num_elements;
        } else {
            threads[i].end = (arr_size) * (1 + i);
        }

        if(task == 1){
            pthread_create(&tids[i], NULL, compute_max, &threads[i]);
        
        }else if(task == 2){
            pthread_create(&tids[i], NULL, compute_sum, &threads[i]);
        } else {
            exit(1); /*invalid task*/
        }
    }

   

    /*join threads*/
    for(i = 0; i < num_threads; i++){
        pthread_join(tids[i], NULL);

        if(task == 1){
            if(threads[i].result > 0){
                final_result = threads[i].result;
            }
        } else if (task == 2){
            final_result = (threads[i].result - 1) % 1000000;
        }
    }

    /*display clock time end*/

    if(print_results == 'Y'){
        /*print results*/
    }

    return 0;
}

