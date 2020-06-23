# Specification of the Cuppa4 Frontend

from ply import yacc
from clex import tokens, lexer
from cstate import state

#########################################################################
# set precedence and associativity
# NOTE: all operators need to have tokens
#       so that we can put them into the precedence table
precedence = (
              ('left', 'EQ', 'LE'),
              ('left', '+', '-'),
              ('left', '*', '/'),
              ('right', 'UMINUS', 'NOT')
             )

#########################################################################
# grammar rules with embedded actions
#########################################################################
def p_prog(p):
    '''
    program : stmt_list
    '''
    state.AST = p[1]

#########################################################################
def p_stmt_list(p):
    '''
    stmt_list : stmt stmt_list
              | empty
    '''
    if (len(p) == 3):
        p[0] = ('seq', p[1], p[2])
    else:
        p[0] = p[1]

#########################################################################
#    stmt : VOID_TYPE ID '(' opt_formal_args ')' stmt
#         | data_type ID '(' opt_formal_args ')' stmt
#         | data_type ID opt_init semi
#         | storable '=' exp semi
#         | GET storable semi
#         | PUT exp semi
#         | ID '(' opt_actual_args ')' semi
#         | RETURN opt_exp semi
#         | WHILE '(' exp ')' stmt
#         | IF '(' exp ')' stmt opt_else
#         | '{' stmt_list '}'

def p_stmt_1(p):
    '''
    stmt : VOID_TYPE ID '(' opt_formal_args ')' stmt
    '''
    p[0] = ('fundecl', p[2], p[1], p[4], p[6])

def p_stmt_2(p):
    '''
    stmt : data_type ID '(' opt_formal_args ')' stmt
    '''
    p[0] = ('fundecl', p[2], p[1], p[4], p[6])

def p_stmt_3(p):
    '''
    stmt : data_type ID opt_init ';'
    '''
    p[0] = ('decl', p[2], p[1], p[3])

def p_stmt_4(p):
    '''
    stmt : operable '=' exp ';'
         | operable AE exp ';'
         | operable ME exp ';'
         | operable TE exp ';'
         | operable DE exp ';'
    '''
    if p[2] == '=':
        p[0] = ('assign', p[1], p[3])
    elif p[2] == '+=':
        p[0] = ('assign', p[1], ('plus', p[1], p[3]))
    elif p[2] == '-=':
        p[0] = ('assign', p[1], ('minus', p[1], p[3]))
    elif p[2] == '*=':
        p[0] = ('assign', p[1], ('times', p[1], p[3]))
    elif p[2] == '/=':
        p[0] = ('assign', p[1], ('divide', p[1], p[3]))
    else:
        raise ValueError("Syntax error at ", p[2])

def p_stmt_5(p):
    '''
    stmt : PRINT '(' exp ')' ';'
    '''
    p[0] = ('print', p[3])

def p_stmt_6(p):
    '''
    stmt : ID '(' opt_actual_args ')' ';'
    '''
    p[0] = ('callstmt', p[1], p[3])

def p_stmt_7(p):
    '''
    stmt : RETURN opt_exp ';'
    '''
    p[0] = ('return', p[2])

def p_stmt_8(p):
    '''
    stmt : WHILE '(' exp ')' stmt
    '''
    p[0] = ('while', p[3], p[5])

def p_stmt_9(p):
    '''
    stmt : FOR '(' data_type ID '=' exp ';' exp ';' exp ')' stmt
    '''
    p[0] = ('for', p[3], p[4], p[6], p[8], p[10], p[12])
    
def p_stmt_10(p):
    '''
    stmt : FOR '(' ID '=' exp ';' exp ';' exp ')' stmt
    '''
    p[0] = ('for', ('nil',), p[3], p[5], p[7], p[9], p[11])
    
def p_stmt_11(p):
    '''
    stmt : IF '(' exp ')' stmt opt_else
    '''
    p[0] = ('if', p[3], p[5], p[6])

def p_stmt_12(p):
    '''
    stmt : '{' stmt_list '}'
    '''
    p[0] = ('block', p[2])

#########################################################################
#    data_type : primitive_type
#              | primitive_type '[' INTEGER ']'

def p_data_type_1(p):
    '''
    data_type : primitive_type
    '''
    p[0] = p[1]

def p_data_type_2(p):
    '''
    data_type : primitive_type '[' INTEGER ']'
    '''
    p[0] = ('array-type', int(p[3]), p[1])

#########################################################################
#    primitive_type : INTEGER_TYPE
#                   | DOUBLE_TYPE
#                   | STRING_TYPE

def p_primitive_type_1(p):
    '''
    primitive_type : CHAR_TYPE
    '''
    p[0] = ('char',)

def p_primitive_type_2(p):
    '''
    primitive_type : INTEGER_TYPE
    '''
    p[0] = ('integer',)

def p_primitive_type_3(p):
    '''
    primitive_type : DOUBLE_TYPE
    '''
    p[0] = ('double',)

def p_primitive_type_4(p):
    '''
    primitive_type :  STRING_TYPE
    '''
    p[0] = ('string',)

#########################################################################
def p_opt_formal_args(p):
    '''
    opt_formal_args : formal_args
                    | empty
    '''
    p[0] = p[1]

#########################################################################
#    formal_args : data_type ID ',' formal_args
#                | data_type ID

def p_formal_args_1(p):
    '''
    formal_args : data_type ID ',' formal_args
    '''
    p[0] = ('seq', ('formalarg', p[1], p[2]), p[4])

def p_formal_args_2(p):
    '''
    formal_args : data_type ID
    '''
    p[0] = ('seq', ('formalarg', p[1], p[2]), ('nil',))

#########################################################################
#    opt_init : '=' exp
#             | '=' '{' exp_list '}'
#             | empty

def p_opt_init_1(p):
    '''
    opt_init : '=' exp
    '''
    p[0] = p[2]

def p_opt_init_2(p):
    '''
    opt_init : '=' '{' exp_list '}'
    '''
    p[0] = p[3]

def p_opt_init_3(p):
    '''
    opt_init : empty
    '''
    p[0] = p[1]

#########################################################################
#    exp_list : exp ',' exp_list
#             | exp

def p_exp_list_1(p):
    '''
    exp_list : exp ',' exp_list
    '''
    p[0] = ('seq', p[1], p[3])

def p_exp_list_2(p):
    '''
    exp_list : exp
    '''
    p[0] = ('seq', p[1], ('nil',))

#########################################################################
def p_opt_actual_args(p):
    '''
    opt_actual_args : actual_args
                    | empty
    '''
    p[0] = p[1]

#########################################################################
def p_actual_args(p):
    '''
    actual_args : exp ',' actual_args
                | exp
    '''
    if (len(p) == 4):
        p[0] = ('seq', p[1], p[3])
    else:
        p[0] = ('seq', p[1], ('nil',))

#########################################################################
def p_opt_exp(p):
    '''
    opt_exp : exp
            | empty
    '''
    p[0] = p[1]

#########################################################################
def p_opt_else(p):
    '''
    opt_else : ELSE stmt
             | empty
    '''
    if p[1] == 'else':
        p[0] = p[2]
    else:
        p[0] = p[1]

#########################################################################
#    exp : exp PLUS exp
#        | exp MINUS exp
#        | exp TIMES exp
#        | exp DIVIDE exp
#        | exp EQ exp
#        | exp LE exp
#        | INTEGER
#        | FLOAT
#        | STRING
#        | storable
#        | ID '(' opt_actual_args ')'
#        | '(' exp ')'
#        | MINUS exp %prec UMINUS
#        | NOT exp

def p_exp_1(p):
    '''
    exp : exp '+' exp
    '''
    p[0] = ('plus', p[1], p[3])
    
def p_exp_2(p):
    '''
    exp : exp '-' exp
    '''
    p[0] = ('minus', p[1], p[3])
    
def p_exp_3(p):
    '''
    exp : exp '*' exp
    '''
    p[0] = ('times', p[1], p[3])
    
def p_exp_4(p):
    '''
    exp : exp '/' exp
    '''
    p[0] = ('divide', p[1], p[3])
    
def p_exp_5(p):
    '''
    exp : exp EQ exp
        | exp LE exp
        | exp GE exp
        | exp LT exp
        | exp GT exp
        | exp NE exp
        | exp MOD exp
        | exp AND exp
        | exp OR exp
    '''
    p[0] = (p[2], p[1], p[3])

def p_exp_6(p):
    '''
    exp : CHAR
    '''
    p[0] = ('val', ('char',), p[1])

def p_exp_7(p):
    '''
    exp : INTEGER
    '''
    p[0] = ('val', ('integer',), int(p[1]))

def p_exp_8(p):
    '''
    exp : DOUBLE
    '''
    p[0] = ('val', ('double',), float(p[1]))

def p_exp_9(p):
    '''
    exp : STRING
    '''
    p[0] = ('val', ('string',), str(p[1]))

def p_exp_10(p):
    '''
    exp : operable
    '''
    p[0] = p[1]

def p_exp_11(p):
    '''
    exp : ID '(' opt_actual_args ')'
    '''
    p[0] = ('callexp', p[1], p[3])

def p_exp_12(p):
    '''
    exp : '(' exp ')'
    '''
    p[0] = p[2]

def p_exp_13(p):
    '''
    exp : '-' exp %prec UMINUS
    '''
    p[0] = ('uminus', p[2])

def p_exp_14(p):
    '''
    exp : NOT exp
    '''
    p[0] = ('not', p[2])

#########################################################################

def p_operable_1(p):
    '''
    operable : storable
    '''
    p[0] = ('oper', p[1], ('nil',))
    
def p_operable_2(p):  
    '''
    operable : INC storable
    operable : DEC storable
    '''
    if p[1] == '++':
        p[0] = ('oper', p[2], 'preinc')
    elif p[1] == '--':
        p[0] = ('oper', p[2], 'predec')
    else:
        raise ValueError(p[1], "is not a prefix")

def p_operable_3(p):
    '''
    operable : storable INC
    operable : storable DEC
    '''
    if p[2] == '++':
        p[0] = ('oper', p[1], 'postinc')
    elif p[2] == '--':
        p[0] = ('oper', p[1], 'postdec')
    else:
        raise ValueError(p[2], "is not a postfix")
    
#########################################################################
#    storable : ID
#             | ID '[' exp ']'

def p_storable_1(p):
    '''
    storable : '*' storable
    '''
    p[0] = ('ref', p[2])

def p_storable_2(p):
    '''
    storable : ID
    '''
    p[0] = ('id', p[1])
    
def p_storable_3(p):
    '''
    storable : ID '[' exp ']'
    '''
    p[0] = ('array-access', p[1], p[3])
    
#########################################################################
def p_empty(p):
    '''
    empty :
    '''
    p[0] = ('nil',)

#########################################################################
def p_error(t):
    print("Syntax error at '%s' (likely forgot semicolon)" % t.value)
    raise ValueError(t.value)

#########################################################################
# build the parser
#########################################################################
parser = yacc.yacc(debug=False,tabmodule='cparsetab')