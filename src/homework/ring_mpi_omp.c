/*
    Inicializando-se  uma  variável  com um valor zero e a passando para outros processos que deverão abrir threads para 
    incrementar  a  variável  recebida,  assim como o próprio processo deverá incrementá-la para que, logo em sequida, a
    envie  para  o  próximo processo para que ocorrá o mesmo até o anel ser totalmente percorrido. O número de processos
    deve ser passado pelo usuário.

                                                 +--------+
                                                 |        |
                    +------+            +---->---+  x = 0 +--->---+               +------+
                    |      |            |        |        |       |               |      |
                    | x+=1 +---+        ^        +--------+       ↓          +----+ x+=1 |
                    |      |   |        |                         |          |    |      |
                    +------+   |    +---+----+                +---+----+     |    +------+
                    |      |   |    |        |                |        |     |    |      |
                    | x+=1 +--------+  x+=1  |                |  x+=1  +----------+ x+=1 |
                    |      |   |    |        |                |        |     |    |      |
                    +------+   |    +---+----+                +---+----+     |    +------+
                    |      |   |        |                         |          |    |      |
                    | x+=1 +---+        ^        +--------+       ↓          +----+ x+=1 |
                    |      |            |        |        |       |               |      |
                    +------+            +----<---+  x+=1  +---<---+               +------+
                                                 |        |
                                                 +---+----+
                                                     |
                                              +--------------+
                                              |      |       |
                                           +--+---+--+---+---+--+
                                           |      |      |      |
                                           | x+=1 | x+=1 | x+=1 |
                                           |      |      |      |
                                           +------+------+------+
*/
#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main (int argc, char **argv) {
    /*  variáveis MPI   */
    int world_size, world_rank, token;
    /*  variáveis OMP   */
    int this_thread, n_threads;
    
    /*  O  número  padrão  de  processos  no  anel  é o passado com a flag '-n' na chamada do programa como argumento, o 
        profesor pediu apenas para quatro, mas caso queira mudar isso, só alterar esse valor no próprio Makefile.
    */
    MPI_Init(&argc, &argv);
    /*  Essa chamada pega o 'id' do processo    */
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    /*  Essa chamada pega o número de processos, '-n', passado para o programa  */
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    /*  Como  o  professor limitou o número de chamadas que cada processo pode fazer ao número de processos, essa função
        garantirá  que o número de threads seja o passado pelo '-n' menos um. O menos um é porque apesar de o processo 0
        ser  criado,  ele servirá apenas para incializar os outros, que realizarão tarefas de abrir em threads, com isso
        o número the threads será igual ao número de processos que as chamam.
    */
    omp_set_num_threads(world_size-1);

    /*  Nessa primeira parte se trabalhará com o número a ser passado, idependemente do processo que o está enviando. */

    if (0 == world_rank) {
        /*  Caso seja o primeiro processo chamando, token, que é o valor a ser enviado, será inicializado com zero. */
        token = 0;
    } else {
        /*  Receba o valor enviado pelo processo anterior do anel   */
        MPI_Recv(&token, 1, MPI_INT, world_rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("\nProcess %d received token %d from process %d\n\n", world_rank, token, world_rank-1);

        /*  Abre a chamada para que as threads desse processo possam incrementar o valor recebido pelo processo anterior
            no anel
        */
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

        /*  Após as threads terem incrementado o valor, o próprio processo incrementa ele antes de enviar para o próximo
            processo  no  anel e que esse trecho do 'else' seja repetido no próximo processo até se chegar ao processo 0
            de novo.
        */
        token += 1;
    }

    /*  Envie o valor atualizado para o próximo processo do anel.   */
    MPI_Send(&token, 1, MPI_INT, (world_rank+1)%world_size, 0, MPI_COMM_WORLD);

    /*  Caso  se  encerre a percorrida no anel e volte de novo ao primeiro processo, este apresentará o valor atualizado
        do final da execução;
    */
    if (0 == world_rank) {
        MPI_Recv(&token, 1, MPI_INT, world_size-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("\nProcess %d received token %d from process %d\n\n", world_rank, token, world_size-1);
    }

    MPI_Finalize();
    
    return 0;
}
