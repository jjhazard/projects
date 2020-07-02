#include <stdio.h>
#include <stdlib.h>
#include <pnmrdr.h>
#include "array2.h"

UArray2_T* read_array(Pnmrdr_T *reader, Pnmrdr_mapdata data);
int check_sudoku(UArray2_T* sudoku, Pnmrdr_mapdata data);
int check_3x3_box(UArray2_T* sudoku, int baseR, int baseC);

int main(int argc, char** argv) {

  //validate arguments
  if (argc > 2) {
    fprintf(stderr, "EXCEPTION: Too Many Arguments\n");
    exit(1);
  }
  FILE *fp;
  if (argc != 2) fp = stdin;
  else fp = fopen(argv[1], "rb");
  if (fp == NULL) {
    fprintf(stderr, "EXCEPTION: Unable to read file.\n");
    exit(1);
  }

  //read header and validate dimensions
  Pnmrdr_T reader = Pnmrdr_new(fp);
  Pnmrdr_mapdata data = Pnmrdr_data(reader);
  if (((data.width != 9)||(data.height != 9))||(data.denominator != 9)) {
    Pnmrdr_free(&reader);
    fprintf(stderr, "EXCEPTION: Sudoku must be 9 by 9\n");
    exit(1);
  }

  //read sudoku data and validate content
  UArray2_T* sudoku = read_array(&reader, data);
  Pnmrdr_free(&reader);
  if (sudoku == NULL) {
    fprintf(stderr, "EXCEPTION: Sudoku element invali\nd");
    exit(1);
  }
  
  //check sudoku solution
  int test = check_sudoku(sudoku, data);

  //free memory and report test result
  UArray2_free(sudoku);
  if (argc > 1) fclose(fp);
  printf("%d\n", test);
  exit(test);
}

UArray2_T* read_array(Pnmrdr_T *reader, Pnmrdr_mapdata data) {

  //create grid
  UArray2_T* sudoku = UArray2_new(data.height, data.width, sizeof(int));
  int x = 0;
  for (unsigned int row=0; row<data.height; row++) {
    for (unsigned int col=0; col<data.width; col++) {

      //get next entry
      x = Pnmrdr_get(*reader);

      //validate input before saving
      if ((x > 0)&&(x < 10)) UArray2_put(sudoku, row, col, &x);
      else {
	UArray2_free(sudoku);
	return NULL;
      }
    }
  }
  return sudoku;
}

int check_sudoku(UArray2_T* sudoku,  Pnmrdr_mapdata data) {
  
  //loop through grid applying formula horizontally and vertically
  int elem_index = 0;
  int* value = NULL;
  for (unsigned int row=0; row<data.height; row++) {
    for (unsigned int col=0; col<data.width; col++) {

      //reduce stored value to original element, subtract 1 for index
      value = UArray2_at(sudoku, row, col);
      elem_index = abs(*value)%10-1;
      
      //get the row value at elem_index
      value = (int*)UArray2_at(sudoku, row, elem_index); 

      //if negative, exit failure, else make value negative
      if (*value < 1) return 1;
      *value *= -1;

      //get the column value at index elem
      value = UArray2_at( sudoku, elem_index, col );

      //if more than 9 units from 0, exit failure, increase distance by 10 
      if (abs(*value) > 9) return 1;
      if (*value < 0) *value -= 10;
      else *value += 10;
    }
  }

  //check the four corner boxes: top left and right, bottom left and right
  if (check_3x3_box(sudoku, 0, 0)) return 1;
  if (check_3x3_box(sudoku, 0, 6)) return 1;
  if (check_3x3_box(sudoku, 6, 0)) return 1;
  if (check_3x3_box(sudoku, 6, 6)) return 1;
  return 0;
}

int check_3x3_box(UArray2_T* sudoku, int baseRow, int baseCol) {
  int elem = 0;
  int* value = NULL;
  
  //for each element in the square
  for (int row=baseRow; row < baseRow+3; row++) {
    for (int col=baseCol; col < baseCol+3; col++) {

      //get the element
      value = UArray2_at( sudoku, row, col );
      printf("value %d\n", *value);
      elem = abs(*value)%10;
      printf("elem %d\n", elem);
      //(elem/(row+1))-1 = row index, (elem%(col+1))-1 = column index
      printf("row index: %d\ncol index: %d\n", (elem-1)/3+baseRow, ((elem-1)%3)+baseCol );
      value = UArray2_at( sudoku, (elem-1)/3+baseRow, ((elem-1)%3)+baseCol );
      printf("index %d\n", *value);

      //if the value is positive, elem is repeat, exit failure
      if (*value > 0) return 1;
      //otherwise, make value positive
      *value *= -1;

        for (int row2=baseRow; row2 < baseRow+3; row2++) {
    for (int col2=baseCol; col2 < baseCol+3; col2++) {
      value = UArray2_at( sudoku, row2, col2 );
      printf("%d ", *value);
    }
    printf("\n");
  }       

    }
  }
  return 0;
}
