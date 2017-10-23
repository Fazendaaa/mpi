/*
    NUSP:
        9293286
        9293522
        9293501
        9293251
        9293053
        8530952
        8598732
*/

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char **argv) {
    int world_size, world_rank;
    int vector_send[10] = {0, 1, 1, 1, 4, 5, 6, 7, 8, 9};
    int recv, vector_size = 10, i, wanted = 1, count = 0, final = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    MPI_Scatter(vector_send, 1, MPI_INT, &recv, 1, MPI_INT, world_rank, MPI_COMM_WORLD);        

    if (wanted == recv)
        count++;

    MPI_Reduce(&count, &final, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (0 == world_rank) {
        for (i=0; i < vector_size; i++)
            printf("%d ", vector_send[i]);
        printf("\n\nFinal: %d\n", final);
    }

    MPI_Finalize();

    return 0;
}
