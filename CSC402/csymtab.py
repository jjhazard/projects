#########################################################################
# symbol table for Cuppa5
#
# it is a scoped symbol table with a dictionary at each scope level
#
#########################################################################

from ctype_promotion import safe_assign, conversion_fun

CURR_SCOPE = 0

def _compute_arg_types(arg_list):
    if arg_list[0] == 'nil':
        return ('nil',)
    else:
        (SEQ, formalarg, next) = arg_list
        (FORMALARG, type, id) = formalarg
        return ('seq', type, _compute_arg_types(next))

def _compute_array_initializer(elem_type, initializer):
    '''
    array initializers come in two flavors:
    (a) as a list of values
    (b) as an expression that represent sufficient init values for the array
    here we check and compute the actual memory for the array.
    '''
    if initializer[0] == 'nil':
        return []
    elif initializer[0] == 'seq':
        (SEQ, (t,v), next) = initializer
        if not safe_assign(elem_type, t):
            raise ValueError(\
                "illegal array initializer {}"\
                .format(v))
        return [conversion_fun(elem_type, t)(v)] + _compute_array_initializer(elem_type, next)
    elif initializer[0][0] == 'array-type':
        return list(initializer[1])
    else:
        raise ValueError("illegal array initializer {}".format(initializer))

class SymTab:

    def __init__(self):
        '''
        constructor: define and initialize all data structures
        '''
        # global scope dictionary must always be present
        self.scoped_symtab = [{}]

    #####################################################################
    # scope manipulation functions

    def get_config(self):
        '''
        current configuration is a shallow copy of the dictionary stack
        '''
        return list(self.scoped_symtab)

    def set_config(self, c):
        '''
        replace the configuration of the symbol table with another
        '''
        self.scoped_symtab = c

    def push_scope(self):
        '''
        push a new dictionary onto the stack - stack grows to the left
        '''
        self.scoped_symtab.insert(CURR_SCOPE,{})

    def pop_scope(self):
        '''
        pop the current (left most) dictionary off the stack
        '''
        if len(self.scoped_symtab) == 1:
            raise ValueError("cannot pop the global scope")
        else:
            self.scoped_symtab.pop(CURR_SCOPE)

    #####################################################################
    # symbol functions
    # types of symbol values:
    #     ('scalar-val', primitive_type, val)
    #     ('function-val', function_type, ('lambda', arglist, body, context))
    #     ('array-val', array_type, memory)

    def declare_scalar(self, sym, data_type, init_val):
        '''
        declare a scalar in the current scope.
        '''
        # first we need to check whether the symbol was already declared
        # at this scope
        if sym in self.scoped_symtab[CURR_SCOPE]:
            raise ValueError("symbol {} already declared".format(sym))

        # look at init value
        if init_val[0] == 'nil':
            init_val = conversion_fun(data_type,('integer',))(0)
        else:
            (t,v, post) = init_val
            if safe_assign(data_type, t):
                init_val = conversion_fun(data_type, t)(v)
            else:
                raise ValueError("illegal init value {}".format(v))

        # enter the symbol with its value in the current scope
        val = ('scalar-val', data_type, init_val)
        self.scoped_symtab[CURR_SCOPE].update({sym : val})

    def declare_fun(self, sym, return_type, arg_list, body, context):
        '''
        declare a function in the current scope.
        '''
        # first we need to check whether the symbol was already declared
        # at this scope
        if sym in self.scoped_symtab[CURR_SCOPE]:
            raise ValueError("symbol {} already declared".format(sym))

        # enter the symbol with its value in the current scope
        arg_types = _compute_arg_types(arg_list)
        type = ('function-type', return_type, arg_types)
        lambda_val = ('lambda', arg_list, body, context)
        val = ('function-val', type, lambda_val)
        self.scoped_symtab[CURR_SCOPE].update({sym : val})
        
    def declare_array(self, sym, array_type, init_val):
        '''
        declare an array in the current scope.
        '''
        # first we need to check whether the symbol was already declared
        # at this scope
        if sym in self.scoped_symtab[CURR_SCOPE]:
            raise ValueError("symbol {} already declared".format(sym))

        # unpack the array type
        (ARRAY_TYPE, size, elem_type) = array_type

        # look at the initializer
        if init_val[0] == 'nil':
            memory = [conversion_fun(elem_type,('integer',))(0) for i in range(size)]
        else:
            memory = _compute_array_initializer(elem_type, init_val)
            if len(memory) != size:
                raise ValueError("size of initializer does not match array {}"\
                                 .format(sym))

        # declare symbol in current scope
        val = ('array-val', array_type, memory)
        self.scoped_symtab[CURR_SCOPE].update({sym : val})

    def lookup_sym(self, sym):
        '''
        find the first occurence of sym in the symtab stack
        and return the associated value
        '''
        n_scopes = len(self.scoped_symtab)
        for scope in range(n_scopes):
            if sym in self.scoped_symtab[scope]:
                return self.scoped_symtab[scope].get(sym)

        # not found
        raise ValueError("{} was not declared".format(sym))

    def update_sym(self, sym, val):
        '''
        find the first occurence of sym in the symtab stack
        and update the associated value
        '''
        n_scopes = len(self.scoped_symtab)

        for scope in range(n_scopes):
            if sym in self.scoped_symtab[scope]:
                self.scoped_symtab[scope].update({sym : val})
                return

        # not found
        raise ValueError("{} was not declared".format(sym))

#########################################################################