#ifndef	IMAGE_IO_TGA_H
#define	IMAGE_IO_TGA_H

#include "RasterImage.h"

struct RasterImage *readTGA(const char* fileName);
int writeTGA(char* fileName, unsigned char** theData, unsigned int nbRows, unsigned int nbCols, unsigned int color);

#endif	//	IMAGE_IO_TGA_H
