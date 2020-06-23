
#ifndef UM_MEM_H
#define UM_MEM_H

#include <seq.h>
#include <stdint.h>

struct Segment_T {
  uint32_t length;
  uint32_t* instructions;
};

typedef struct Segment_T *Segment_T;

Segment_T Segment_new(uint32_t length);

struct Mem_T {
  Segment_T segments;
  unsigned next;
  unsigned size;
  uint32_t* counter;
  uint32_t* unmapped;
  uint32_t top;
  uint32_t last;
};

typedef struct Mem_T *Mem_T;

extern struct Mem_T Mem_new(char* fileName);
extern uint32_t Mem_map(Mem_T memory, uint32_t length);
extern void Mem_unmap(Mem_T memory, uint32_t index);
extern void Mem_load(Mem_T memory, uint32_t id);
extern void Mem_release(Mem_T memory);
#endif
