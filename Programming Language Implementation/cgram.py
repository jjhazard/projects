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

#          | data_type ID opt_init ';'


def p_grammar(_):
    '''
    program : stmt_list
    stmt_list : stmt stmt_list
              | empty
    stmt : VOID_TYPE ID '(' opt_formal_args ')' stmt
         | data_type ID '(' opt_formal_args ')' stmt
         | data_type ID opt_init ';'
         | operable '=' exp ';'
         | operable AE exp ';'
         | operable ME exp ';'
         | operable TE exp ';'
         | operable DE exp ';'
         | PRINT '(' exp ')' ';'
         | ID '(' opt_actual_args ')' ';'
         | RETURN opt_exp ';'
         | WHILE '(' exp ')' stmt
         | FOR '(' data_type ID '=' exp ';' exp ';' exp ')' stmt
         | FOR '(' ID '=' exp ';' exp ';' exp ')' stmt
         | IF '(' exp ')' stmt opt_else
         | '{' stmt_list '}'
    data_type : primitive_type
              | primitive_type '[' INTEGER ']'
    primitive_type : CHAR_TYPE
                   | INTEGER_TYPE
                   | DOUBLE_TYPE
                   | STRING_TYPE
    opt_formal_args : formal_args
                    | empty
    formal_args : data_type ID ',' formal_args
                | data_type ID
    opt_init : '=' exp
             | '=' '{' exp_list '}'
             | empty
    exp_list : exp ',' exp_list
             | exp
    opt_actual_args : actual_args
                    | empty
    actual_args : exp ',' actual_args
                | exp
    opt_exp : exp
            | empty
    opt_else : ELSE stmt
             | empty
    exp : exp '*' exp
        | exp '-' exp
        | exp '*' exp
        | exp '/' exp
        | exp EQ exp
        | exp LE exp
        | exp GE exp
        | exp LT exp
        | exp GT exp
        | exp NE exp
        | exp MOD exp
        | exp AND exp
        | exp OR exp
        | CHAR
        | INTEGER
        | DOUBLE
        | STRING
        | operable
        | ID '(' opt_actual_args ')'
        | '(' exp ')'
        | '-' exp %prec UMINUS
        | NOT exp
    operable : storable
             | INC storable
             | DEC storable
             | storable INC
             | storable DEC
    storable : ID
             | ID '[' exp ']'
    '''
    pass

def p_empty(p):
    'empty :'
    pass

def p_error(t):
    print("Syntax error at '%s'" % t.value)
    
### build the parser
parser = yacc.yacc(debug=True,tabmodule='cparsetab')