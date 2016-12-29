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
	int width		= DIMENSION_WIDTH	* n;
	int height		= DIMENSION_HEIGHT	* n;
	int nbRow		= height / nbProcessor;
	int rowBegin	= rank * nbRow;
	int extraRows	= height % nbProcessor;
	if (extraRows != 0)
	{
		rowBegin += minInt(rank, extraRows);
		if (rank < (height % nbProcessor))
			nbRow++;
	}
	int rowEnd	= rowBegin + nbRow- 1;
	printf("[Process %d out of %d]: I should compute pixel rows %d to %d, for a total of %d rows\n", rank, nbProcessor, rowBegin, rowEnd, (rowEnd-rowBegin+1));


	// Build the local picture
	unsigned char *picture		= malloc(sizeof(unsigned char) * (nbRow * width * DIMENSION_PIXEL));
	unsigned char *picturePtr	= picture;
	int x, y;

	for (y=rowBegin; y<=rowEnd; y++)											// Compute the pixel values
	{
		for (x=0; x<width; x++)
		{
			compute_julia_pixel(x, y, width, height, TINT_BIAS, picturePtr);
			picturePtr += DIMENSION_PIXEL;
		}
	}


	// Write to file
	if (rank != 0)																// Wait for the previous process to finish
	{
		int msg;
		MPI_Recv(&msg, 1, MPI_INT, rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		file = fopen(OUTPUT_FILE_NAME_PARALLEL, "a");
	}
	else
	{
		file = fopen(OUTPUT_FILE_NAME_PARALLEL, "w");
		write_bmp_header(file, width, height);									// Write the file header
	}
	picturePtr = picture;
	for (y=rowBegin; y<rowEnd; y++)												// Write the pixel into the file
	{
		for (x=0; x<width; x++)
		{
			fwrite(picturePtr, sizeof(unsigned char), DIMENSION_PIXEL, file);
			picturePtr += DIMENSION_PIXEL;
		}
	}

	if (rank != (nbProcessor-1))																// Wake-up the next process to finish
	{
		MPI_Send(&nbProcessor, 1, MPI_INT, rank+1, 0, MPI_COMM_WORLD);
	}
	fclose(file);
	free(picture);

	MPI_Finalize();

	return 0;
}
