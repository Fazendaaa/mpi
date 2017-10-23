/*
    The  primary  difference  between  MPI_Bcast  and  MPI_Scatter  is small but
    important.  MPI_Bcast  sends  the  same piece of data to all processes while
    MPI_Scatter sends chunks of an array to different processes. 
*/
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <mpi.h>

#define ELEMENTS_PER_PROC   10
#define RAND_LIMIT          100

#define is_root(w)          (0 == w) ? 1 : 0

static float *generate_rand_array (const unsigned int);
static float compute_avg (const float *, const unsigned int);

int main (int argc, char **argv) {
    int world_rank = 0, world_size = 0;
    float *rand_nums = NULL, *sub_avgs = NULL, *sub_rand_nums = (float*) malloc(sizeof(float) * ELEMENTS_PER_PROC); 
    float sub_avg = 0, avg = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    if (is_root(world_rank)) {
        rand_nums = generate_rand_array(world_size * ELEMENTS_PER_PROC);
    }

    if (NULL != sub_rand_nums) {
        MPI_Scatter(rand_nums, ELEMENTS_PER_PROC, MPI_FLOAT, sub_rand_nums, ELEMENTS_PER_PROC, MPI_FLOAT, 0, MPI_COMM_WORLD);
    }

    sub_avg = compute_avg(sub_rand_nums, ELEMENTS_PER_PROC);

    if (is_root(world_rank)) {
        sub_avgs = (float*) malloc(sizeof(float) * world_size);
    }
    
    MPI_Gather(&sub_avg, 1, MPI_FLOAT, sub_avgs, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
    
    if (is_root(world_rank)) {
        avg = compute_avg(sub_avgs, world_size);
        printf("Average computed across all data is: %f\n", avg);
    }

    MPI_Finalize();
    
    free(rand_nums);
    free(sub_rand_nums);
    free(sub_avgs);

    return 0;
}

static float *generate_rand_array (const unsigned int length) {
    float *array = (float*) malloc(sizeof(float) * length);
    unsigned int i = 0;

    srand(time(NULL));

    if (NULL != array) {
        for (i = 0; i < length; i++)
            array[i] = rand()%RAND_LIMIT;
    }

    return array;
}

static float compute_avg (const float * array, const unsigned int length) {
    unsigned int i = 0;
    float sum = 0;

    if (NULL != array) {
        for (i = 0; i < length; i++) {
            sum += array[i];
        }
    }

    return sum/length;
}

