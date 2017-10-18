/*
    Run it with:
        mpirun -n 3 ./exe
    Because if your PC have less than three processors it won't work.
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

int main (int argc, char **argv) {
    int world_size, world_rank;
    const int max_numbers = 100;
    int *numbers = (int *)malloc(sizeof(int)*max_numbers), *buffer = NULL;
    int number_amount;
    MPI_Status status;

    if (NULL != numbers) {
        MPI_Init(&argc, &argv);
        MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
        MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    
        if (0 == world_rank) {
            srand(time(NULL));
            number_amount = rand()%max_numbers;

            MPI_Send(numbers, number_amount, MPI_INT, 1, 0, MPI_COMM_WORLD);
            MPI_Send(numbers, number_amount, MPI_INT, 2, 0, MPI_COMM_WORLD);
            printf("0  send %d numbers to 1 and to 2.\n", number_amount);
        } else if (1 == world_rank) {
            MPI_Recv(numbers, max_numbers, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
            MPI_Get_count(&status, MPI_INT, &number_amount);

            printf("1 recived %d numbers from 0.\nMessage source = %d, tag =%d\n", number_amount, status.MPI_SOURCE, status.MPI_TAG);
        } else if (2 == world_rank) {
            MPI_Probe(0, 0, MPI_COMM_WORLD, &status);
            MPI_Get_count(&status, MPI_INT, &number_amount);

            buffer = (int *)malloc(sizeof(int)*number_amount);

            if (NULL != buffer) {
                MPI_Recv(buffer, number_amount, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                printf("2 dynamically received %d numbers from 0.\n", number_amount);

                free(buffer);
            }
        }

        MPI_Finalize();
        free(numbers);
    }
    
    return 0;
}
