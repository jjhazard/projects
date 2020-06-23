# Lexer for Cuppa4

import re
from ply import lex

reserved = {
    'printf'  : 'PRINT',
    'if'      : 'IF',
    'else'    : 'ELSE',
    'while'   : 'WHILE',
    'for'     : 'FOR',
    'not'     : 'NOT',
    'return'  : 'RETURN',
    'int'     : 'INTEGER_TYPE',
    'double'  : 'DOUBLE_TYPE',
    'string'  : 'STRING_TYPE',
    'void'    : 'VOID_TYPE'
}

literals = [',',';','=','(',')','{','}', '[',']','+','-','*','/']

tokens = [
          'EQ','LE','GE','LT','GT','NE','AE',
          'ME','TE','DE','MOD','AND','OR','INC','DEC',
          'INTEGER', 'DOUBLE', 'STRING', 'ID',
          ] + list(reserved.values())

t_EQ      = r'=='
t_LE      = r'<='
t_GE      = r'>='
t_LT      = r'<'
t_GT      = r'>'
t_NE      = r'!='
t_AE      = r'\+='
t_ME      = r'-='
t_TE      = r'\*='
t_DE      = r'/='
t_MOD     = r'%'
t_AND     = r'&&'
t_OR      = r'\|\|'
t_INC     = r'\+\+'
t_DEC     = r'--'


t_ignore = ' \t'

def t_ID(t):
    r'[a-zA-Z_][a-zA-Z_0-9]*'
    t.type = reserved.get(t.value,'ID')    # Check for reserved words
    return t

def t_NUMBER(t):
    r'([0-9]*[.])?[0-9]+'
    t.type = 'FLOAT' if '.' in t.value else 'INTEGER'
    return t

def t_STRING(t):
    r'\"[^\"]*\"'
    t.value = t.value[1:-1] # strip the quotes
    return t

def t_COMMENT(t):
    r'//.*'
    pass

def t_NEWLINE(t):
    r'\n'
    pass

def t_error(t):
    print("Illegal character %s" % t.value[0])
    t.lexer.skip(1)

# build the lexer
lexer = lex.lex(debug=0)