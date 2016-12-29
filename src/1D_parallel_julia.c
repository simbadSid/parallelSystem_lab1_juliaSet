/*
 * 1D_parallel_julia.c
 *
 *  Created on: Nov 6, 2016
 *      Author: kassuskley
 */



#include "tools.h"
#include <mpi.h>





int main(int argc, char **argv)
{
	int nbProcessor, rank;
	int n;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nbProcessor);

	if ((argc <= 1) || (n=atoi(argv[1]) <= 0) || (n%2 != 0))
	{
		printf("******** Missing even positive argument N: dimension of the Julia set ******** \n");
		return -1;
	}
	n=atoi(argv[1]);



//	MPI_Get_processor_name(hostname,&hostname_len);

	int height		= DIMENSION_HEIGHT * n;
	int sizeSlot	= height / nbProcessor;
	int indexBegin	= rank * sizeSlot;
	int extraRows	= height % nbProcessor;
	if (extraRows != 0)
	{
		indexBegin += minInt(rank, extraRows);
		if (rank < (height % nbProcessor))
			sizeSlot ++;
	}
	int indexEnd	= indexBegin + sizeSlot - 1;
	printf("[Process %d out of %d]: I should compute pixel rows %d to %d, for a total of %d rows\n", rank, nbProcessor, indexBegin, indexEnd, (indexEnd-indexBegin+1));

	MPI_Finalize();

	return 0;
}
