#include "array2.h"
#include "mem.h"
#include <stdio.h>
#include <stdlib.h>
#include "assert.h"

UArray2_T* UArray2_new(int height, int width, int size) {
  UArray2_T* grid =  (UArray2_T*)calloc(1, sizeof(UArray2_T));
  grid->height = height;
  grid->width = width;
  grid->size = size;
  assert(size > 0);
  if ((height < 1) || (width < 1)) grid->rows = NULL;
  else {
    grid->rows = Array_new(height, sizeof(Array_T));
    for (int i = 0; i < height; i++) {
      Array_T row = Array_new(width, size);
      (void)Array_put(grid->rows, i, &row);
    }
  }
  return grid;
}

void UArray2_free(UArray2_T *grid) {
  assert(grid);
  for (int i = 0; i<grid->height; i++) {
    Array_T* row = Array_get(grid->rows, i); 
    Array_free(row);
  }
  Array_free(&(grid->rows));
  free(grid);
}

void* UArray2_put(UArray2_T *grid, int height, int width, void* data) {
  assert(grid);
  assert(height >= 0 && height < grid->height);
  assert(width >= 0 && width < grid->width);
  Array_T* row = Array_get(grid->rows, height);
  Array_put(*row, width, data);
  return data;
}
 
void* UArray2_at(UArray2_T* grid, int height, int width) {
  assert(grid);
  assert(height >= 0 && height < grid->height);
  assert(width >= 0 && width < grid->width);
  Array_T* row = Array_get(grid->rows, height);
  return Array_get(*row, width);
}

int UArray2_height(UArray2_T* grid) {
  assert(grid);
  return grid->height;
}

int UArray2_width(UArray2_T* grid) {
  assert(grid);
  return grid->width;
}

int UArray2_size(UArray2_T* grid) {
  assert(grid);
  return grid->size;
}
