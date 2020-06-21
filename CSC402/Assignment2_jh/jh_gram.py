from ply import yacc
from jh_lex import tokens, lexer

def p_grammar(_):
    """
    prog : sexp prog
         | empty
              
    sexp : '(' exp ')'
         | var
         | num
         
    exp : '+' sexp sexp
        | '-' sexp sexp
        | '*' sexp sexp
        | '/' sexp sexp
        | 's' var sexp
        | 'p' sexp
        
    var : 'x' 
        | 'y' 
        | 'z'
        
    num : '0' 
        | '1' 
        | '2' 
        | '3' 
        | '4' 
        | '5' 
        | '6' 
        | '7' 
        | '8' 
        | '9'
    """
    pass

def p_empty(p):
    'empty :'
    pass

def p_error(t):
    print("Syntax error at '%s'" % t.value)

parser = yacc.yacc(debug=False,tabmodule='exp0parsetab')