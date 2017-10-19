#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define location(w) ((w)->location)
#define steps(w)    ((w)->num_steps_left_in_walk)

typedef struct {
    int location;
    int num_steps_left_in_walk;
} Walker;

static void decompose_domain (const int, const int, const int, int *, int *);
static Walker * initialize_walkers (const int, const int, const int, const int);
static Walker * walk (Walker *, const int, const int, const int);
static void send_outgoing_walkers (Walker *, const int, const int, const int);
static void receive_incoming_walkers (Walker *, const int, const int);

int main (int argc, char **argv) {
    int domain_size = 25, max_walk_size = 5, num_walkers_per_proc = 5, subdomain_start, subdomain_size, maximum_sends_recvs, i, j;
    int world_size, world_rank;
    Walker *incoming_walkers = NULL, *outgoing_walkers = NULL;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    decompose_domain(domain_size, world_rank, world_size, &subdomain_start, &subdomain_size);
    incoming_walkers = initialize_walkers(num_walkers_per_proc, max_walk_size, subdomain_start, subdomain_size);

    if (NULL != incoming_walkers) {
        maximum_sends_recvs = max_walk_size/(domain_size/world_size)+1;

        for (i = 0; i < maximum_sends_recvs; i++) {
            for (j = 0; j < num_walkers_per_proc; j++) {
                walk(&incoming_walkers, subdomain_start, subdomain_size, domain_size, &outgoing_walkers);
            }
        }
    }

    MPI_Finalize();

    return 0;
}

static void decompose_domain (const int domain_size, const int world_rank, const int world_size, int * subdomain_start, int * subdomain_size) {
    if (world_size > domain_size) {
        MPI_Abort(MPI_COMM_WORLD, 1);
    } else {
        *subdomain_start = domain_size/world_size*world_rank;
        *subdomain_size = domain_size/world_size;
        
        if (world_size - 1 == world_rank) {
            *subdomain_size += domain_size % world_size;
        }
    }
    
}

static Walker * initialize_walkers (const int num_walkers_per_proc, const int max_walk_size, const int subdomain_start, const int subdomain_size) {
    int i;
    Walker *incoming_walkers = malloc(sizeof(Walker) * num_walkers_per_proc);
    
    if (NULL != incoming_walkers) {
        for (i = 0; i < num_walkers_per_proc; i++) {
            location(incoming_walkers[i]) = subdomain_start;
            steps(incoming_walkers[i]) = rand()%5;
        }
    }

    return incoming_walkers;
}

static Walker * walk (Walker * walker, const int subdomain_start, const int subdomain_size, const int domain_size) {
    Walker *outgoing_walkers = NULL;
    
    while (0 < steps(walker)) {
        if (subdomain_start + subdomain_size == location(walker)) {
            if (domain_size == location(walker)) {
                location(walker) = 0;
            }

            outgoing_walkers;
            break;
        } else {
            steps(walker)--;
            location(walker)++;
        }
    }

    return outgoing_walkers;
}

static void send_outgoing_walkers (Walker * outgoing_walkers, const int size, const int world_rank, const int world_size) {
    MPI_Send(outgoing_walkers, size*sizeof(Walker), MPI_BYTE, (world_rank+1)%world_size, 0, MPI_COMM_WORLD);
    free(outgoing_walkers);
}

static void receive_incoming_walkers (Walker * incoming_walkers, const int world_rank, const int world_size) {
    int incoming_rank = (0 == world_rank) ? world_size - 1 : world_rank - 1;
    int incoming_walkers_size;
    MPI_Status status;

    MPI_Probe(incoming_rank, 0, MPI_COMM_WORLD, &status);
    MPI_Get_count(&status, MPI_BYTE, &incoming_walkers_size);

    incoming_walkers = malloc(sizeof(Walker)*incoming_walkers_size);

    if (NULL != incoming_walkers) {
        MPI_Recv(incoming_walkers, incoming_walkers_size, MPI_BYTE, incoming_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
}
