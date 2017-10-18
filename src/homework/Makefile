all:
	mpicc -fopenmp ring_mpi_omp.c -o exe
run:
	@# mpirun ./exe
	mpirun -n 4 ./exe
