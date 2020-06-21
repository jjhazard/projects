from cuppa1_ast_gram import parser
from cuppa1_state import state
from grammar_stuff import dump_AST
from cuppa1_examples import *
from cuppa1_interp_visitor import dispatch as interp_dispatch

# factorial
parser.parse(fact)
#dump_AST(state.AST)
#interp_dispatch(state.AST)

from cuppa1_pp1_visitor import dispatch as pp1_dispatch
from cuppa1_pp2_visitor import dispatch as pp2_dispatch

pp1_dispatch(state.AST)
code = pp2_dispatch(state.AST)

print(code)
