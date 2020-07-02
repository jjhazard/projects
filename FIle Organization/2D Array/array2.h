#include "array.h"

typedef struct UArray2_T {

  int height;
  int width;
  int size;
  Array_T rows;

} UArray2_T;

UArray2_T* UArray2_new(int height, int width, int size);
void UArray2_free(UArray2_T *grid);
void* UArray2_put(UArray2_T* grid, int height, int width, void* data);
void* UArray2_at(UArray2_T* grid, int height, int width);
int UArray2_height(UArray2_T* grid);
int UArray2_width(UArray2_T* grid);
