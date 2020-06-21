#include "umMem.h"
#include "umLoad.h"
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

Segment_T Segment_new(uint32_t length) {
  Segment_T segment = calloc(1, sizeof(struct Segment_T));
  segment->instructions = calloc(length, sizeof(uint32_t));
  segment->length = length;
  return segment;
}

struct Mem_T Mem_new(char* fileName) {
  struct Mem_T memory = { .segments = collectInstructions(fileName),
			  .next     = 1,
			  .size     = 512,
			  .counter  = NULL,
			  .unmapped = malloc(512*sizeof(uint32_t)),
			  .top      = 0,
                          .last     = 512 };
  memory.segments = realloc(memory.segments, 512 * sizeof(struct Segment_T));
  memory.counter = memory.segments[0].instructions;
  return memory;
}

//map a new segment of length
uint32_t Mem_map(Mem_T memory, uint32_t length) {
  uint32_t id;
  //if there is unmapped memory, acquire the id and store
  if ( memory->top ) {
    id = memory->unmapped[--memory->top];
    memory->segments[id].instructions = calloc(length, sizeof(uint32_t));
    memory->segments[id].length = length;

  //if ther is no unmapped memory, create a new memory segment
  } else {
    id = memory->next++;
    Segment_T segments = memory->segments;

    //if we resized the segment sequence, resize the segHeads
    if ( memory->next == memory->size ) {
      memory->size *= 2;
      segments =
	realloc(segments, memory->size*sizeof(struct Segment_T));
    }
    segments[id].instructions = calloc(length, sizeof(uint32_t));
    segments[id].length = length;
    memory->segments = segments;
  }
  return id;
}

//unmap memory segment id and set the address to 0, add id to unmapped memory
void Mem_unmap(Mem_T memory, uint32_t id) {
  free( memory->segments[id].instructions );
  memory->segments[id].length = 0;
  if ( memory->top + 1 == memory->last ) {
    memory->last *= 2;
    memory->unmapped = realloc(memory->unmapped,
			       memory->last*sizeof(uint32_t) );
  }
  memory->unmapped[ memory->top++ ] = id;
}

//erase segment 0 and copy segment id to segment 0
void Mem_load(Mem_T memory, uint32_t id) {
    int length = memory->segments[id].length;
    free( memory->segments[0].instructions );
    memory->segments[0].instructions =
      calloc(length, sizeof(uint32_t));
    memcpy(memory->segments[0].instructions,
	   memory->segments[id].instructions,
	   sizeof(uint32_t)*length);
    memory->segments[0].length = length;
}

//free the um memory
void Mem_release(Mem_T memory) {

  //while the memory has length greater than 0
  unsigned last = memory->next;
  while ( --last ) {

    //acquire the segment address and free it if it is not 0
    if (memory->segments[last].length)
      free( memory->segments[last].instructions );
  }

  //free the segment and unmapped segment sequences
  free(memory->segments[0].instructions);
  free(memory->segments);
  free(memory->unmapped);
}
