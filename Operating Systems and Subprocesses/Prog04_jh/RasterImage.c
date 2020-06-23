#include <stdlib.h>        
#include <stdio.h>
#include "RasterImage.h"



typedef enum ImageFileType
{
		kUnknownType = -1,
		kTGA_COLOR,		//24-bit color image
		kTGA_GRAY,
		kPPM,			//24-bit color image
		kPGM			//8-bit gray-level image
} ImageFileType;

/**	This is the enum type that refers to images loaded in memory, whether
 *	they were read from a file, played from a movie, captured from a live
 *	video stream, or the result of calculations.
 *	Feel free to edit and add types you need for your project.
 */
typedef enum ImageType
{
		/**	Color image with 4 bytes per pixel
		 */
		RGBA32_RASTER,

		/**	Gray image with 1 byte per pixel
		 */
		GRAY_RASTER,

		/**	Monochrome image (either gray or one color channel of a 
		 *	color image)
		 *	stored in a float raster
		 */
		FLOAT_RASTER
			
} ImageType;


/**	
 *	This is the data type that holds the raster data for an image,
 *	this includes the dimensions and image type, pixel data, 
 *	a 2d framework for the pixel data, and the number of bytes
 *	per pixel and row.
 */
struct RasterImage {
	unsigned int bpPixel;/**< Bytes per pixel in the image. */
	unsigned int bpRow;/**< Bytes per row in the image. */
	unsigned int numRows;/**< Number of rows in the image. */
	unsigned int numCols;/**< Number of columns in the image. */
	ImageType *imgType;/**< The type of image: RGBA, GRAY, or FLOAT. */
	unsigned char *raster2d;/**< THe two dimensional raster structure. */
	unsigned char *raster;/**< The image data raster. */
};


