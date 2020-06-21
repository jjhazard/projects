#include "umLoad.h"
#include <stdint.h>
#include <sys/stat.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static int programLength(char* fileName) {
  struct stat file;
  stat(fileName, &file);
  if (file.st_size && !(file.st_size%4)) return file.st_size/4;
  fprintf(stderr, "Program file invalid.\n");
  exit(1);
}

static void getInstruction(FILE* fp, uint32_t* counter) {
  *counter = ((uint32_t)getc(fp) << 24);
  *counter |= ((uint32_t)getc(fp) << 16);
  *counter |= ((uint32_t)getc(fp) <<  8);
  *counter |=  (uint32_t)getc(fp);
}

Segment_T collectInstructions(char* fileName) {
  FILE* fp = fopen(fileName, "r");
  if (fp == NULL) {
    fprintf(stderr, "Could not open file.\n");
    exit(1);
  }
  
  int progIndex = 0;
  int length = programLength(fileName);

  Segment_T program = Segment_new(length);
  uint32_t* counter = program->instructions;
  
  while (progIndex < length) {    
    getInstruction(fp, counter);
    counter++;
    progIndex += 1;
  }

  fclose(fp);

  return program;
}
