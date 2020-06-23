# grammar for Cuppa5

from ply import yacc
from clex import tokens, lexer

# set precedence and associativity
# NOTE: all arithmetic operator need to have tokens
#       so that we can put them into the precedence table
precedence = (
              ('left', 'EQ', 'LE'),
              ('left', '+', '-'),
              ('left', '*', '/'),
              ('right', 'UMINUS', 'NOT')
             )

#          | data_type ID opt_init opt_semi


def p_grammar(_):
    '''
    program : funcdec
    funcdec : data_type ID '(' opt_formal_args ')' ';' funcdec
            | INTEGER_TYPE main '(' opt_main_args ')' '{' stmt_list '}' opt_func
    opt_func : data_type ID '(' opt_formal_args ')' stmtlist opt_func
             | empty
    '''
    pass

def p_empty(p):
    'empty :'
    pass

def p_error(t):
    print("Syntax error at '%s'" % t.value)
    
### build the parser
parser = yacc.yacc(debug=True,tabmodule='cparsetab')