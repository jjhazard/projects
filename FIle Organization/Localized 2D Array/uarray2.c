#include <array.h>
#include "uarray2.h"
#include "mem.h"
#include <stdio.h>
#include <stdlib.h>
#include "assert.h"

struct UArray2_T {
  int width;
  int height;
  int size;
  Array_T rows;
};

//UUArray2_new creates a UUArray2 object of the desired dimensions
//parameter height is the height of the UUArray2
//parameter width is the width of the UUArray2
//parameter size is the size of the UUArray2 elements
UArray2_T UArray2_new(int width, int height, int size) {
  UArray2_T array2;
  NEW(array2);
  array2->width = width;
  array2->height = height;
  array2->size = size;
  assert(size > 0);
  if ((height < 1) || (width < 1)) array2->rows = NULL;
  else {
    array2->rows = Array_new(height, sizeof(Array_T));
    for (int i = 0; i < height; i++) {
      Array_T row = Array_new(width, size);
      (void)Array_put(array2->rows, i, &row);
    }
  }
  return array2;
}

//UArray2_free frees a UArray2 object
void UArray2_free(UArray2_T* array2) {
  assert(array2);
  assert(*array2);
  for (int i=0; i<(*array2)->height; i++) {
    Array_T* row = Array_get((*array2)->rows, i); 
    Array_free(row);
  }
  Array_free(&((*array2)->rows));
  FREE(*array2);
}
 
//UArray2_at gets a pointer an element of a UArray2 object
void* UArray2_at(UArray2_T array2, int width, int height) {
  printf("width - %d - height - %d\n", width, height); 
  assert(array2);
  assert(height >= 0 && height < array2->height);
  assert(width >= 0 && width < array2->width);
  Array_T* row = Array_get(array2->rows, height);
  return Array_get(*row, width);
}

//UArray2_height gets the height of a UArray2 object
int UArray2_height(UArray2_T array2) {
  assert(array2);
  return array2->height;
}

//UArray2_width gets the width of a UArray2 object
int UArray2_width(UArray2_T array2) {
  assert(array2);
  return array2->width;
}

//UArray2_size gets the size of a UArray2 object
int UArray2_size(UArray2_T array2) {
  assert(array2);
  return array2->size;
}

int UArray2_blocksize(UArray2_T array2) {
  assert(array2);
  return 1;
}

void UArray2_map_row_major(UArray2_T array2,
         void apply(int i, int j, UArray2_T array2, void *elem, void *cl),
			   void *cl) {
  assert(array2);
  Array_T* row __attribute__((unused));
  for (int rowNum=0; rowNum<UArray2_height(array2); rowNum++) {
    Array_T* row = Array_get(array2->rows, rowNum);
    for (int colNum=0; colNum<UArray2_width(array2); colNum++)
      apply(colNum, rowNum, array2, Array_get(*row, colNum), cl);
  }
}

void UArray2_map_col_major(UArray2_T array2,
	 void apply(int i, int j, UArray2_T array2, void *elem, void *cl),
			   void *cl) {
  assert(array2);
  Array_T* row __attribute__((unused));
  for (int colNum=0; colNum<UArray2_width(array2); colNum++)
    for (int rowNum=0; rowNum<UArray2_height(array2); rowNum++) {
      row = Array_get(array2->rows, rowNum);
      apply(colNum, rowNum, array2, Array_get(*row, colNum), cl);
    }

}
