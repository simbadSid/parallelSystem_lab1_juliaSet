/*
 * sequential_julia.c
 *
 *  Created on: Nov 6, 2016
 *      Author: kassuskley
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "tools.h"








int main(int argc, char **argv)
{
	int n;

	if ((argc <= 1) || (n=atoi(argv[1]) <= 0) || (n%2 != 0))
	{
		printf("******** Missing even positive argument N: dimension of the Julia set ******** \n");
		return -1;
	}
	n=atoi(argv[1]);
	FILE *file = fopen(OUTPUT_FILE_NAME_SEQUENTIAL, "w");
	if (file == NULL)
	{
		printf("******** Can't open the file \"%s\" ********\n", OUTPUT_FILE_NAME_SEQUENTIAL);
		return -1;
	}

	int width	= DIMENSION_WIDTH	* n;
	int height	= DIMENSION_HEIGHT	* n;
	unsigned char *picture		= malloc(sizeof(unsigned char) * (width * height * DIMENSION_PIXEL));
	unsigned char *picturePtr	= picture;
	int x, y;


	for (x=0; x<width; x++)														// Compute the pixel values
	{
		for (y=0; y<height; y++)
		{
			compute_julia_pixel(x, y, width, height, TINT_BIAS, picturePtr);
			picturePtr += DIMENSION_PIXEL;
		}
	}

	write_bmp_header(file, width, height);										// Write the file header
	picturePtr = picture;
	for (x=0; x<width; x++)														// Write the pixel into the file
	{
		for (y=0; y<height; y++)
		{
			fwrite(picturePtr, sizeof(unsigned char), DIMENSION_PIXEL, file);
			picturePtr += DIMENSION_PIXEL;
		}
	}

	fclose(file);
	return 0;
}
