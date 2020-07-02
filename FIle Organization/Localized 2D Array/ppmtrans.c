#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "assert.h"
#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "pnm.h"

void transpose(Pnm_ppm image, int rotation, int flip);
void apply_90(int i, int j, A2Methods_Array2 array2,
	      A2Methods_Object *ptr, void *cl);
void apply_180(int i, int j, A2Methods_Array2 array2,
	       A2Methods_Object *ptr, void *cl);
void apply_270(int i, int j, A2Methods_Array2 array2,
	       A2Methods_Object *ptr, void *cl);
void apply_vflip(int i, int j, A2Methods_Array2 array2,
	         A2Methods_Object *ptr, void *cl);
void apply_hflip(int i, int j, A2Methods_Array2 array2,
	         A2Methods_Object *ptr, void *cl);
A2Methods_T methods;

int main(int argc, char *argv[]) {
  int rotation = -1;
  int flip = 0;
  FILE *fp;
  methods = array2_methods_plain; // default to UArray2 methods
  assert(methods);
  A2Methods_mapfun *map = methods->map_default; // default to best map
  assert(map);
#define SET_METHODS(METHODS, MAP, WHAT) do { \
      methods = (METHODS); \
      assert(methods); \
      map = methods->MAP; \
      if (!map) { \
        fprintf(stderr, "%s does not support " WHAT "mapping\n", argv[0]); \
        exit(1); \
      } \
    } while(0)

  int i;
  for (i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "-row-major")) {
      SET_METHODS(array2_methods_plain, map_row_major, "row-major");
    } else if (!strcmp(argv[i], "-col-major")) {
      SET_METHODS(array2_methods_plain, map_col_major, "column-major");
    } else if (!strcmp(argv[i], "-block-major")) {
      SET_METHODS(array2_methods_blocked, map_block_major, "block-major");
    } else if (!strcmp(argv[i], "-rotate")) {
      assert(i + 1 < argc);
      char *endptr;
      rotation = strtol(argv[++i], &endptr, 10);
      assert(*endptr == '\0'); // parsed all correctly
      assert(rotation == 0   || rotation == 90
          || rotation == 180 || rotation == 270);
    } else if (!strcmp(argv[i], "-flip")) {
      assert(i + 1 < argc);
      if (!strcmp(argv[++i], "horizontal")) flip += 1;
      else if (!strcmp(argv[i], "vertical")) flip -= 1;
      assert(flip == -1 || flip == 1);
    } else if (*argv[i] == '-') {
      fprintf(stderr, "%s: unknown option '%s'\n", argv[0], argv[i]);
      exit(1);
    } else if (argc - i > 2) {
      fprintf(stderr, "Usage: %s [-rotate <angle>] "
              "[-{row,col,block}-major] [filename]\n", argv[0]);
      exit(1);
    } else {
      break;
    }
  }

  //After checking the args, get the image
  if (argc - i == 1) {
    fp = fopen(argv[i], "r");
    if (fp == NULL) {
      fprintf(stderr, "Unable to open file: %s\n", argv[i]);
      exit(1);
    }
  } else fp = stdin;

  //Here we read the pnm file in from the file or stdin
  Pnm_ppm image = Pnm_ppmread(fp, methods);
  fclose(fp);
  //Here we transpose the image
  transpose(image, rotation, flip);
  //Here we write the image to output
  Pnm_ppmwrite(stdout, image);  
  Pnm_ppmfree(&image);  
}

//Transpose uses the input args to transpose the image
void transpose(Pnm_ppm image, int rotation, int flip) {
  A2Methods_Array2 tran;
  //180 degree rotation or any flip, image has dimensions width by height
  if ((rotation == 180) || flip) {
    tran = image->methods->
      new_with_blocksize(image->width,
			 image->height,
			 sizeof(int),
			 image->methods->blocksize(image->pixels));
    //if not a flip, rotate 180
    if (!flip) {
      fprintf(stdout, "180 rotation requested\n");
      image->methods->
	map_default(image->pixels, apply_180, &tran);
    //if negative flip, flip vertical
    } else (flip < 0) {
      fprintf(stdout, "vertical flip requested\n");
      image->methods->
	map_default(image->pixels, apply_vflip, &tran);
    //if positive flip, flip horizontal
    } else {
      fprintf(stdout, "horizontal flip requested\n");
      image->methods->
	map_default(image->pixels, apply_hflip, &tran);
    }

  //If new image changes dimensions, swap width and height
  } else {
    tran = image->methods->
      new_with_blocksize(image->height,
			 image->width,
			 sizeof(int),
			 image->methods->blocksize(image->pixels));
    //if rotation is 90, rotate 90
    if (rotation == 90) {
      fprintf(stdout, "90 rotation requested\n");
      image->methods->
	map_default(image->pixels, apply_90, &tran);
    //else rotation is 270
    } else {
      fprintf(stdout, "270 rotation requested\n");
      image->methods->
	map_default(image->pixels, apply_270, &tran);
    }
  }
  //assign the new image to the Pnm struct
  image->methods->free(&(image->pixels));
  image->width = image->methods->width(tran);
  image->height = image->methods->height(tran);
  image->pixels = tran;
}

//apply the formula to rotate 90 degrees
// j, r = height - r - 1, j
void apply_90(int col, int row, A2Methods_Array2 array2,
	      A2Methods_Object *ptr, void *cl) {
  (void)array2;
  A2Methods_Array2* newArray = cl;
  int* newElem = methods->at(*newArray, methods->width(newArray) - row - 1, col);
  *newElem = *(int*)ptr;
}

//apply the formula to rotate 180 degrees
// j, r = width - c - 1, height - r - 1
void apply_180(int col, int row, A2Methods_Array2 array2,
	       A2Methods_Object *ptr, void *cl) {
  (void)array2;
  A2Methods_Array2* newArray = cl;
  int* newElem = methods->at(*newArray,
			    methods->width(newArray) - col - 1,
			    methods->height(newArray) - row - 1);
  *newElem = *(int*)ptr;
}

//apply the formula to rotate 270 degrees
// j, r = row, width - c - 1
void apply_270(int col, int row, A2Methods_Array2 array2,
	       A2Methods_Object *ptr, void *cl) {
  (void)array2;
  A2Methods_Array2* newArray = cl;
  int* newElem = methods->at(*newArray, row,
			     methods->height(*newArray) - col - 1);
  *newElem = *(int*)ptr;
}

//apply the formula to flip vertical
// j, r = j, height - r - 1
void apply_vflip(int col, int row, A2Methods_Array2 array2,
	         A2Methods_Object *ptr, void *cl) {
  (void)array2;
  A2Methods_Array2* newArray = cl;
  int* newElem = methods->at(*newArray, col,
			     methods->height(*newArray) - row - 1);
  *newElem = *(int*)ptr;
}

//apply the formula to rotate 90 degrees
// j, r = width- j - 1, r
void apply_hflip(int col, int row, A2Methods_Array2 array2,
	         A2Methods_Object *ptr, void *cl) {
  (void)array2;
  A2Methods_Array2* newArray = cl;
  int* newElem = methods->at(*newArray,
			     methods->width(*newArray)- col - 1, row);
  *newElem = *(int*)ptr;
}
	      
