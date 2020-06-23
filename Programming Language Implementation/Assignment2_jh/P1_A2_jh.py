
'''
Recursive descent grammar for Assignment 2:
    prog : {(,x,y,z,1,2,3,4,5,6,7,8,9} sexp prog
         | {""} ""
              
    sexp : {(} '(' exp ')'
         | {x,y,z} var
         | {1,2,3,4,5,6,7,8,9} num
         
    exp : {+} ’+’ sexp sexp
        | {-} ’-’ sexp sexp
        | {*} ’*’ sexp sexp
        | {/} ’/’ sexp sexp
        | {s} ’s’ var sexp
        | {p} ’p’ sexp
        
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
'''
from grammar_stuff import InputStream

I = None

def set_stream(input_stream):
    global I
    I = input_stream

def set_count():
    global count
    count = 0
    
def prog():
    global count
    sym = I.pointer()
    if sym == '(' or sym in ['x','y','z'] or sym in ['1','2','3','4','5','6','7','8','9']:
        sexp()
        prog()
    elif sym == "":
        print(count)
        pass
    else:
        raise SyntaxError('unexpected symbol {} while parsing'.format(sym))

def sexp():
    sym = I.pointer()
    if sym == '(':
        I.next()
        exp()
        I.match(')')
    elif sym in ['x', 'y', 'z']:
        var()
    elif sym in ['0', '1', '2', '3', '4', '5', '6','7', '8', '9']:
        num()
    else:
        raise SyntaxError('unexpected symbol {} while parsing'.format(sym))

def exp():
    global count
    sym = I.pointer()
    if sym == '+':
        I.next()
        sexp()
        sexp()
    elif sym == '-':
        I.next()
        sexp()
        sexp()
    elif sym == '*':
        I.next()
        sexp()
        sexp()
    elif sym == '/':
        I.next()
        sexp()
        sexp()
    elif sym == 's':
        I.next()
        var()
        sexp()
    elif sym == 'p':
        global count
        count += 1
        I.next()
        sexp()
    else:
        raise SyntaxError('unexpected symbol {} while parsing'.format(sym))

def var():
    sym = I.pointer()
    if sym == 'x':
        I.next()
    elif sym == 'y':
        I.next()
    elif sym == 'z':
        I.next()
    else:
        raise SyntaxError('unexpected symbol {} while parsing'.format(sym))

def num():
    sym = I.pointer()
    if sym in ['0', '1', '2', '3', '4', '5', '6','7', '8', '9']:
        I.next()
    else:
        raise SyntaxError('unexpected symbol {} while parsing'.format(sym))
    
# example test case

#I = InputStream(['(', 's', 'x', '1', ')'])
#prog()
