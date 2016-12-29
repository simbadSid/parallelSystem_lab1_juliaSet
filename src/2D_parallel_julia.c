/*
 * 1D_parallel_julia.c
 *
 *  Created on: Nov 6, 2016
 *      Author: kassuskley
 */



#include "tools.h"
#include <math.h>
#include <mpi.h>





int main(int argc, char **argv)
{
	int nbProcessor, rank;
	int n;
	FILE *file;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nbProcessor);

	if ((argc <= 1) || (n=atoi(argv[1]) <= 0) || (n%2 != 0))
	{
		printf("******** Missing even positive argument N: dimension of the Julia set ******** \n");
		return -1;
	}
	n=atoi(argv[1]);


	// Compute the index of the local picture among the global picture
	int pSqrt	= sqrt(nbProcessor);
	int width	= DIMENSION_WIDTH	* n;
	int height	= DIMENSION_HEIGHT	* n;
	int i		= rank % pSqrt;
	int j		= rank / pSqrt;
	int tilleWidth	= width	/ pSqrt;
	int tilleHeight	= height/ pSqrt;
	int xMin	= i * width	/ pSqrt;
	int yMin	= j * height/ pSqrt;
	printf("[Process rank %d out of %d]: my 2-D rank is (%d, %d), my tile is [%d:%d, %d:%d]\n", rank, nbProcessor, i, j, xMin, xMin+tilleWidth-1, yMin, yMin+tilleHeight-1);


	// Build the local picture
	unsigned char *picture		= malloc(sizeof(unsigned char) * (tilleWidth * tilleHeight * DIMENSION_PIXEL));
	unsigned char *picturePtr	= picture;
	int x, y;

	for (y=yMin; y<yMin+tilleHeight; y++)											// Compute the pixel values
	{
		for (x=xMin; x<xMin+tilleWidth; x++)
		{
			compute_julia_pixel(x, y, width, height, TINT_BIAS, picturePtr);
			picturePtr += DIMENSION_PIXEL;
		}
	}


	// Write to file
	if (rank == 0)																// Write the file header
	{
		file = fopen(OUTPUT_FILE_NAME_PARALLEL, "w");
		write_bmp_header(file, width, height);									// Write the file header
		fclose(file);															// This close allows to do not test for the rank at each iteration
	}

	picturePtr = picture;
	for (y=yMin; y<yMin+tilleHeight; y++)
	{
		if ((i != 0) || (j != 0))												// Wait for the previous process to finish:
		{
			int msg, previousProcessRank;
			if ((y == yMin) && (i == 0))										//		Previous process is on the previous raw
				previousProcessRank = rank - 1;
			else if (i == 0)													//		Previous process is on the same raw (other side)
				previousProcessRank = rank + pSqrt - 1;
			else																//		Previous process is on the same raw
				previousProcessRank = rank - 1;
			MPI_Recv(&msg, 1, MPI_INT, previousProcessRank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
		file = fopen(OUTPUT_FILE_NAME_PARALLEL, "a");
		for (x=xMin; x<xMin+tilleWidth; x++)
		{
			fwrite(picturePtr, sizeof(unsigned char), DIMENSION_PIXEL, file);
			picturePtr += DIMENSION_PIXEL;
		}
		if ((i != pSqrt-1) || (j != pSqrt-1) ||									// Wake up the next process
			(x != xMin+tilleWidth-1) || (y != yMin+tilleHeight-1))
		{
			int nextProcessRank;
			if ((y == yMin+tilleHeight-1) && (i == pSqrt-1))					//		Next process on the next raw
				nextProcessRank = rank + 1;
			else if (i == pSqrt-1)												//		Next process on the same raw  (other side)
				nextProcessRank = rank - pSqrt + 1;
			else																//		Next process on the same raw
				nextProcessRank = rank + 1;
			MPI_Send(&nbProcessor, 1, MPI_INT, nextProcessRank, 0, MPI_COMM_WORLD);
		}
		fclose(file);
	}

	free(picture);

	MPI_Finalize();

	return 0;
}
