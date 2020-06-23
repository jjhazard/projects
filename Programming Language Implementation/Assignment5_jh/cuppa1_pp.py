
#!/usr/bin/env python
# Cuppa1 pretty printer

from sys import stdin
from cuppa1_frontend_gram import parser
from cuppa1_lex import lexer
from cuppa1_state import state
from cuppa1_pp1_walk import walk as pp1_walk
from cuppa1_pp2_walk import walk as pp2_walk
from cuppa1_pp2_walk import init_indent_level

def ppjh(input_stream = None):

    # if no input stream was given read from stdin
    if not input_stream:
        input_stream = stdin.read()

    # initialize the state object and indent level
    state.initialize()
    init_indent_level()

    # build the AST
    parser.parse(input_stream, lexer=lexer)
    
    # walk the AST
    pp1_walk(state.AST,0)

if __name__ == "__main__":
    # execute only if run as a script
    pp()