from cuppa1_state import state
from grammar_stuff import assert_match

# pp1: this is the first pass of the Cuppa1 pretty printer that marks
# any defined variable as used if it appears in an expression

#########################################################################
# node functions
#########################################################################
def seq(node, level):

    (SEQ, stmt, stmt_list) = node
    assert_match(SEQ, 'seq')
    
    walk(stmt, level+1)
    walk(stmt_list, level+1)

#########################################################################
def nil(node, levle):
    
    (NIL,) = node
    assert_match(NIL, 'nil')
    
    # do nothing!
    pass

#########################################################################
def assign_stmt(node, level):

    (ASSIGN, name, exp) = node
    assert_match(ASSIGN, 'assign')
    
    walk(exp, level+1)

#########################################################################
def get_stmt(node, level):

    (GET, name) = node
    assert_match(GET, 'get')
    
#########################################################################
def put_stmt(node, level):

    (PUT, exp) = node
    assert_match(PUT, 'put')
    
    walk(exp, level+1)

#########################################################################
def while_stmt(node, level):

    (WHILE, cond, body) = node
    assert_match(WHILE, 'while')
    
    walk(cond, level+1)
    walk(body, level+1)

#########################################################################
def if_stmt(node, level):

    (IF, cond, s1, s2) = node
    assert_match(IF, 'if')
    
    walk(cond, level+1)
    walk(s1, level+1)
    walk(s2, level+1)

#########################################################################
def block_stmt(node, level):

    (BLOCK, stmt_list) = node
    assert_match(BLOCK, 'block')

    walk(stmt_list, level+1)

#########################################################################
def binop_exp(node, level):

    (OP, c1, c2) = node
    if OP not in ['+', '-', '*', '/', '==', '<=']:
        raise ValueError("pattern match failed on " + OP)
    
    walk(c1, level+1)
    walk(c2, level+1)

#########################################################################
def integer_exp(node, level):

    (INTEGER, value) = node
    assert_match(INTEGER, 'integer')

#########################################################################
def id_exp(node, level):
    
    (ID, name) = node
    assert_match(ID, 'id')
    
    # we found a use scenario of a variable, if the variable is defined
    # set it to true
    if name in state.symbol_table:
        state.symbol_table[name] = True

#########################################################################
def uminus_exp(node, level):
    
    (UMINUS, e) = node
    assert_match(UMINUS, 'uminus')
    
    walk(e, level+1)

#########################################################################
def not_exp(node, level):
    
    (NOT, e) = node
    assert_match(NOT, 'not')
    
    walk(e, level+1)

#########################################################################
def paren_exp(node, level):
    
    (PAREN, exp) = node
    assert_match(PAREN, 'paren')
    
    walk(exp, level+1)
#########################################################################
# walk
#########################################################################
def walk(node, level):
    
    node_type = node[0]
    if node_type in dispatch_dict:
        for i in range(level): print("   |",end='')
        print(" ",end='')
        nchildren = len(node) - 1
        print("%s" % node[0], end=' ')
        if nchildren > 0:
            if not isinstance(node[1], tuple): 
                print("%s" % str(node[1]), end='')
        print(' ')
        node_function = dispatch_dict[node_type]
        return node_function(node, level)
    
    else:
        raise ValueError("walk: unknown tree node type: " + node_type)
        
# a dictionary to associate tree nodes with node functions
dispatch_dict = {
    'seq'     : seq,
    'nil'     : nil,
    'assign'  : assign_stmt,
    'get'     : get_stmt,
    'put'     : put_stmt,
    'while'   : while_stmt,
    'if'      : if_stmt,
    'block'   : block_stmt,
    'binop'   : binop_exp,
    'integer' : integer_exp,
    'id'      : id_exp,
    'uminus'  : uminus_exp,
    'not'     : not_exp,
    'paren'   : paren_exp,
    '+'       : binop_exp,
    '-'       : binop_exp,
    '*'       : binop_exp,
    '/'       : binop_exp,
    '=='      : binop_exp,
    '<='      : binop_exp

}
