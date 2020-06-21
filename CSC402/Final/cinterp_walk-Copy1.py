# A tree walker to interpret Cuppa5 programs

from cstate import state
from grammar_stuff import assert_match
from ctype_promotion import promote
from ctype_promotion import safe_assign
from ctype_promotion import conversion_fun
from ctype_promotion import is_primitive
from ctype_promotion import cast_string

#########################################################################
# Use the exception mechanism to return values from function calls

class ReturnValue(Exception):

    def __init__(self, value):
        self.value = value

    def __str__(self):
        return(repr(self.value))


#########################################################################
def compute_init_vals(list):
    if list[0] == 'nil':
        return ('nil',)
    elif list[0] == 'seq':
        (SEQ, init_val_tree, rest) = list
        (t,v) = walk(init_val_tree)
        return ('seq', (t,v), compute_init_vals(rest))
    else:
        return walk(list)

#########################################################################
def len_seq(seq_list):

    if seq_list[0] == 'nil':
        return 0

    elif seq_list[0] == 'seq':
        # unpack the seq node
        (SEQ, p1, p2) = seq_list

        return 1 + len_seq(p2)

    else:
            raise ValueError("unknown node type: {}".format(seq_list[0]))

#########################################################################
def eval_actual_args(args):

    if args[0] == 'nil':
        return ('nil',)

    elif args[0] == 'seq':
        # unpack the seq node
        (SEQ, p1, p2) = args

        (data_type, val) = walk(p1)

        return ('seq', (data_type, val), eval_actual_args(p2))

    else:
        raise ValueError("unknown node type: {}".format(args[0]))

#########################################################################
def declare_formal_args(formal_args, actual_val_args):

    if len_seq(actual_val_args) != len_seq(formal_args):
        raise ValueError("actual and formal argument lists do not match")

    if formal_args[0] == 'nil':
        return

    # unpack the args
    (SEQ, (FORMALARG, formal_type, formal_arg), p1) = formal_args
    (SEQ, (actual_type, actual_val), p2) = actual_val_args

    # declare the variable
    if not safe_assign(formal_type, actual_type):
        raise ValueError("cannot assign a value of type {} to formal argument {} of type {}"\
                         .format(actual_type, formal_arg, formal_type))

    if is_primitive(formal_type):
        state.symbol_table \
             .declare_scalar(formal_arg, formal_type, (actual_type, actual_val))
    elif formal_type[0] == 'array-type':
        # arrays are passed by reference - first declare the arrays
        # and then replace the declared memory with the memory
        # of the actual argument.
        state.symbol_table.declare_array(formal_arg, formal_type, ('nil',))
        value = ('array-val', actual_type, actual_val)
        state.symbol_table.update_sym(formal_arg, value)

    else:
        raise ValueError("unknown declaration type {}".format(str(formal_type)))

    declare_formal_args(p1, p2)

#########################################################################
def handle_call(name, actual_arglist):

    (FUNCTION_VAL, function_type, lambda_val) = state.symbol_table.lookup_sym(name)
    
    if FUNCTION_VAL != 'function-val':
        raise ValueError("{} is not a function".format(name))

    # unpack the symtab info
    (FUNCTION_TYPE, return_data_type, arg_types) = function_type
    assert_match(FUNCTION_TYPE, 'function-type')
    (LAMDBA, formal_arglist, body, context) = lambda_val
    assert_match(LAMDBA, 'lambda')

    if len_seq(formal_arglist) != len_seq(actual_arglist):
        raise ValueError("function {} expects {} arguments"\
                         .format(name, len_seq(formal_arglist)))

    # set up the environment for static scoping and then execute the function
    actual_val_args = eval_actual_args(actual_arglist)   # evaluate actuals in current symtab
    save_symtab = state.symbol_table.get_config()        # save current symtab

    state.symbol_table.set_config(context)               # make function context current symtab
    state.symbol_table.push_scope()                      # push new function scope
    declare_formal_args(formal_arglist, actual_val_args) # declare formals in function scope

    return_value = None
    try:
        walk(body)                                       # execute the function
    except ReturnValue as val:
        if return_data_type[0] == 'void' and not val.value is None:
            raise ValueError("the void function {} cannot return a value"\
                             .format(name))
        elif return_data_type[0] == 'void' and val.value is None:
            return_value = None
        elif return_data_type[0] != 'void' and val.value is None:
            raise ValueError("the function {} of type {} returns no value"\
                             .format(name, return_data_type))
        else:
            # make sure that we are returning the correct type
            (data_type, _) = val.value
            if not safe_assign(return_data_type, data_type):
                raise ValueError("return statement in {} returned a value of type {} instead of the expected {}"\
                                 .format(name, data_type, return_data_type))
            return_value = val.value

    # NOTE: popping the function scope is not necessary because we
    # are restoring the original symtab configuration
    state.symbol_table.set_config(save_symtab)           # restore original symtab config

    return return_value

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
def fundecl_stmt(node):

    (FUNDECL, name, return_type, arglist, body) = node
    assert_match(FUNDECL, 'fundecl')

    context = state.symbol_table.get_config()
    
    state.symbol_table \
         .declare_fun(name, return_type, arglist, body, context)

#########################################################################
def decl_stmt(node):
    (DECL, name, data_type, init_val) = node
    assert_match(DECL, 'decl')

    if is_primitive(data_type):
        state.symbol_table \
             .declare_scalar(name, data_type, compute_init_vals(init_val))
    elif data_type[0] == 'array-type':
        state.symbol_table \
             .declare_array(name, data_type, compute_init_vals(init_val))
    else:
        raise ValueError("unknown declaration type {}".format(str(data_type)))

#########################################################################
def assign_stmt(node):

    (ASSIGN, lexp, rexp) = node
    assert_match(ASSIGN, 'assign')

    if lexp[0] == 'id': #('id', name)
        # grab everything we know about variable named id
        name = lexp[1]
        (sym_type, data_type, val) = state.symbol_table.lookup_sym(name)

        # compute rhs value of assignment stmt
        (t, v) = walk(rexp)
        if not safe_assign(data_type, t):
            raise ValueError(\
                "a value of type {} cannot be assigned to the variable {} of type {}"\
                .format(t,name,data_type))

        # update symbol table with new value
        value = (sym_type, data_type, conversion_fun(data_type,t)(v))
        state.symbol_table.update_sym(name, value)

    elif lexp[0] == 'array-access': # ('array-access', id, exp)
        # grab everything we know about the array named id
        name = lexp[1]
        (ARRAY_VAL, data_type, memory) = state.symbol_table.lookup_sym(name)
        if ARRAY_VAL != 'array-val': # make sure we are seeing an array-val
            raise ValueError("cannot index non-array {}".format(name))
        (ARRAY_TYPE, size_val, elem_type) = data_type
        assert_match(ARRAY_TYPE, 'array-type')

        # grab the index expression value and do error checking
        (ix_type, ix_val) = walk(lexp[2])
        if ix_type[0] != 'integer':
            raise ValueError("illegal index value {}".format(ix_val))
        if ix_val not in range(size_val):
            raise ValueError("index {} for {} out of bounds".format(ix_val, name))

        # compute the rhs value of the assignment stmt
        (t,v) = walk(rexp)
        if not safe_assign(elem_type, t):
            raise ValueError(\
                "a value of type {} cannot be assigned to the elements of {} of type {}"\
                .format(t,name,elem_type))

        # update the memory cell for this array access
        memory[ix_val] = conversion_fun(elem_type,t)(v)

    else:
        raise ValueError("illegal left side {} of assignment".format(lexp[0]))

#########################################################################
def print_stmt(node):

    (PRINT, exp) = node
    assert_match(PRINT, 'print')

    (_, v) = walk(exp)
    print(str(v))

#########################################################################
def call_stmt(node):
    (CALLSTMT, name, actual_args) = node
    assert_match(CALLSTMT, 'callstmt')

    handle_call(name, actual_args)

#########################################################################
def return_stmt(node):
    # if a return value exists the return stmt will record it
    # in the state object

    try: # try return without exp
        (RETURN, (NIL,)) = node
        assert_match(RETURN, 'return')
        assert_match(NIL, 'nil')

    except ValueError: # return with exp
        (RETURN, exp) = node
        assert_match(RETURN, 'return')

        value = walk(exp)
        raise ReturnValue(value)

    else: # return without exp
        raise ReturnValue(None)

#########################################################################
def while_stmt(node):

    (WHILE, cond, body) = node
    assert_match(WHILE, 'while')

    (t, v) = walk(cond)

    if t[0] != 'integer':
        raise ValueError("the while condition has to be an integer expression")

    while v != 0:
        walk(body)
        (_, v) = walk(cond)

#########################################################################
def for_stmt(node):

    state.symbol_table.push_scope()
    
    try: # try the declaration pattern
        (FOR, (NIL,), ID, init, cond, step, body) = node
        assert_match(FOR, 'for')
        assert_match(NIL, 'nil')
        walk(('assign', ('id', ID), init))
        
    except ValueError: # declaration pattern didn't match
        (FOR, data_type, ID, init, cond, step, body) = node
        assert_match(FOR, 'for')
        if is_primitive(data_type):
            state.symbol_table \
                 .declare_scalar(ID, data_type, compute_init_vals(init))
        elif data_type[0] == 'array-type':
            state.symbol_table \
                 .declare_array(ID, data_type, compute_init_vals(init))
        else:
            raise ValueError("unknown declaration type {}".format(str(data_type)))
        
    
    (t, v) = walk(cond)

    if t[0] != 'integer':
        raise ValueError("the while condition has to be an integer expression")

    while v != 0:
        walk(body)
        walk(('assign', ('id', ID), step))
        (_, v) = walk(cond)
    
    state.symbol_table.pop_scope()
    
#########################################################################
def if_stmt(node):

    try: # try the if-then pattern
        (IF, cond, then_stmt, (NIL,)) = node
        assert_match(IF, 'if')
        assert_match(NIL, 'nil')

    except ValueError: # if-then pattern didn't match
        (IF, cond, then_stmt, else_stmt) = node
        assert_match(IF, 'if')

        (t, v) = walk(cond)

        if t[0] != 'integer':
            raise ValueError("the if condition has to be an integer expression")

        if v != 0:
            walk(then_stmt)
        else:
            walk(else_stmt)

    else: # if-then pattern matched
        (t, v) = walk(cond)

        if t[0] != 'integer':
            raise ValueError("the if condition has to be an integer expression")

        if v != 0:
            walk(then_stmt)

#########################################################################
def block_stmt(node):

    (BLOCK, stmt_list) = node
    assert_match(BLOCK, 'block')

    state.symbol_table.push_scope()
    walk(stmt_list)
    state.symbol_table.pop_scope()

#########################################################################
def plus_exp(node):

    (PLUS,c1,c2) = node
    assert_match(PLUS, 'plus')

    (t1, v1) = walk(c1)
    (t2, v2) = walk(c2)

    type = promote(t1, t2)

    if type[0] in ['integer', 'double']:
        return (type, v1 + v2)
    elif type[0] == 'string':
        return (('string',), str(v1) + str(v2))
    else:
        raise ValueError('unsupported type {} in + operator'.format(type[0]))

#########################################################################
def minus_exp(node):

    (MINUS,c1,c2) = node
    assert_match(MINUS, 'minus')

    (t1, v1) = walk(c1)
    (t2, v2) = walk(c2)

    type = promote(t1, t2)

    if type[0] in ['integer', 'double']:
         return (type, v1 - v2)
    else:
        raise ValueError('unsupported type {} in - operator'.format(type[0]))

#########################################################################
def times_exp(node):

    (TIMES,c1,c2) = node
    assert_match(TIMES, 'times')

    (t1, v1) = walk(c1)
    (t2, v2) = walk(c2)

    type = promote(t1, t2)

    if type[0] in ['integer', 'double']:
         return (type, v1 * v2)
    else:
        raise ValueError('unsupported type {} in * operator'.format(type[0]))

#########################################################################
def divide_exp(node):

    (DIVIDE,c1,c2) = node
    assert_match(DIVIDE, 'divide')

    (t1, v1) = walk(c1)
    (t2, v2) = walk(c2)

    type = promote(t1, t2)

    if type[0] in ['integer', 'double']:
        return (('integer',), int(v1 / v2)) if (v1 / v2).is_integer() else (('double',), v1 / v2)
    else:
        raise ValueError('unsupported type {} in / operator'.format(type[0]))

#########################################################################
def eq_exp(node):

    (EQ,c1,c2) = node
    assert_match(EQ, '==')

    (t1, v1) = walk(c1)
    (t2, v2) = walk(c2)

    type = promote(t1, t2)

    if type[0] in ['integer', 'double']:
         return (('integer',), 1 if v1 == v2 else 0)
    else:
        raise ValueError('unsupported type {} in / operator'.format(type[0]))

#########################################################################
def le_exp(node):

    (LE,c1,c2) = node
    assert_match(LE, '<=')

    (t1, v1) = walk(c1)
    (t2, v2) = walk(c2)

    type = promote(t1, t2)

    if type[0] in ['integer', 'double']:
         return (('integer',), 1 if v1 <= v2 else 0)
    else:
        raise ValueError('unsupported type {} in / operator'.format(type[0]))

#########################################################################
def ge_exp(node):

    (GE,c1,c2) = node
    assert_match(GE, '>=')

    (t1, v1) = walk(c1)
    (t2, v2) = walk(c2)

    type = promote(t1, t2)

    if type[0] in ['integer', 'double']:
         return (('integer',), 1 if v1 >= v2 else 0)
    else:
        raise ValueError('unsupported type {} in / operator'.format(type[0]))
        
#########################################################################
def lt_exp(node):

    (LT,c1,c2) = node
    assert_match(LT, '<')

    (t1, v1) = walk(c1)
    (t2, v2) = walk(c2)

    type = promote(t1, t2)

    if type[0] in ['integer', 'double']:
         return (('integer',), 1 if v1 < v2 else 0)
    else:
        raise ValueError('unsupported type {} in / operator'.format(type[0]))

#########################################################################
def gt_exp(node):

    (GT,c1,c2) = node
    assert_match(GT, '>')

    (t1, v1) = walk(c1)
    (t2, v2) = walk(c2)

    type = promote(t1, t2)

    if type[0] in ['integer', 'double']:
         return (('integer',), 1 if v1 > v2 else 0)
    else:
        raise ValueError('unsupported type {} in / operator'.format(type[0]))

#########################################################################
def ne_exp(node):

    (NE,c1,c2) = node
    assert_match(NE, '!=')

    (t1, v1) = walk(c1)
    (t2, v2) = walk(c2)

    type = promote(t1, t2)

    if type[0] in ['integer', 'double']:
         return (('integer',), 1 if v1 != v2 else 0)
    else:
        raise ValueError('unsupported type {} in / operator'.format(type[0]))

#########################################################################
def mod_exp(node):

    (MOD,c1,c2) = node
    assert_match(MOD, '%')

    (t1, v1) = walk(c1)
    (t2, v2) = walk(c2)

    type = promote(t1, t2)

    if type[0] == 'integer':
        return (type, v1 % v2)
    else:
        raise ValueError('unsupported type {} in % operator'.format(type[0]))
        
#########################################################################
def and_exp(node):

    (AND,c1,c2) = node
    assert_match(AND, '&&')

    (t1, v1) = walk(c1)
    (t2, v2) = walk(c2)

    type = promote(t1, t2)
 
    if type[0] in ['integer', 'double']:
             return (('integer',), 1 if v1!=0 and v2!=0 else 0)
    else:
        raise ValueError('unsupported type {} in % operator'.format(type[0]))
        
#########################################################################
def or_exp(node):

    (OR,c1,c2) = node
    assert_match(OR, '||')

    (t1, v1) = walk(c1)
    (t2, v2) = walk(c2)

    type = promote(t1, t2)
 
    if type[0] in ['integer', 'double']:
             return (('integer',), 1 if v1!=0 or v2!=0 else 0)
    else:
        raise ValueError('unsupported type {} in % operator'.format(type[0]))
        
#########################################################################
def array_exp(node):

    #try to match no-prefix pattern
    try: 
        (ARRAY_ACCESS, name, exp, (NIL,)) = node
        assert_match(ARRAY_ACCESS, 'array-access')
        assert_match(NIL, 'nil')

        # grab everything we know about the array named name
        (ARRAY_VAL, data_type, memory) = state.symbol_table.lookup_sym(name)
        if ARRAY_VAL != 'array-val': # make sure we are seeing an array-val
            raise ValueError("cannot index non-array {}".format(name))
        (ARRAY_TYPE, size_val, elem_type) = data_type
        assert_match(ARRAY_TYPE, 'array-type')

        # grab the index expression value and do error checking
        (ix_type, ix_val) = walk(exp)
        if ix_type[0] != 'integer':
            raise ValueError("illegal index value {}".format(ix_val))
        if ix_val not in range(size_val):
            raise ValueError("index {} for {} out of bounds".format(ix_val, name))
    
    #if prefix
    except:
        (ARRAY_ACCESS, name, exp, PRE) = node
        assert_match(ARRAY_ACCESS, 'array-access')
        (ARRAY_VAL, data_type, memory) = state.symbol_table.lookup_sym(name)
        (ARRAY_TYPE, size_val, elem_type) = data_type
        #remove and operate with 
        if elem_type == ('integer',):
            if PRE == 'inc':
                walk(('assign', (ARRAY_ACCESS, name, exp, ('nil',)), ('plus', (ARRAY_ACCESS, name, exp, ('nil',)), ('val', data_type, 1))))
            elif PRE == 'dec':
                walk(('assign', (ARRAY_ACCESS, name, exp, ('nil',)), ('minus', (ARRAY_ACCESS, name, exp, ('nil',)), ('val', data_type, 1))))
        else:
            raise ValueError("prefix {} not allowed for type {}".format(PRE, elem_type))
        (ARRAY_VAL, data_type, memory) = state.symbol_table.lookup_sym(name)
        (ix_type, ix_val) = walk(exp)
        
    # return the value of the memory cell
    return (elem_type, memory[ix_val])

#########################################################################
def id_exp(node):
    try: 
        (ID, name, (NIL,)) = node
        assert_match(ID, 'id')
        assert_match(NIL, 'nil')
        (sym_type, data_type, val) = state.symbol_table.lookup_sym(name)
        
    except:
        (ID, name, PRE) = node
        assert_match(ID, 'id')
        (sym_type, data_type, val) = state.symbol_table.lookup_sym(name)
        if data_type[0] == 'integer':
            if PRE == 'inc':
                walk(('assign', (ID, name, ('nil',)), ('plus', (ID, name, ('nil',)), ('val', data_type, 1))))
            elif PRE == 'dec':
                walk(('assign', (ID, name, ('nil',)), ('minus', (ID, name, ('nil',)), ('val', data_type, 1))))
        else:
            raise ValueError("prefix {} not allowed for type {}".format(PRE, data_type[0]))
        (sym_type, data_type, val) = state.symbol_table.lookup_sym(name)

    return (data_type, val)

#########################################################################
def val_exp(node):

    (VAL, data_type, value) = node
    assert_match(VAL, 'val')

    return (data_type, value)

#########################################################################
def call_exp(node):
    # call_exp works just like call_stmt with the exception
    # that we have to pass back a return value

    (CALLEXP, name, args) = node
    assert_match(CALLEXP, 'callexp')

    return_value = handle_call(name, args)

    if return_value is None:
        raise ValueError("No return value from function {}".format(name))

    return return_value

#########################################################################
def uminus_exp(node):

    (UMINUS, exp) = node
    assert_match(UMINUS, 'uminus')

    (t, v) = walk(exp)

    if t[0] in ['integer', 'double']:
        return (t, - v)
    else:
        raise ValueError("unsupported type {} in unary minus expression".format(t[0]))

#########################################################################
def not_exp(node):

    (NOT, exp) = node
    assert_match(NOT, 'not')

    (t, v) = walk(exp)

    if t[0] == 'integer':
        return (('integer',), 0 if v != 0 else 1)
    else:
        raise ValueError("unsupported type {} in not expression".format(t))

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
    'seq'         : seq,
    'nil'         : nil,
    'fundecl'     : fundecl_stmt,
    'decl'        : decl_stmt,
    'assign'      : assign_stmt,
    'print'       : print_stmt,
    'callstmt'    : call_stmt,
    'return'      : return_stmt,
    'while'       : while_stmt,
    'for'         : for_stmt,
    'if'          : if_stmt,
    'block'       : block_stmt,
    'val'         : val_exp,
    'id'          : id_exp,
    'array-access': array_exp,
    'callexp'     : call_exp,
    'plus'        : plus_exp,
    'minus'       : minus_exp,
    'times'       : times_exp,
    'divide'      : divide_exp,
    '=='          : eq_exp,
    '<='          : le_exp,
    '>='          : ge_exp,
    '<'           : lt_exp,
    '>'           : gt_exp,
    '!='          : ne_exp,
    '%'           : mod_exp,
    '&&'          : and_exp,
    '||'          : or_exp,
    'uminus'      : uminus_exp,
    'not'         : not_exp
}