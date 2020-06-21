#!/usr/bin/env python

from argparse import ArgumentParser
from exp1bytecode_lex import lexer
from exp1bytecode_interp_gram import parser
from exp1bytecode_interp_state import state

#####################################################################################
def interp_program():
    'execute abstract bytecode machine'
    
    # We cannot use the list iterator here because we
    # need to be able to interpret jump instructions
    # start at the first instruction in program
    state.instr_ix = 0
    
    # keep interpreting until we run out of instructions
    # or we hit a 'stop'
    while True:
        if state.instr_ix == len(state.program):
            # no more instructions
            break
        else:
            # get instruction from program
            instr = state.program[state.instr_ix]
      
        
        # instruction format: (type, [arg1, arg2, ...])
        type = instr[0]
        
        # interpret instruction
        if type == 'pushn':
            # PUSH
            state.stack += [instr[1]]
            state.instr_ix += 1
            
        elif type == 'pushv':
            # PUSH
            state.stack += [state.symbol_table[instr[1]]]
            state.instr_ix += 1
            
        elif type == 'print':
            # PRINTS
            if instr[1]:
                print("> {}".format(instr[1]))
            print("> {}".format(state.stack.pop()))
            state.instr_ix += 1
            
        elif type == 'pop':
            # POP
            state.stack.pop()
            state.instr_ix += 1
            
        elif type == 'ask':
            # INPUT NAME
            state.stack += [int(input(instr[1]))]
            state.instr_ix += 1
        
        elif type == 'store':
            # STORE type exp
            state.symbol_table[instr[1]] = state.stack[-1]
            state.stack.pop()
            state.instr_ix += 1
        
        elif type == 'dup':
            # DUPLICATE\
            state.stack += [state.stack[-1]]
            state.instr_ix += 1
        
        elif type == 'add':
            # ADD
            state.stack[-2] += state.stack[-1]
            state.stack.pop()
            state.instr_ix += 1
        
        elif type == 'sub':
            # SUBTRACT
            state.stack[-2] -= state.stack[-1]
            state.stack.pop()
            state.instr_ix += 1
                  
        elif type == 'mul':
            # MULTIPLY
            state.stack[-2] = state.stack[-2] * state.stack[-1]
            state.stack.pop()
            state.instr_ix += 1
        
        elif type == 'div':
            # DIVIDE
            state.stack[-2] = state.stack[-2] // state.stack[-1]
            state.stack.pop()
            state.instr_ix += 1
                  
        elif type == 'equ':
            # EQUAL
            if state.stack[-2] == state.stack[-1]:
                state.stack[-2] = 1
            else:
                state.stack[-2] = 0
            state.stack.pop()
            state.instr_ix += 1
              
        elif type == 'leq':
            # EQUAL
            if state.stack[-2] <= state.stack[-1]:
                state.stack[-2] = 1
            else:
                state.stack[-2] = 0
            state.stack.pop()
            state.instr_ix += 1
                  
        elif type == 'jumpT':
            # JUMPT exp label
            if state.stack[-1]:
                state.instr_ix = state.label_table.get(instr[1], None)
            else:
                state.instr_ix += 1
            state.stack.pop()

        elif type == 'jumpF':
            # JUMPF exp label
            if not state.stack[-1]:
                state.instr_ix = state.label_table.get(instr[1], None)
            else:
                state.instr_ix += 1
            state.stack.pop()

        elif type == 'jump':
            # JUMP label
            state.instr_ix = state.label_table.get(instr[1], None)
        
        elif type == 'stop':
            # STOP
            if instr[1]:
                print("> {}".format(instr[1]))
            break

        elif type == 'noop':
            # NOOP
            state.instr_ix += 1
        
        else:
            raise ValueError("Unexpected instruction type: {}".format(p[1]))


#####################################################################################

def interp(input_stream):
    'driver for our Exp1bytecode interpreter.'

    # initialize our abstract machine
    state.initialize()
    
    # build the IR
    parser.parse(input_stream, lexer=lexer)
    
    # interpret the IR
    interp_program()

#####################################################################################
if __name__ == '__main__':
    # parse command line args
    aparser = ArgumentParser()
    aparser.add_argument('input')
    
    args = vars(aparser.parse_args())
    
    f = open(args['input'], 'r')
    input_stream = f.read()
    f.close()
    
    interp(input_stream=input_stream)