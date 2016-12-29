/*
 * tools.h
 *
 *  Created on: Nov 6, 2016
 *      Author: kassuskley
 */

#ifndef SRC_TOOLS_H_
#define SRC_TOOLS_H_

#include <stdlib.h>
#include <stdio.h>
#include <math.h>





#define DIMENSION_PIXEL				3	// RGB pixel
#define DIMENSION_WIDTH				2
#define DIMENSION_HEIGHT			1

#define OUTPUT_FILE_NAME_SEQUENTIAL	"data/output/pictureSeqential.bmp"
#define TINT_BIAS					1.0	// TODO to change at Activity 1, step 2







/*
 * compute_julia_pixel(): compute RBG values of a pixel in a
 *                        particular Julia set image.
 *
 *  In:
 *      (x,y):            pixel coordinates
 *      (width, height):  image dimensions
 *      tint_bias:        a float to "tweak" the tint (1.0 is "no tint")
 *  Out:
 *      rgb: an already-allocated 3-byte array into which R, G, and B
 *           values are written.
 *
 *  Return:
 *      0 in success, -1 on failure
 *
 */

int compute_julia_pixel(int x, int y, int width, int height, float tint_bias, unsigned char *rgb);



/* write_bmp_header():
 *
 *   In:
 *      f: A file open for writing ('w')
 *      (width, height): image dimensions
 *
 *   Return:
 *      0 on success, -1 on failure
 *
 */

int write_bmp_header(FILE *f, int width, int height);


int maxInt(int a, int b);
int minInt(int a, int b);

#endif /* SRC_TOOLS_H_ */
