#include <stdio.h>
#include <mpi.h>
#include <omp.h>

int main (int argc, char **argv) {
    /*  variáveis MPI   */
    int world_size, world_rank, token;
    /*  variáveis OMP   */
    int this_thread, n_threads, max_threads = 3;

    /*  O  número  padrão  de  processos  no  anel  é o passado com a flag '-n' na chamada do programa como argumento, o 
        profesor pediu apenas para quatro, mas caso queira mudar isso, só alterar esse valor no próprio Makefile.
    */
    MPI_Init(&argc, &argv);
    /*  Essa chamada que pega o 'id' do processo    */
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    /*  Como  o professor limitou o número de chamadas que cada processo pode fazer com apenas três threads, essa função
        garantirá isso.
    */
    omp_set_num_threads(max_threads);

    /*  Caso seja o primeiro processo chamando, envia zero para os outros processos do anel.    */
    if(0 == world_rank) {
        token = 0;
    } else {
        /*  Receba o valor enviado pelo processo anterior do anel   */
        MPI_Recv(&token, 1, MPI_INT, world_rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Process %d received token %d from process %d\n", world_rank, token, world_rank-1);

        #pragma omp parallel default(shared) private(this_thread, n_threads)
        {
            this_thread = omp_get_thread_num();
            n_threads = omp_get_num_threads();

            /*  Possibilite que cada thread da chamada acrescente ao valor passado, deve ser crítico porque senão haverá
                inconsistências no valor repassado.
            */
            #pragma omp critical
            token += 1;

            printf("[Process %d] Thread #%d out of %d\tToken = %d\n", world_rank, this_thread, n_threads, token);
        }

        token += 1;
    }

    /*  Envie o valor atualizado para o próximo processo do anel.   */
    MPI_Send(&token, 1, MPI_INT, (world_rank+1)%world_size, 0, MPI_COMM_WORLD);

    /*  Caso  se  encerre a percorrida no anel e volte de novo ao primeiro processo, este apresentará o valor atualizado
        do final da execução;
    */
    if(0 == world_rank) {
        MPI_Recv(&token, 1, MPI_INT, world_size-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Process %d received token %d from process %d\n", world_rank, token, world_size-1);
    }

    MPI_Finalize();
    
    return 0;
}
