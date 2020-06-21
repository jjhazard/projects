# A tree walker to interpret Cuppa1 programs

from ubasic_state import state
from grammar_stuff import assert_match
import sys

#########################################################################
# node functions
#########################################################################
def seq(node):
    
    (SEQ, stmt, stmt_list) = node
    assert_match(SEQ, 'seq')
    
    walk(stmt)
    walk(stmt_list)

#########################################################################
def nil(node):
    
    (NIL,) = node
    assert_match(NIL, 'nil')
    
    # do nothing!
    pass

#########################################################################
def print_stmt(node):
    
    try:
        (PRINT, value, (NIL,)) = node
        assert_match(PRINT, 'print')
        assert_match(NIL, 'nil')
        x = walk(value)
        print(x)

    except ValueError: 
        (PRINT, value, LIST) = node
        assert_match(PRINT, 'print')
        x = str(walk(value))
        y = walk(LIST)
        print(x + y)

#########################################################################
def assign_stmt(node):

    (ASSIGN, name, exp) = node
    assert_match(ASSIGN, 'assign')
    
    value = walk(exp)
    state.symbol_table[name] = value

#########################################################################
def input_stmt(node):
    try: # try the if-then pattern
        (INPUT, name) = node
        assert_match(INPUT, 'input')
        s = input("Value for " + name + '? ')

    except ValueError: # if-then pattern didn't match
        (INPUT, STRING, name) = node
        assert_match(INPUT, 'input')
        s = input(STRING)
    
    try:
        value = int(s)
    except ValueError:
        raise ValueError("expected an integer value for " + name)
    
    state.symbol_table[name] = value

#########################################################################
def put_stmt(node):

    (PUT, exp) = node
    assert_match(PUT, 'put')
    
    value = walk(exp)
    print("> {}".format(value))
    
#########################################################################
def end_stmt(node):

    (END,) = node
    assert_match(END, 'end')
    
    sys.exit()
    
#########################################################################
def while_stmt(node):

    (WHILE, cond, body) = node
    assert_match(WHILE, 'while')
    
    value = walk(cond)
    while value != 0:
        walk(body)
        value = walk(cond)

#########################################################################
def if_stmt(node):
    
    try: # try the if-then pattern
        (IF, cond, then_stmt, (NIL,)) = node
        assert_match(IF, 'if')
        assert_match(NIL, 'nil')

    except ValueError: # if-then pattern didn't match
        (IF, cond, then_stmt, else_stmt) = node
        assert_match(IF, 'if')

        value = walk(cond)
        if value != 0:
            walk(then_stmt)
        else:
            walk(else_stmt)
        return
 
    else: # if-then pattern matched
        value = walk(cond)
        if value != 0:
            walk(then_stmt)
        return

#########################################################################
def for_stmt(node):
    # FOR ID '=' exp TO exp opt_step stmt_list NEXT ID
    try: # try the for-step pattern
        (FOR, name, base, cond, (NIL,), body) = node
        assert_match(FOR, 'for')
        assert_match(NIL, 'nil')
        step = 1
    except ValueError: 
        (FOR, name, base, cond, step, body) = node
        assert_match(FOR, 'for')
        step = walk(step)
        
    value = walk(base)
    state.symbol_table[name] = value
    end = walk(cond)
    
    if step > 0:
        while value <= end:
            walk(body)
            walk(('assign', name, ('+', ('id', name), ('integer', step))))
            value+=step
    else:
        while value >= end:
            walk(body)
            walk(('assign', name, ('+', ('id', name), ('integer', step))))
            value+=step
    
#########################################################################
def block_stmt(node):
    
    (BLOCK, stmt_list) = node
    assert_match(BLOCK, 'block')
    
    walk(stmt_list)
    
#########################################################################
def value_list(node):
    try:
        (VALUE_LIST, value, (NIL,)) = node
        assert_match(VALUE_LIST, 'vallist')
        assert_match(NIL, 'nil')
        x = walk(value)
        return str(x)

    except ValueError: 
        (VALUE_LIST, value, LIST) = node
        assert_match(VALUE_LIST, 'vallist')
        x = str(walk(value))
        y = walk(LIST)
        return x + y
    
#########################################################################
def string_value(node):
    
    (STRING, sentence) = node
    assert_match(STRING, 'string')
    
    return sentence

#########################################################################
def plus_exp(node):
    
    (PLUS,c1,c2) = node
    assert_match(PLUS, '+')
    
    v1 = walk(c1)
    v2 = walk(c2)
    
    return v1 + v2

#########################################################################
def minus_exp(node):
    
    (MINUS,c1,c2) = node
    assert_match(MINUS, '-')
    
    v1 = walk(c1)
    v2 = walk(c2)
    
    return v1 - v2

#########################################################################
def times_exp(node):
    
    (TIMES,c1,c2) = node
    assert_match(TIMES, '*')
    
    v1 = walk(c1)
    v2 = walk(c2)
    
    return v1 * v2

#########################################################################
def divide_exp(node):
    
    (DIVIDE,c1,c2) = node
    assert_match(DIVIDE, '/')
    
    v1 = walk(c1)
    v2 = walk(c2)
    
    return v1 // v2

#########################################################################
def eq_exp(node):
    
    (EQ,c1,c2) = node
    assert_match(EQ, '==')
    
    v1 = walk(c1)
    v2 = walk(c2)
    
    return 1 if v1 == v2 else 0

#########################################################################
def le_exp(node):
    
    (LE,c1,c2) = node
    assert_match(LE, '<=')
    
    v1 = walk(c1)
    v2 = walk(c2)
    
    return 1 if v1 <= v2 else 0

#########################################################################
def and_exp(node):
    
    (AND,c1,c2) = node
    assert_match(AND, '&')
    
    v1 = walk(c1)
    v2 = walk(c2)
    
    return 0 if v1 == 0 or v2 == 0 else 1

#########################################################################
def or_exp(node):
    
    (OR,c1,c2) = node
    assert_match(OR, '|')
    
    v1 = walk(c1)
    v2 = walk(c2)
    
    return 1 if v1 != 0 or v2 != 0 else 0

#########################################################################
def integer_exp(node):

    (INTEGER, value) = node
    assert_match(INTEGER, 'integer')
    
    return value

#########################################################################
def id_exp(node):
    
    (ID, name) = node
    assert_match(ID, 'id')
    if name not in state.symbol_table:
        state.symbol_table[name] = 0
    elif state.symbol_table.get(name) == None:
        state.symbol_table[name] = 0
    return state.symbol_table.get(name)

#########################################################################
def uminus_exp(node):
    
    (UMINUS, exp) = node
    assert_match(UMINUS, 'uminus')
    
    val = walk(exp)
    return - val

#########################################################################
def not_exp(node):
    
    (NOT, exp) = node
    assert_match(NOT, 'not')
    
    val = walk(exp)
    return 0 if val != 0 else 1

#########################################################################
def paren_exp(node):
    
    (PAREN, exp) = node
    assert_match(PAREN, 'paren')
    
    # return the value of the parenthesized expression
    return walk(exp)

#########################################################################
# walk
#########################################################################
def walk(node):
    # node format: (TYPE, [child1[, child2[, ...]]])
    type = node[0]
    
    if type in dispatch_dict:
        node_function = dispatch_dict[type]
        return node_function(node)
    else:
        raise ValueError("walk: unknown tree node type: " + type)

# a dictionary to associate tree nodes with node functions
dispatch_dict = {
    'seq'     : seq,
    'nil'     : nil,
    'print'   : print_stmt,
    'assign'  : assign_stmt,
    'input'   : input_stmt,
    'for'     : for_stmt,
    'put'     : put_stmt,
    'end'     : end_stmt,
    'while'   : while_stmt,
    'if'      : if_stmt,
    'block'   : block_stmt,
    'vallist' : value_list,
    'string'  : string_value,
    'integer' : integer_exp,
    'id'      : id_exp,
    'paren'   : paren_exp,
    '+'       : plus_exp,
    '-'       : minus_exp,
    '*'       : times_exp,
    '/'       : divide_exp,
    '=='      : eq_exp,
    '<='      : le_exp,
    '&'       : and_exp,
    '|'       : or_exp,
    'uminus'  : uminus_exp,
    'not'     : not_exp
}
