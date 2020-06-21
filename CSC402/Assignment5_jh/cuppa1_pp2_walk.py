
from cuppa1_state import state
from grammar_stuff import assert_match

# pp2: this is the second pass of the Cuppa1 pretty printer that
# generates the output together with the warning

indent_level = 0

#########################################################################
# node functions
#########################################################################
def seq(node):

    (SEQ, s1, s2) = node
    assert_match(SEQ, 'seq')
    
    stmt = walk(s1)
    list = walk(s2)

    return stmt + list

#########################################################################
def nil(node):
    
    (NIL,) = node
    assert_match(NIL, 'nil')
    
    return ''
    
#########################################################################
def assign_stmt(node):

    (ASSIGN, name, exp) = node
    assert_match(ASSIGN, 'assign')
    
    exp_code = walk(exp)

    code = indent() + name + ' = ' + exp_code
    
    if not state.symbol_table[name]:
        code += ' // *** '+ name + ' is not used ***'

    code += '\n'
    return code

#########################################################################
def get_stmt(node):

    (GET, name) = node
    assert_match(GET, 'get')

    code = indent() + 'get ' + name
    
    if not state.symbol_table[name]:
        code += ' // *** '+ name + ' is not used ***'

    code += '\n'
    return code

#########################################################################
def put_stmt(node):

    (PUT, exp) = node
    assert_match(PUT, 'put')
    
    exp_code = walk(exp)

    code = indent() + 'put ' + exp_code + '\n'
    return code

#########################################################################
def while_stmt(node):
    global indent_level
    
    (WHILE, cond, body) = node
    assert_match(WHILE, 'while')
    
    cond_code = walk(cond)

    indent_level += 1
    body_code = walk(body)
    indent_level -= 1

    code = indent() + 'while (' + cond_code + ')\n' + body_code

    return code

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
        
        cond_code = walk(cond)

        indent_level += 1
        stmt1_code = walk(s1)
        stmt2_code = walk(s2)
        indent_level -= 1

        code = indent() + 'if (' + cond_code + ')\n' + stmt1_code
        code += indent() + 'else\n' + stmt2_code
        return code

    else:
        cond_code = walk(cond)

        indent_level += 1
        stmt1_code = walk(s1)
        indent_level -= 1

        code = indent() + 'if (' + cond_code + ')\n' + stmt1_code
        return code
    


#########################################################################
def block_stmt(node):
    global indent_level
    adjust_level = False

    (BLOCK, s) = node
    assert_match(BLOCK, 'block')

    if indent_level > 0:
        indent_level -= 1
        adjust_level = True

    indent_level += 1
    code = walk(s)
    indent_level -= 1
    
    code = indent() + '{\n' + code + indent() + '}\n'

    if adjust_level:
        indent_level += 1
    
    return code

#########################################################################
def binop_exp(node):

    (OP, c1, c2) = node
    if OP not in ['+', '-', '*', '/', '==', '<=']:
        raise ValueError("pattern match failed on " + OP)
    
    lcode = walk(c1)
    rcode = walk(c2)

    code = lcode + ' ' + OP + ' ' + rcode

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

    return 'not ' + code

#########################################################################
def paren_exp(node):
    
    (PAREN, exp) = node
    assert_match(PAREN, 'paren')
    
    exp_code = walk(exp)

    return '(' + exp_code + ')'

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


#########################################################################
def indent():
    s = ''
    for i in range(indent_level):
        s += '   '
    return s

#########################################################################
def init_indent_level():
    global indent_level
    indent_level = 0
