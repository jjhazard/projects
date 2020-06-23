from ply import yacc
from cuppa1_lex import tokens, lexer

# set precedence and associativity
# NOTE: all arithmetic operator need to have tokens
#       so that we can put them into the precedence table
precedence = (
              ('left', 'EQ', 'LE'),
              ('left', 'PLUS', 'MINUS'),
              ('left', 'TIMES', 'DIVIDE'),
              ('right', 'UMINUS', 'NOT')
             )


def p_grammar(_):
    '''
    program : stmt_list
    stmt_list : stmt stmt_list
              | empty
    stmt : ID '=' exp opt_semi
         | GET ID opt_semi
         | PUT exp opt_semi
         | WHILE '(' exp ')' stmt
         | IF '(' exp ')' stmt opt_else
         | '{' stmt_list '}'
    opt_else : ELSE stmt
             | empty
             
    opt_semi : ';'
             | empty
    exp : exp PLUS exp
        | exp MINUS exp
        | exp TIMES exp
        | exp DIVIDE exp
        | exp EQ exp
        | exp LE exp
        | INTEGER
        | ID
        | '(' exp ')'
        | MINUS exp %prec UMINUS
        | NOT exp
    '''
    pass

def p_empty(p):
    'empty :'
    pass

def p_error(t):
    print("Syntax error at '%s'" % t.value)

### build the parser
parser = yacc.yacc()

