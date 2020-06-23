from ply import yacc
from stackmachine_lex import tokens, lexer
from stackmachine_interp_state import state

def p_prog(_):
    '''
    prog : instr_list
    '''
    pass

def p_instr_list(_):
    '''
    instr_list : labeled_instr instr_list
              | empty
    '''
    pass

def p_labeled_instr(p):
    '''
    labeled_instr : label_def instr
    '''
    # if label exists record it in the label table
    if p[1]:
        state.label_table[p[1]] = state.instr_ix
    # append instr to program
    state.program.append(p[2])
    state.instr_ix += 1

def p_label_def(p):
    '''
    label_def : NAME ':' 
              | empty
    '''
    p[0] = p[1]

def p_instr(p):
    '''
    instr : PUSH exp ';'
          | POP ';'
          | PRINT msg ';'
          | STORE NAME ';'
          | ASK msg ';'
          | DUP ';'
          | ADD ';'
          | SUB ';'
          | MUL ';'
          | DIV ';'
          | EQU ';'
          | LEQ ';'
          | JUMPT label ';'
          | JUMPF label ';'
          | JUMP label ';'
          | STOP msg ';'
          | NOOP ';'
    '''
    # for each instr assemble the appropriate tuple
    if p[1] in ['push', 'print', 'store', 'ask', 'jumpT', 'jumpF', 'jump', 'stop']:
        p[0] = (p[1], p[2])
    elif p[1] in ['pop', 'dup', 'add', 'sub', 'mul', 'div', 'equ', 'leq', 'noop']:
        p[0] = (p[1],)
    else:
        raise ValueError("Unexpected instr value: %s" % p[1])

def p_label(p):
    '''
        label : NAME
        '''
    p[0] = p[1]

def p_uminus_exp(p):
    '''
    exp : '-' exp
    '''
    p[0] = ('UMINUS', p[2])
    
def p_var_exp(p):
    '''
    exp : NAME
    '''
    p[0] = ('NAME', p[1])

def p_number_exp(p):
    '''
    exp : NUMBER
    '''
    p[0] = ('NUMBER', int(p[1]))
    
def p_msg(p):
    '''
        msg : MSG
            | empty
        '''
    p[0] = p[1]
    
def p_empty(p):
    '''
    empty : 
    '''
    p[0] = ''

def p_error(t):
    print("Syntax error at '%s'" % t.value)

parser = yacc.yacc(debug=False, tabmodule='stackmachineparsetab')

