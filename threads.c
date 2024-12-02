#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <limits.h> 

#define THREAD_CT 4

typedef struct {
    int *array;
    int start;
    int end;
    int result;
} ThreadInfo;

void *compute_max(void *arg) {
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

void *compute_sum(void *arg) {
    int i;
    ThreadInfo *info = (ThreadInfo *)arg;
    int *sum = malloc(sizeof(int));
    *sum = 0;

    for (i = info->start; i < info->end; i++) {
        *sum = (*sum + info->array[i]) % 1000000;
    }
    return sum;
}

void *compute_max(void *arg);
void *compute_sum(void *arg);

int main(int argc, char *argv[]) {
    pthread_t tids[THREAD_CT];
    int i, arr_size, final_result;
    int num_elements, num_threads, seed, task;
    char print_results;
    ThreadInfo threads[THREAD_CT];
    int *arr;
    struct timeval start, end;
    struct rusage usage;
    double wall_time;

    /*Check for valid input arguments*/
    if (argc < 6) {
        fprintf(stderr, "Usage: %s <num_elements> <num_threads> <seed> <task> <print_results>\n", argv[0]);
        exit(1);
    }

    /*Parse command-line arguments*/
    num_elements = atoi(argv[1]);
    num_threads = atoi(argv[2]);
    seed = atoi(argv[3]);
    task = atoi(argv[4]);
    print_results = argv[5][0];

    if (num_threads > THREAD_CT) {
        fprintf(stderr, "Max thread count (%d) exceeded.\n", THREAD_CT);
        exit(1);
    }

    if (num_threads > num_elements) {
        fprintf(stderr, "Number of threads cannot exceed number of elements.\n");
        exit(1);
    }

    /*Allocate memory for the array*/
    arr = (int *)malloc(num_elements * sizeof(int));
    if (!arr) {
        perror("Failed to allocate memory");
        exit(1);
    }

    /*Generate array of random integers*/
    srand(seed);
    for (i = 0; i < num_elements; i++) {
        arr[i] = rand() % 1000;
    }

    /*Start timing*/
    gettimeofday(&start, NULL);

    /*Divide work among threads*/
    arr_size = num_elements / num_threads;
    for (i = 0; i < num_threads; i++) {
        threads[i].start = arr_size * i;
        threads[i].array = arr;
        threads[i].result = 0;

        if (i == num_threads - 1) {
            threads[i].end = num_elements; /*Last thread processes the remainder*/
        } else {
            threads[i].end = arr_size * (1 + i);
        }

        if (task == 1) {
            pthread_create(&tids[i], NULL, compute_max, &threads[i]);
        } else if (task == 2) {
            pthread_create(&tids[i], NULL, compute_sum, &threads[i]);
        } else {
            fprintf(stderr, "Invalid task specified.\n");
            free(arr);
            exit(1);
        }
    }

    /*Join threads and aggregate results*/
    final_result = (task == 1) ? INT_MIN : 0; /* Initialize for max or sum*/
    for (i = 0; i < num_threads; i++) {
        void *thread_result;
        pthread_join(tids[i], &thread_result);

        if (task == 1) {
            int *thread_max = (int *)thread_result;
            if (*thread_max > final_result) {
                final_result = *thread_max;
            }
            free(thread_max); /*Free thread result*/
        } else if (task == 2) {
            int *thread_sum = (int *)thread_result;
            final_result = (final_result + *thread_sum) % 1000000;
            free(thread_sum); /*Free thread result*/
        }
    }

    /*End timing*/
    gettimeofday(&end, NULL);
    getrusage(RUSAGE_SELF, &usage);

    /*Display results if requested*/
    if (print_results == 'Y' || print_results == 'y') {
        if (task == 1) {
            printf("Max value: %d\n", final_result);
        } else if (task == 2) {
            printf("Sum value: %d\n", final_result);
        }
    }

    /*Compute and display timing information*/
    wall_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
    printf("Wall clock time: %.6f seconds\n", wall_time);
    printf("User time: %.6f seconds\n", usage.ru_utime.tv_sec + usage.ru_utime.tv_usec / 1e6);
    printf("System time: %.6f seconds\n", usage.ru_stime.tv_sec + usage.ru_stime.tv_usec / 1e6);

    /*Clean up*/
    free(arr);
    return 0;
}
