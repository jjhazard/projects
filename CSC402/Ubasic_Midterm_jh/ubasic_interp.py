#!/usr/bin/env python
# Cuppa1 interpreter

from argparse import ArgumentParser
from ubasic_lex import lexer
from ubasic_frontend_gram import parser
from ubasic_state import state
from ubasic_interp_walk import walk

def interp(input_stream):

    # initialize the state object
    state.initialize()

    # build the AST
    parser.parse(input_stream, lexer=lexer)

    # walk the AST
    walk(state.AST)

if __name__ == "__main__":
    # parse command line args
    aparser = ArgumentParser()
    aparser.add_argument('input')

    args = vars(aparser.parse_args())

    f = open(args['input'], 'r')
    input_stream = f.read()
    f.close()

    # execute interpreter
    interp(input_stream=input_stream)
