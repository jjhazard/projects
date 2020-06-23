#!/usr/bin/env python

from argparse import ArgumentParser
from stackmachine_lex import lexer
from stackmachine_interp_gram import parser
from stackmachine_interp_state import state

#####################################################################################
def interp_program():
    'execute stack machine'
    
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
        
        if type == 'push':
            # PUSH NUMBER or VAR
            val = eval_exp_tree(instr[1])
            state.stack.append(val)
            state.instr_ix += 1
            
        elif type == 'pop':
            # POP top of stack
            state.stack.pop()
            state.instr_ix += 1
        
        # interpret instruction
        elif type == 'print':
            # PRINT top of stack
            val = state.stack.pop()
            string_val = instr[1]
            if string_val:
                print("{}{}".format(string_val, val))
            else:
                print("> {}".format(val))
            state.instr_ix += 1
        
        elif type == 'store':
            # STORE VAR
            var_name = instr[1]
            val = state.stack.pop()
            state.symbol_table[var_name] = val
            state.instr_ix += 1
        
        elif type == 'ask':
            # ask
            prompt = instr[1]
            val = input(prompt)
            try:
                state.stack.append(int(val))
            except:
                # rethrow the exception
                raise ValueError("Error: {} not valid integer".format(val))
            state.instr_ix += 1

        elif type == 'jumpT':
            # JUMPT exp label
            val = state.stack.pop()
            if val:
                state.instr_ix = state.label_table.get(instr[1], None)
            else:
                state.instr_ix += 1

        elif type == 'jumpF':
            # JUMPF exp label
            val = state.stack.pop()
            if not val:
                state.instr_ix = state.label_table.get(instr[1], None)
            else:
                state.instr_ix += 1

        elif type == 'jump':
            # JUMP label
            state.instr_ix = state.label_table.get(instr[1], None)
            
        elif type == 'stop':
            # STOP
            print("{}".format(instr[1]))
            break

        elif type == 'noop':
            # NOOP
            state.instr_ix += 1
            
        elif type == 'dup':
            # INPUT NAME
            val = state.stack.pop()
            state.stack.append(val)
            state.stack.append(val)
            state.instr_ix += 1
            
        elif type == 'add':
            # INPUT NAME
            val1 = state.stack.pop()
            val2 = state.stack.pop()
            state.stack.append(val2 + val1)
            state.instr_ix += 1    
        
        elif type == 'sub':
            # INPUT NAME
            val1 = state.stack.pop()
            val2 = state.stack.pop()
            state.stack.append(val2 - val1)
            state.instr_ix += 1    
       
        elif type == 'mul':
            # INPUT NAME
            val1 = state.stack.pop()
            val2 = state.stack.pop()
            state.stack.append(val2 * val1)
            state.instr_ix += 1    
        
        elif type == 'div':
            # INPUT NAME
            val1 = state.stack.pop()
            val2 = state.stack.pop()
            state.stack.append(val2 // val1)
            state.instr_ix += 1    
       
        elif type == 'equ':
            # INPUT NAME
            val1 = state.stack.pop()
            val2 = state.stack.pop()
            state.stack.append(1 if val2 == val1 else 0)
            state.instr_ix += 1    
        
        elif type == 'leq':
            # INPUT NAME
            val1 = state.stack.pop()
            val2 = state.stack.pop()
            state.stack.append(1 if val2 <= val1 else 0)
            state.instr_ix += 1    
            
        else:
            raise ValueError("Unexpected instruction type: {}".format(p[1]))


#####################################################################################
def eval_exp_tree(node):
    'walk expression tree and evaluate to an integer value'

    # tree nodes are tuples (TYPE, [arg1, arg2,...])
    
    type = node[0]
    
    if type == 'UMINUS':
        # 'UMINUS' exp
        val = eval_exp_tree(node[1])
        return - val
    
    elif type == 'NAME':
        # 'NAME' var_name
        return state.symbol_table.get(node[1],0)

    elif type == 'NUMBER':
        # NUMBER val
        return node[1]

#####################################################################################
def interp(input_stream):
    'driver for our stack machine interpreter.'

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

