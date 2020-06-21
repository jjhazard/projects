from cuppa1_state import state
from grammar_stuff import assert_match

# codegen: this is the code generator for our Cuppa1 compiler

# The generated code is a list of Exp1bytecode tuples, that means
# the codegen walker generates lists of tuples for statements but
# strings for expressions.

#########################################################################
# node functions
def seq(node):

    (SEQ, s1, s2) = node
    assert_match(SEQ, 'seq')
    
    stmt = walk(s1)
    lst = walk(s2)

    return stmt + lst

#########################################################################
def nil(node):
    
    (NIL,) = node
    assert_match(NIL, 'nil')
    
    return []
    
#########################################################################
def assign_stmt(node):

    (ASSIGN, name, exp) = node
    assert_match(ASSIGN, 'assign')
    
    exp_code = walk(exp)

    code = [('store', name, exp_code)]
    
    return code

#########################################################################
def get_stmt(node):

    (GET, name) = node
    assert_match(GET, 'get')

    code = [('input', name)]

    return code

#########################################################################
def put_stmt(node):

    (PUT, exp) = node
    assert_match(PUT, 'put')
    
    exp_code = walk(exp)

    code = [('print', exp_code)]

    return code

#########################################################################
def while_stmt(node):
    
    (WHILE, cond, body) = node
    assert_match(WHILE, 'while')
    
    top_label = label()
    bottom_label = label()
    cond_code = walk(cond)
    body_code = walk(body)

    code = [(top_label + ':',)]
    code += [('jumpF', cond_code, bottom_label)]
    code += body_code
    code += [('jump', top_label)]
    code += [(bottom_label + ':',)]
    code += [('noop',)]

    return code

#########################################################################
def if_stmt(node):

    try: # try the if-then pattern
        (IF, cond, s1, (NIL,)) = node
        assert_match(IF, 'if')
        assert_match(NIL, 'nil')
    
    except ValueError: # pattern didn't match
        # try the if-then-else pattern
        (IF, cond, s1, s2) = node
        assert_match(IF, 'if')
        
        else_label = label()
        end_label = label()
        cond_code = walk(cond)
        stmt1_code = walk(s1)
        stmt2_code = walk(s2)

        code = [('jumpF', cond_code, else_label)]
        code += stmt1_code
        code += [('jump', end_label)]
        code += [(else_label + ':',)]
        code += stmt2_code
        code += [(end_label + ':',)]
        code += [('noop',)]

        return code

    else:
        end_label = label();
        cond_code = walk(cond)
        stmt1_code = walk(s1)

        code = [('jumpF', cond_code, end_label)]
        code += stmt1_code
        code += [(end_label + ':',)]
        code += [('noop',)]

        return code

#########################################################################
def block_stmt(node):

    (BLOCK, s) = node
    assert_match(BLOCK, 'block')

    code = walk(s)
    
    return code

#########################################################################
def binop_exp(node):

    (OP, c1, c2) = node
    if OP not in ['+', '-', '*', '/', '==', '<=']:
        raise ValueError("pattern match failed on " + OP)
    
    lcode = walk(c1)
    rcode = walk(c2)

    code = '(' + OP + ' ' + lcode + ' ' + rcode + ')'

    return code

#########################################################################
def integer_exp(node):

    (INTEGER, value) = node
    assert_match(INTEGER, 'integer')

    return str(value)

#########################################################################
def id_exp(node):
    
    (ID, name) = node
    assert_match(ID, 'id')
    
    return name

#########################################################################
def uminus_exp(node):
    
    (UMINUS, e) = node
    assert_match(UMINUS, 'uminus')
    
    code = walk(e)

    return '-' + code

#########################################################################
def not_exp(node):
    
    (NOT, e) = node
    assert_match(NOT, 'not')
    
    code = walk(e)

    return '!' + code

#########################################################################
def paren_exp(node):
    
    (PAREN, exp) = node
    assert_match(PAREN, 'paren')
    
    exp_code = walk(exp)

    return exp_code

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
    '+'       : binop_exp,
    '-'       : binop_exp,
    '*'       : binop_exp,
    '/'       : binop_exp,
    '=='      : binop_exp,
    '<='      : binop_exp

}

#########################################################################
label_id = 0

def label():
    global label_id
    s =  'L' + str(label_id)
    label_id += 1
    return s
