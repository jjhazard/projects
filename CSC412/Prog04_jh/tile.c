

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "image_IO_TGA.h"



int main(int argc, char *argv[]) {

  	int tileNum = argc-2; 
  	struct RasterImage* tile = readTGA(argv[1]);
  	int columns = tile->numCols;
  	int rows = tile->numRows;
   	int squareR = (int)ceil(sqrt(tileNum));
   	int squareC = (int)ceil((float)tileNum/squareR);
   	struct RasterImage* full = (RasterImage*)malloc(sizeof(RasterImage));
	full->numRows = squareR*rows;
	full->numCols = squareC*tile->numCols;
	full->bpPixel = tile->bpPixel;
	full->bpRow =full->bpPixel * full->numCols;
	full->imgType = tile->imgType;
  	free(tile->raster2d);
	free(tile->raster);
	free(tile);
	full->raster = (unsigned char*)malloc((full->numRows*full->bpRow+1)*sizeof(unsigned char));
	full->raster2d = (unsigned char**)malloc((full->numRows+1)*sizeof(unsigned char*));
	for (int i=0; i<full->numRows; i++) {
		full->raster2d[i] = &full->raster[i*full->bpRow];
	}
	for(int i = 0; i < tileNum; i++){
		struct RasterImage* tile = readTGA(argv[i + 1]);
     		int split[2] = { i/squareC, i%squareC };
     		for(int a = 0, c = split[0]*rows; a < rows; a++, c++){
			
       			for(int b = 0, d = split[1]*columns; b < columns; b++, d++){
          			for (int k = 0; k<4; k++) {
						//full->raster2d[a][b*4+k] = tile->raster2d[c][d*4+k];
				}
         		}
       		}
		free(tile->raster2d);
		free(tile->raster);
		free(tile);
     	}
	char newPath[128];
	strcpy(newPath,argv[argc-1]);
	strcat(newPath,"tiledimage.tga");
	if (writeTGA(newPath, full->raster2d, full->numRows, full->numCols, 3) != 0) {
		printf("failed to write split image to %s\n", newPath);
	}
	free(full->raster2d);
	free(full->raster);
	free(full);
}
