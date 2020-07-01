#include <stdio.h>
#include <stdlib.h>
#include <pnmrdr.h>
int pixel_sum(Pnmrdr_T *reader, Pnmrdr_mapdata data);
void print_image_data(double total_sum, Pnmrdr_mapdata data);

// Main checks the args and reads the input
int main(int argc, char** argv) {
  if (argc > 2) {
    fprintf(stderr, "EXCEPTION: Too Many Arguments");
    exit(1);
  }
  FILE *fp;
  if (argc != 2) fp = stdin;
  else fp = fopen(argv[0], "rb");
  if (fp == NULL) {
    fprintf(stderr, "EXCEPTION: Unable to read file.");
    exit(1);
  }    
  Pnmrdr_T reader = Pnmrdr_new(fp);
  Pnmrdr_mapdata data = Pnmrdr_data(reader);
  print_image_data((double)pixel_sum(&reader, data)/data.denominator, data);
  Pnmrdr_free(&reader);
  fclose(fp);
}

// pixel_sum reads the pixels and computes the average
int pixel_sum(Pnmrdr_T *reader, Pnmrdr_mapdata data) {
  unsigned int pixels_left = data.width*data.height, total_sum = 0;
  for (pixels_left=pixels_left; pixels_left > 0; pixels_left--) {
    total_sum += Pnmrdr_get(*reader);
  } return total_sum;
}

// print_image_data prints the average of the pixels
void print_image_data(double total_sum, Pnmrdr_mapdata data) {
  printf("%.3lf\n", (total_sum/data.width)/data.height);
}
