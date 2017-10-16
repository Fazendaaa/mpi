#include <stdio.h>
#include <mpi.h>

int main(int argc, char **argv) {
    int world_size, world_rank, name_len;
    char processor_name[MPI_MAX_PROCESSOR_NAME];

    MPI_Init(&argc, &argv);
    // Encloses all of the processes in the job -- this should return the amount of processes that were requested.
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    // Returns the ranks of a process in a communicator.
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Get_processor_name(processor_name, &name_len);

    printf("Hello world from processor %s, rank %d out of %d processors\n", processor_name, world_rank, world_size);

    MPI_Finalize();

    return 0;
}
