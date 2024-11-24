#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

int main(int argc, char *argv[]){
    pthread_t tid;
    int num_elements = atoi(argv[1]);
    int num_threads = atoi(argv[2]);
    int seed = atoi(argv[3]);
    int task = atoi(argv[4]);
    char print_results = argv[5][0];

    /*create array of random ints*/

    /*display clock time start*/

    if(task == 1){
        /*create compute max thread*/
    }else if(task == 2){
        /*create compute sum thread*/
    }

    /*join threads*/

    /*display clock time end*/

    if(strcmp(print_results[0], "Y") == 0){
        /*print results*/
    }

}

int compute_max(){

}

int compute_sum(){

}