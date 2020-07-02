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

int main() {

  uint64_t nosign = 0;
  //int64_t sign = 0;
  unsigned width = 0;
  for (unsigned msb=0; msb<64; msb++) {
    for (unsigned j=1; j<65; j++) {
      if (msb < j) {
	assert(Bitpack_fitsu(((uint64_t)1 << msb), j) == true);
	assert(Bitpack_fitss((( int64_t)1 << msb), j) == true);
      } else {
	assert(Bitpack_fitsu(((uint64_t)1 << msb), j) == false);
	assert(Bitpack_fitss((( int64_t)1 << msb), j) == false);
      }
    }
  }

  for (unsigned msb=0; msb<64; msb++) {
    for (unsigned lsb=0; lsb<=msb; lsb++) {
      width = msb-lsb;
      nosign = Bitpack_getu(((uint64_t)1<<msb), width+1, lsb);
      assert( nosign == ((uint64_t)1<<width) );
      //sign = Bitpack_gets(((uint64_t)1<<msb), width+1, lsb);
      //assert(   sign == ((uint64_t)1<<width) );
    }
  }

  for (unsigned msb=0; msb<64; msb++) {
    for (unsigned lsb=0; lsb<=msb; lsb++) {
      width = msb-lsb;
      nosign = Bitpack_newu( 0, width+1, lsb, ((uint64_t)1<<width));
      //printf("%lx %lx\n", nosign, ((uint64_t)1<<msb));
      assert( nosign == ((uint64_t)1<<msb) );
      nosign = Bitpack_news( 0, width+1, lsb, (( int64_t)1<<width));
      //printf("%lx %lx\n\n", nosign, ((uint64_t)((int64_t)1<<width)<<lsb));
      assert( nosign == (((uint64_t)1<<width)<<lsb) );
    }
  }

  for (unsigned i=0; i<16; i++)
    printf("%f\n", Arith_chroma_of_index(i)); 

  float range = 0.6 / 31;
  int x = 1;
  for (float num=-0.3; num<0.3; num += range)
    printf("%d - %f\n", x++, num*50); 

  for (unsigned index=0; index<32; index++) {
    if (index < 16) printf("%d %f \n", index, (float)index/50);
    else printf("%d %d %f\n",index, -(index - 15), -((float)(index - 15)/50));
  }
}
