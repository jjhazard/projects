
#!/usr/bin/env python
# Cuppa1 compiler

from argparse import ArgumentParser
from cuppa1_lex import lexer
from cuppa1_frontend_gram import parser
from cuppa1_state import state
from cuppa1_cc_codegen import walk as codegen
from cuppa1_cc_fold import walk as fold
from cuppa1_cc_output import output
from cuppa1_cc_output import peephole_opt

def cc(input_stream, opt = False):

    # initialize the state object
    state.initialize()

    # build the AST
    parser.parse(input_stream, lexer=lexer)

    # run the constant fold optimizer
    if opt:
        state.AST = fold(state.AST)

    # generate the list of instruction tuples
    instr_stream = codegen(state.AST) + [('stop',)]

    # run the peephole optimizer
    if opt:
        peephole_opt(instr_stream)

    # output the instruction stream
    bytecode = output(instr_stream)

    return bytecode

if __name__ == "__main__":
    # parse command line args
    aparser = ArgumentParser()
    aparser.add_argument('-O', action='store_true', help='optimization flag')
    aparser.add_argument('input', metavar='input_file', help='cuppa1 input file')
    aparser.add_argument('-o', metavar='output_file', help='exp1bytecode output file')
    
    args = vars(aparser.parse_args())
    
    f = open(args['input'], 'r')
    input_stream = f.read()
    f.close()
    
    # run the compiler
    bytecode = cc(input_stream=input_stream, opt=args['O'])

    if not args['o']:
        print(bytecode)

    else:
        f = open(args['o'], 'w')
        f.write(bytecode)
        f.close()
