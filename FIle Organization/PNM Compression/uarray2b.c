#include <array.h>
#include <uarray2b.h>
#include "mem.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "assert.h"

struct UArray2b_T {
  int blocksize;
  int blockNum;
  int width;
  int height;
  int size;
  int blockHeight;
  int blockWidth;
  int edgeHeight;
  int edgeWidth;
  Array_T blocks;
};

UArray2b_T UArray2b_new(int width, int height, int size, int blocksize) {
  //assert dimensions are correct
  assert(blocksize > 0 && size > 0);
  assert(width >= 0 && height >= 0);

  //create array2b
  UArray2b_T array2b;
  NEW(array2b);
  array2b->width = width;
  array2b->height = height;
  array2b->size = size;
  array2b->blocksize = blocksize;
  array2b->blockWidth = width/blocksize;
  array2b->blockHeight = height/blocksize;
  array2b->edgeWidth = width - blocksize*array2b->blockWidth;
  array2b->edgeHeight = height - blocksize*array2b->blockHeight;
  array2b->blockNum = (array2b->blockWidth+1)*(array2b->blockHeight+1);

  //create subarrays
  if ((height == 0) || (width == 0)) array2b->blocks = NULL;
  else {
    array2b->blocks = Array_new(array2b->blockNum, sizeof(Array_T));
    for (int blockIndex; blockIndex<array2b->blockNum; blockIndex++) {
      Array_T block = Array_new(blocksize*blocksize, size);
      (void)Array_put(array2b->blocks, blockIndex, &block);
    }
  }
  
  //return array2b
  return array2b;
}

/* new blocked 2d array: blocksize = square root of # of cells in block */
UArray2b_T UArray2b_new_64K_block(int width, int height, int size) {
  //assert dimensions are correct
  assert(size > 0);
  assert(width >= 0 && height >= 0);

  //create array2b  
  UArray2b_T array2b;
  int blocksize = (int)sqrt(64000/size) + 1;
  NEW(array2b);
  array2b->blocksize = blocksize;
  array2b->width = width;
  array2b->height = height;
  array2b->size = size;
  array2b->blockWidth = width/blocksize;
  array2b->blockHeight = height/blocksize;
  array2b->edgeWidth = width - blocksize*array2b->blockWidth;
  array2b->edgeHeight = height - blocksize*array2b->blockHeight;
  array2b->blockNum = (array2b->blockWidth+1)*(array2b->blockHeight+1);

  
  //create subarrays
  if ((height < 1) || (width < 1)) array2b->blocks = NULL;
  else {
    array2b->blocks = Array_new(array2b->blockNum, sizeof(Array_T));
    for (int blockNum; blockNum<array2b->blockNum; blockNum++) {
      Array_T block = Array_new(blocksize*blocksize, size);
      (void)Array_put(array2b->blocks, blockNum, &block);
    }
  }

  //return array2b
  return array2b;
}

/* new blocked 2d array: blocksize as large as possible provided
   block occupies at most 64KB (if possible) */
void UArray2b_free(UArray2b_T *array2b) {
  assert(array2b);
  assert(*array2b);
  for (int block; block<(*array2b)->blockNum; block++)
    Array_free(Array_get((*array2b)->blocks, block));
  Array_free(&((*array2b)->blocks));
  FREE(*array2b);
}

int UArray2b_width(UArray2b_T array2b) {
  assert(array2b);
  return array2b->width;
}

int UArray2b_height(UArray2b_T array2b) {
  assert(array2b);
  return array2b->height;
}

int UArray2b_size(UArray2b_T array2b) {
  assert(array2b);
  return array2b->size;
}

int UArray2b_blocksize(UArray2b_T array2b) {
  assert(array2b);
  return array2b->blocksize;
}

void *UArray2b_at(UArray2b_T array2b, int i, int j) {
  assert(array2b);
  assert(i >= 0 && i<array2b->height);
  assert(j >= 0 && j<array2b->width);
  int row = i/array2b->blocksize;
  int col = j/array2b->blocksize;
  Array_T* block = Array_get(array2b->blocks, row*array2b->blockWidth + col);
  row = i%array2b->blocksize;
  col = j%array2b->blocksize;
  return Array_get(*block, row*array2b->blocksize + col);
}

void UArray2b_put(UArray2b_T array2b, int i, int j, void* elem) {
  assert(array2b);
  assert(i >= 0 && i<array2b->height);
  assert(j >= 0 && j<array2b->width);
  int row = i/array2b->blocksize;
  int col = j/array2b->blocksize;
  Array_T* block = Array_get(array2b->blocks, row*array2b->blockWidth + col);
  row = i%array2b->blocksize;
  col = j%array2b->blocksize;
  Array_put(*block, row*array2b->blocksize + col, elem);
}

/* return a pointer to the cell in column i, row j;index out of range is a checked run-time error*/
void UArray2b_map(UArray2b_T array2b,
       void apply(int i, int j, UArray2b_T array2b, void *elem, void *cl),
    	          void *cl) {
  assert(array2b);
  Array_T* block __attribute__ ((unused));
  int rowLimit;
  int colLimit;
  for (int blockRow=0; blockRow<=array2b->blockHeight; blockRow++) {
   if (blockRow == array2b->blockHeight) rowLimit = array2b->edgeHeight;
   else rowLimit = array2b->blocksize; 
   for (int blockCol=0; blockCol<=array2b->blockWidth; blockCol++) {
    if (blockCol == array2b->blockWidth) colLimit = array2b->edgeWidth;
    else colLimit = array2b->blocksize;
    block = Array_get(array2b->blocks, blockRow*array2b->blockWidth+blockCol);
    for (int row=0; row<rowLimit; row++) {
     for (int col=0; col<colLimit; col++) {
      apply(col, row, array2b, Array_get(*block, row*colLimit+col), cl);
     }
    }
   }
  }
}
/* visits every cell in one block before moving to another block */
/* it is a checked run-time error to pass a NULL Array2
   to any function in this interface */
