#include "bitpack.h"
#include <stdint.h>
#include "assert.h"
#include <stdio.h>
//subtract from width and check if 0, shift n and check if 0
bool Bitpack_fitsu(uint64_t n, unsigned width) {
  assert(width <= 64);
  if (!width) return width;
  while ((n = (n >> 1)) && (--width)) continue;
  return width;
}

//if negative, take bitwise complement and add one to width for sign bit
bool Bitpack_fitss( int64_t n, unsigned width) { 
  assert(width <= 64);
  if (!width) return width;
  uint64_t m = 0;
  if (n < 0) m = (~(uint64_t)n << 1);
  else m = (uint64_t)n;
  while ((m = (m >> 1)) && (--width)) continue;
  return width;
}

//shift the word to bit 0 and extract
uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb) {
  assert(lsb + width <= 64);
  if (width ==  0) return    0;
  if (width == 64) return word;
  width = 64 - width;  
  return ((word << (width - lsb)) >> width);
}

//same, but if sign bit is 1 move it to the most significant bit
int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb) {
  assert(lsb + width <= 64);
  if (width ==  0) return (int64_t)0;
  if (width == 64) return (int64_t)word;
  width = 64 - width;
  word = ((word << (width - lsb)) >> width);
  if ( (word >> (width-1)) ) word &= ((uint64_t)1 << 63);
  return (int64_t)word;
}

//clear bits and set
uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb, uint64_t value) {
  assert(lsb + width <= 64);
  if (width ==  0) return word;
  if (width == 64) return value;
  word &= ~(~(0xFFFFFFFFFFFFFFFF << width) << lsb);
  return word | (value << lsb);
}

//same as before, but if negative mark the left bit
uint64_t Bitpack_news(uint64_t word, unsigned width, unsigned lsb,  int64_t value) {
  assert(lsb + width <= 64);
  if (width ==  0) return word;
  if (width == 64) return (uint64_t)value;
  word &= ~(~(0xFFFFFFFFFFFFFFFF << width) << lsb);
  if (value < 0) word |= (1 << (width+lsb-1));
  uint64_t tester = ((uint64_t)value << (64 - width));
  return word | (tester >> (64 - width - lsb));
}
