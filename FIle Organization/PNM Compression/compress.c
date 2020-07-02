#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "assert.h"
#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "compress.h"
#include <pnm.h>
#include "arith411.h"
#include "bitpack.h"

A2Methods_T methods;
uint32_t batch_avg_index( float**  pixel);
unsigned index_of_scalar(   float scalar);
   float scalar_of_index(unsigned  index);

void compress(FILE* fp) {
  //assert file and methods
  assert(fp);
  methods = array2_methods_plain; // default to UArray2 methods
  assert(methods);

  //read image
  Pnm_ppm image = Pnm_ppmread(fp, methods);

  //create all local variables
  Pnm_rgb* block = calloc(4, sizeof(Pnm_rgb));
  float** pixel = calloc(4, sizeof(float*));
  pixel[0] = calloc(3, sizeof(float));
  pixel[1] = calloc(3, sizeof(float));
  pixel[2] = calloc(3, sizeof(float));
  pixel[3] = calloc(3, sizeof(float));
  unsigned width = image->width;
  unsigned height = image->height;
  unsigned denominator = image->denominator;

  float cosine[4] = { 0 };
  float luma[4] = { 0 };
  uint32_t word = 0;
  char* byte = 0;

  //trim and print header
  if ( width%2)  width--;
  if (height%2) height--;
  printf("Compressed image format 2\n%u %u\n", width, height);
   
  //compress and print
  for (unsigned evenRow = 0; evenRow < height; evenRow += 2) {
    for (unsigned evenCol = 0; evenCol < width; evenCol += 2) {
      //get the pixels as floats
      block[0] = methods->at(image->pixels, evenCol  , evenRow  );
      block[1] = methods->at(image->pixels, evenCol  , evenRow+1);
      block[2] = methods->at(image->pixels, evenCol+1, evenRow  );
      block[3] = methods->at(image->pixels, evenCol+1, evenRow+1);

      //get the floats
      for (int pNum=0; pNum<4; pNum++) {
	pixel[pNum][0] = ((float)(block[pNum]->red  ))/denominator;
	pixel[pNum][1] = ((float)(block[pNum]->green))/denominator;
	pixel[pNum][2] = ((float)(block[pNum]->blue ))/denominator;	
      }

      //get word with pb and pr
      word = batch_avg_index(pixel);

      //get luma values
      luma[0] = 0.299*pixel[0][0] + 0.587*pixel[0][1] + 0.114*pixel[0][2];
      luma[1] = 0.299*pixel[1][0] + 0.587*pixel[1][1] + 0.114*pixel[1][2];
      luma[2] = 0.299*pixel[2][0] + 0.587*pixel[2][1] + 0.114*pixel[2][2];
      luma[3] = 0.299*pixel[3][0] + 0.587*pixel[3][1] + 0.114*pixel[3][2];

      //cosine transform
      cosine[0] = (luma[3] + luma[2] + luma[1] + luma[0])/4;
      cosine[1] = (luma[3] + luma[2] - luma[1] - luma[0])/4;
      cosine[2] = (luma[3] - luma[2] + luma[1] - luma[0])/4;
      cosine[3] = (luma[3] - luma[2] - luma[1] + luma[0])/4;

      //store
      word = Bitpack_newu(word, 9, 23, (unsigned)round(cosine[0]*511));
      word = Bitpack_news(word, 5, 18, index_of_scalar(cosine[1]));
      word = Bitpack_news(word, 5, 13, index_of_scalar(cosine[2]));
      word = Bitpack_news(word, 5,  8, index_of_scalar(cosine[3]));

      //print in big endian
      byte = (char*)&word;
      putchar(byte[3]);
      putchar(byte[2]);
      putchar(byte[1]);
      putchar(byte[0]);
    }
  }
  
  for (int i=0; i<4; i++) free(pixel[i]);
  free(pixel);
  free(block);
  Pnm_ppmfree(&image);
}






void decompress(FILE* fp) {
  assert(fp);
  methods = array2_methods_plain; // default to UArray2 methods
  assert(methods);
  unsigned width = 0;
  unsigned height = 0;
  int denominator = 15;
  int read = fscanf(fp,"Compressed image format 2\n%u %u\n",&width,&height);
  assert(read == 2);
  
  Pnm_rgb* block = calloc(4, sizeof(Pnm_rgb));
  float** pixel = calloc(4, sizeof(float*));
  pixel[0] = calloc(3, sizeof(float));
  pixel[1] = calloc(3, sizeof(float));
  pixel[2] = calloc(3, sizeof(float));
  pixel[3] = calloc(3, sizeof(float));
  float luma[4] = { 0 };
  float sums[3] = { 0 };
  float cosine[4] = { 0 };
  uint32_t word = 0;
  float pb = 0;
  float pr = 0;
  A2Methods_Array2 pixmap = methods->new(width,height,sizeof(struct Pnm_rgb));
  struct Pnm_ppm image = { .width = width, .height = height,
			   .denominator = denominator, .pixels = pixmap,
			   .methods = methods };

  for (unsigned evenRow = 0; evenRow < height; evenRow += 2) {
    for (unsigned evenCol = 0; evenCol < width; evenCol += 2) {

      //get the block of bits
      block[0] = methods->at(pixmap, evenCol, evenRow);
      block[1] = methods->at(pixmap, evenCol, evenRow+1);
      block[2] = methods->at(pixmap, evenCol+1, evenRow);
      block[3] = methods->at(pixmap, evenCol+1, evenRow+1);

      //get the word
      word = 0;
      word |= (getc(fp) << 24);
      word |= (getc(fp) << 16);
      word |= (getc(fp) << 8);
      word |= getc(fp);

      //extract
      cosine[0] = (float)Bitpack_getu(word, 9, 23)/511;
      cosine[1] = scalar_of_index(Bitpack_gets(word, 5, 18));
      cosine[2] = scalar_of_index(Bitpack_gets(word, 5, 13));
      cosine[3] = scalar_of_index(Bitpack_gets(word, 5,  8));

      //reverse dicrete cosign transform
      luma[0] = cosine[0] - cosine[1] - cosine[2] + cosine[3];
      luma[1] = cosine[0] - cosine[1] + cosine[2] - cosine[3];
      luma[2] = cosine[0] + cosine[1] - cosine[2] - cosine[3];
      luma[3] = cosine[0] + cosine[1] + cosine[2] + cosine[3];
      
      pb = Arith_chroma_of_index(Bitpack_getu(word, 4, 4));
      pr = Arith_chroma_of_index(Bitpack_getu(word, 4, 0));
      sums[0] = 1.402*pr;
      sums[1] = -(0.344136*pb + 0.714136*pr);
      sums[2] = 1.772*pb;

      //compare pixels
      for (int pNum=0; pNum<4; pNum++) { 	
 	pixel[pNum][0] = round(denominator*(luma[pNum] + sums[0]));
	pixel[pNum][1] = round(denominator*(luma[pNum] + sums[1]));
	pixel[pNum][2] = round(denominator*(luma[pNum] + sums[2]));
	if (pixel[pNum][0] > 255) pixel[pNum][0] = 255;
	if (pixel[pNum][1] > 255) pixel[pNum][1] = 255;
	if (pixel[pNum][2] > 255) pixel[pNum][2] = 255;
	//printf("red   %f = %d\n", pixel[pNum][0], block[pNum]->red);
	//printf("green %f = %d\n", pixel[pNum][1], block[pNum]->green);
	//printf("blue  %f = %d\n", pixel[pNum][2], block[pNum]->blue);
	block[pNum]->red   = (int)(pixel[pNum][0]);
	block[pNum]->green = (int)(pixel[pNum][1]);
	block[pNum]->blue  = (int)(pixel[pNum][2]);
      }
    }
  }

  Pnm_ppmwrite(stdout, &image);
  for (int i=0; i<4; i++) free(pixel[i]);
  free(pixel);
  free(block);
  methods->free(&pixmap);;
}

uint32_t batch_avg_index(float** pixel) {
  //batch avg
  float rsum = pixel[0][0] + pixel[1][0] + pixel[2][0] + pixel[3][0];
  float gsum = pixel[0][1] + pixel[1][1] + pixel[2][1] + pixel[3][1];
  float bsum = pixel[0][2] + pixel[1][2] + pixel[2][2] + pixel[3][2];
  float avg  = (-0.168736*rsum + -0.331264*gsum + 0.5*bsum)/4;

  //word
  uint32_t word = ((uint32_t)Arith_index_of_chroma(avg) << 4);
  assert(Arith_index_of_chroma(avg) == (unsigned)Bitpack_getu(word, 4, 4));
  avg = (0.5*rsum + -0.418688*gsum + -0.081312*bsum)/4;
  word |= ((uint32_t)Arith_index_of_chroma(avg));
  assert(Arith_index_of_chroma(avg) == (unsigned)Bitpack_getu(word, 4, 0));
  return word;
}


unsigned index_of_scalar(float scalar) {
  return (int)round(50*scalar);
}

float scalar_of_index(unsigned index) {
  if ((index >> 4)) return ((float)( (int)(index |= 0xFFFFFFE0) )) /50; 
  return (float)index/50;
}
