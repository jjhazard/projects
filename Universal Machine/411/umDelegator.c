#include "seq.h"
#include "umMem.h"
#include "umDelegator.h"
#include <signal.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

typedef enum {
  False = 0,
  LoadSeg = 1,
  StoreSeg = 2,
  Add = 3,
  Multiply = 4,
  Divide = 5,
  Nand = 6,
  Halt = 7,
  Map = 8,
  Unmap = 9,
  Output = 10,
  Input = 11,
  LoadProg = 12,
  LoadVal = 13
} opCode;

//This variable and subsequent function exist to ensure no memory is
//leaked when the user sends a shutdown signal.
volatile sig_atomic_t uninterrupted = 1;
static void handle_interrupt_signal(int _) {
  (void)_;
  uninterrupted = 0;
}


#define getOpCode(instr) (opCode)((instr) >> 28)

#define REGA(instr) ((unsigned)((instr) >> 6) & 0x7)
#define REGB(instr) ((unsigned)((instr) >> 3) & 0x7)
#define REGC(instr) ((unsigned)(instr) & 0x7)

#define REGS(instr) ((unsigned)((instr) >> 25) & 0x7)
#define VALUE(instr) ((instr) & 0x1FFFFFF)

//The um delegation loop.
//Calls collectInstructions from umLoad, which validates and reads the input
//Calls Mem_new from umMem, which stores the instructions in a Mem structure
//Processes and extracts all information from the instruction
//Calls execute from umOperate, which executes the operation
//If the Interupt Signal is sent, frees all allocated memory
void executeUM(char* fileName) {
  uint32_t regs[8] = { 0 };  
  uint32_t instruction;
  unsigned regID;
  struct Mem_T memory = Mem_new(fileName);
  signal(SIGINT, handle_interrupt_signal);

  //while no interrupt signal is sent
  while(uninterrupted) {
    //get the instruction and extract the opCode
    instruction = *memory.counter;

    //execute operation and increment counter
    //Call the function related to the operation Opcode
    switch (getOpCode(instruction)) {
    case False:
      if (regs[REGC(instruction)])
	regs[REGA(instruction)] = regs[REGB(instruction)];
      break;
    case LoadSeg:
      regs[REGA(instruction)] =
	memory.segments[regs[REGB(instruction)]].
	instructions[regs[REGC(instruction)]];
      break;
    case StoreSeg:
      memory.segments[regs[REGA(instruction)]].
	instructions[regs[REGB(instruction)]] =
	regs[REGC(instruction)];
      break;
    case Add:
      regs[REGA(instruction)] =
	regs[REGB(instruction)] + regs[REGC(instruction)];
      break;
    case Multiply:
      regs[REGA(instruction)] =
	regs[REGB(instruction)] * regs[REGC(instruction)];
      break;
    case Divide:
      regs[REGA(instruction)] =
	regs[REGB(instruction)] / regs[REGC(instruction)];
      break;
    case Nand:
      regs[REGA(instruction)] =
	~(regs[REGB(instruction)] & regs[REGC(instruction)]);
      break;
    case Halt:
      Mem_release(&memory);
      exit(0);
      break;
    case Map:
      regs[REGB(instruction)] =
	Mem_map(&memory, regs[REGC(instruction)]);
      break;
    case Unmap:
      Mem_unmap(&memory, regs[REGC(instruction)]);
      break;
    case Output:
      putchar(regs[REGC(instruction)]);
      break;
    case Input:
      regID = REGC(instruction);
      regs[regID] = getchar();
      if (regs[regID] == (uint32_t)EOF) regs[regID] = -1;
      break;
    case LoadProg:
      regID = regs[REGB(instruction)];
      if (regID)
	Mem_load(&memory, regID);
      memory.counter =
	memory.segments[regID].instructions + regs[REGC(instruction)];
      continue;
    case LoadVal:
      regs[REGS(instruction)] = VALUE(instruction);
      break;
    default:
      //unknown opCode compromises the um contract, abort
      fprintf(stderr,
	      "Unknown opCode %d, aborting.\n",
	      (int)getOpCode(instruction));
      Mem_release(&memory);
      exit(0);
      break;
    }
    memory.counter++;
  }

  //Only reached if the shutdown signal is sent, frees all program memory
  Mem_release(&memory);
}




