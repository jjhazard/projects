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
    program : declaration
    declaration : VOID_TYPE ID '(' opt_formal_args ')' ';' declaration
                | data_type ID '(' opt_formal_args ')' ';' declaration
                | data_type ID opt_init ';' declaration
                | INTEGER_TYPE MAIN '(' opt_formal_args ')' '{' stmt_list '}' opt_func
    opt_func : VOID_TYPE ID '(' opt_formal_args ')' stmt
             | data_type ID '(' opt_formal_args ')' stmt
    stmt : VOID_TYPE ID '(' opt_formal_args ')' stmt
         | data_type ID '(' opt_formal_args ')' stmt
         | data_type ID opt_init ';'
         | storable '=' exp ';'
         | storable AE exp ';'
         | storable ME exp ';'
         | storable TE exp ';'
         | storable DE exp ';'
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
    primitive_type : INTEGER_TYPE
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
        | INTEGER
        | DOUBLE
        | STRING
        | storable
        | ID '(' opt_actual_args ')'
        | '(' exp ')'
        | '-' exp %prec UMINUS
        | NOT exp
    storable : ID
             | ID '[' exp ']'
             | INC ID
             | INC ID '[' exp ']'
             | DEC ID
             | DEC ID '[' exp ']'
    '''
    pass

def p_empty(p):
    'empty :'
    pass

def p_error(t):
    print("Syntax error at '%s'" % t.value)
    
### build the parser
parser = yacc.yacc(debug=True,tabmodule='cparsetab')