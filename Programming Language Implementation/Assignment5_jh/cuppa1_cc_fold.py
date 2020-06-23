
from cuppa1_state import state
from grammar_stuff import assert_match

# fold: this is simple constant folder for our Cuppa1 compiler

# it is a tree rewriter so at every node function we have to construct
# a new node because we are not allowed to update tuples in Python
# and we are not sure if the tree below us has changed.

#########################################################################
# node functions
def seq(node):

    (SEQ, s1, s2) = node
    assert_match(SEQ, 'seq')
    
    stmt_tree = walk(s1)
    list_tree = walk(s2)

    return ('seq', stmt_tree, list_tree)

#########################################################################
def nil(node):
    
    (NIL,) = node
    assert_match(NIL, 'nil')
    
    return node # nil nodes are immutable
    
#########################################################################
def assign_stmt(node):

    (ASSIGN, name, exp) = node
    assert_match(ASSIGN, 'assign')
    
    exp_tree = walk(exp)
    
    return ('assign', name, exp_tree)

#########################################################################
def get_stmt(node):

    (GET, name) = node
    assert_match(GET, 'get')

    return node # nothing to be rewritten in get nodes

#########################################################################
def put_stmt(node):

    (PUT, exp) = node
    assert_match(PUT, 'put')
    
    exp_tree = walk(exp)

    return ('put', exp_tree)

#########################################################################
def while_stmt(node):
    global indent_level
    
    (WHILE, cond, body) = node
    assert_match(WHILE, 'while')
    
    cond_tree = walk(cond)
    body_tree = walk(body)

    return ('while', cond_tree, body_tree)

#########################################################################
def if_stmt(node):
    global indent_level

    try: # try the if-then pattern
        (IF, cond, s1, (NIL,)) = node
        assert_match(IF, 'if')
        assert_match(NIL, 'nil')
    
    except ValueError: # pattern didn't match
        # try the if-then-else pattern
        (IF, cond, s1, s2) = node
        assert_match(IF, 'if')
        
        cond_tree = walk(cond)
        stmt1_tree = walk(s1)
        stmt2_tree = walk(s2)
        
        return ('if', cond_tree, stmt1_tree, stmt2_tree)

    else:
        cond_tree = walk(cond)
        stmt1_tree = walk(s1)

        return ('if', cond_tree, stmt1_tree, ('nil',))

#########################################################################
def block_stmt(node):
    global indent_level
    adjust_level = False

    (BLOCK, s) = node
    assert_match(BLOCK, 'block')

    stmt_tree = walk(s)
    
    return ('block', stmt_tree)

#########################################################################
# Expressions -- try to fold constants
#########################################################################
def plus_exp(node):

    (OP, c1, c2) = node
    assert_match(OP, '+')
    
    ltree = walk(c1)
    rtree = walk(c2)

    # if the children are constants -- fold!
    if ltree[0] == 'integer' and rtree[0] == 'integer':
        return ('integer', ltree[1] + rtree[1])
    
    else:
        return ('+', ltree, rtree)

#########################################################################
def minus_exp(node):

    (OP, c1, c2) = node
    assert_match(OP, '-')
    
    ltree = walk(c1)
    rtree = walk(c2)

    # if the children are constants -- fold!
    if ltree[0] == 'integer' and rtree[0] == 'integer':
        return ('integer', ltree[1] - rtree[1])
    
    else:
        return ('-', ltree, rtree)

#########################################################################
def mult_exp(node):

    (OP, c1, c2) = node
    assert_match(OP, '*')
    
    ltree = walk(c1)
    rtree = walk(c2)

    # if the children are constants -- fold!
    if ltree[0] == 'integer' and rtree[0] == 'integer':
        return ('integer', ltree[1] * rtree[1])
    
    else:
        return ('*', ltree, rtree)

#########################################################################
def div_exp(node):

    (OP, c1, c2) = node
    assert_match(OP, '/')
    
    ltree = walk(c1)
    rtree = walk(c2)

    # if the children are constants -- fold!
    if ltree[0] == 'integer' and rtree[0] == 'integer':
        return ('integer', ltree[1] // rtree[1])
    
    else:
        return ('/', ltree, rtree)

#########################################################################
def eq_exp(node):

    (OP, c1, c2) = node
    assert_match(OP, '==')
    
    ltree = walk(c1)
    rtree = walk(c2)

    # if the children are constants -- fold!
    if ltree[0] == 'integer' and rtree[0] == 'integer':
        return ('integer', 1 if ltree[1] == rtree[1] else 0)
    
    else:
        return ('==', ltree, rtree)

#########################################################################
def le_exp(node):

    (OP, c1, c2) = node
    assert_match(OP, '<=')
    
    ltree = walk(c1)
    rtree = walk(c2)

    # if the children are constants -- fold!
    if ltree[0] == 'integer' and rtree[0] == 'integer':
        return ('integer', 1 if ltree[1] <= rtree[1] else 0)
    
    else:
        return ('<=', ltree, rtree)

#########################################################################
def integer_exp(node):

    (INTEGER, value) = node
    assert_match(INTEGER, 'integer')

    return node # integer nodes are immutable

#########################################################################
def id_exp(node):
    
    (ID, name) = node
    assert_match(ID, 'id')
    
    return node # id nodes are immutable

#########################################################################
def uminus_exp(node):
    
    (UMINUS, e) = node
    assert_match(UMINUS, 'uminus')
    
    exp_tree = walk(e)

    return ('uminus', exp_tree)

#########################################################################
def not_exp(node):
    
    (NOT, e) = node
    assert_match(NOT, 'not')
    
    exp_tree = walk(e)

    return ('not', exp_tree)

#########################################################################
def paren_exp(node):
    
    (PAREN, exp) = node
    assert_match(PAREN, 'paren')
    
    exp_tree = walk(exp)

    return ('paren', exp_tree)

#########################################################################
# walk
#########################################################################
def walk(node):
    node_type = node[0]
    
    if node_type in dispatch_dict:
        node_function = dispatch_dict[node_type]
        return node_function(node)
    
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
    'integer' : integer_exp,
    'id'      : id_exp,
    'uminus'  : uminus_exp,
    'not'     : not_exp,
    'paren'   : paren_exp,
    '+'       : plus_exp,
    '-'       : minus_exp,
    '*'       : mult_exp,
    '/'       : div_exp,
    '=='      : eq_exp,
    '<='      : le_exp

}
