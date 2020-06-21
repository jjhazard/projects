
# exp1parsetab.py
# This file is automatically generated. Do not edit.
# pylint: disable=W,C,R
_tabversion = '3.10'

_lr_method = 'LALR'

_lr_signature = "NAME NUMBER PRINT STOREprog : stmt_list\n    stmt_list : stmt stmt_list\n              | empty\n    stmt : PRINT exp ';'stmt : STORE NAME exp ';'\n    exp : '+' exp exp\n    \n    exp : '-' exp exp\n    \n    exp : '*' exp exp\n    \n    exp : '/' exp exp\n    \n    exp : '(' exp ')'\n    exp : varexp : numvar : NAMEnum : NUMBERempty :"
    
_lr_action_items = {'PRINT':([0,3,19,31,],[5,5,-4,-5,]),'STORE':([0,3,19,31,],[6,6,-4,-5,]),'$end':([0,1,2,3,4,7,19,31,],[-15,0,-1,-15,-3,-2,-4,-5,]),'+':([5,9,10,11,12,13,14,15,16,17,18,20,21,22,23,26,27,28,29,30,],[9,9,9,9,9,9,-11,-12,-13,-14,9,9,9,9,9,-6,-7,-8,-9,-10,]),'-':([5,9,10,11,12,13,14,15,16,17,18,20,21,22,23,26,27,28,29,30,],[10,10,10,10,10,10,-11,-12,-13,-14,10,10,10,10,10,-6,-7,-8,-9,-10,]),'*':([5,9,10,11,12,13,14,15,16,17,18,20,21,22,23,26,27,28,29,30,],[11,11,11,11,11,11,-11,-12,-13,-14,11,11,11,11,11,-6,-7,-8,-9,-10,]),'/':([5,9,10,11,12,13,14,15,16,17,18,20,21,22,23,26,27,28,29,30,],[12,12,12,12,12,12,-11,-12,-13,-14,12,12,12,12,12,-6,-7,-8,-9,-10,]),'(':([5,9,10,11,12,13,14,15,16,17,18,20,21,22,23,26,27,28,29,30,],[13,13,13,13,13,13,-11,-12,-13,-14,13,13,13,13,13,-6,-7,-8,-9,-10,]),'NAME':([5,6,9,10,11,12,13,14,15,16,17,18,20,21,22,23,26,27,28,29,30,],[16,18,16,16,16,16,16,-11,-12,-13,-14,16,16,16,16,16,-6,-7,-8,-9,-10,]),'NUMBER':([5,9,10,11,12,13,14,15,16,17,18,20,21,22,23,26,27,28,29,30,],[17,17,17,17,17,17,-11,-12,-13,-14,17,17,17,17,17,-6,-7,-8,-9,-10,]),';':([8,14,15,16,17,25,26,27,28,29,30,],[19,-11,-12,-13,-14,31,-6,-7,-8,-9,-10,]),')':([14,15,16,17,24,26,27,28,29,30,],[-11,-12,-13,-14,30,-6,-7,-8,-9,-10,]),}

_lr_action = {}
for _k, _v in _lr_action_items.items():
   for _x,_y in zip(_v[0],_v[1]):
      if not _x in _lr_action:  _lr_action[_x] = {}
      _lr_action[_x][_k] = _y
del _lr_action_items

_lr_goto_items = {'prog':([0,],[1,]),'stmt_list':([0,3,],[2,7,]),'stmt':([0,3,],[3,3,]),'empty':([0,3,],[4,4,]),'exp':([5,9,10,11,12,13,18,20,21,22,23,],[8,20,21,22,23,24,25,26,27,28,29,]),'var':([5,9,10,11,12,13,18,20,21,22,23,],[14,14,14,14,14,14,14,14,14,14,14,]),'num':([5,9,10,11,12,13,18,20,21,22,23,],[15,15,15,15,15,15,15,15,15,15,15,]),}

_lr_goto = {}
for _k, _v in _lr_goto_items.items():
   for _x, _y in zip(_v[0], _v[1]):
       if not _x in _lr_goto: _lr_goto[_x] = {}
       _lr_goto[_x][_k] = _y
del _lr_goto_items
_lr_productions = [
  ("S' -> prog","S'",1,None,None,None),
  ('prog -> stmt_list','prog',1,'p_prog','jh1_lrinterp.py',7),
  ('stmt_list -> stmt stmt_list','stmt_list',2,'p_stmt_list','jh1_lrinterp.py',12),
  ('stmt_list -> empty','stmt_list',1,'p_stmt_list','jh1_lrinterp.py',13),
  ('stmt -> PRINT exp ;','stmt',3,'p_print_stmt','jh1_lrinterp.py',18),
  ('stmt -> STORE NAME exp ;','stmt',4,'p_store_stmt','jh1_lrinterp.py',22),
  ('exp -> + exp exp','exp',3,'p_plus_exp','jh1_lrinterp.py',27),
  ('exp -> - exp exp','exp',3,'p_minus_exp','jh1_lrinterp.py',33),
  ('exp -> * exp exp','exp',3,'p_times_exp','jh1_lrinterp.py',39),
  ('exp -> / exp exp','exp',3,'p_divide_exp','jh1_lrinterp.py',45),
  ('exp -> ( exp )','exp',3,'p_paren_exp','jh1_lrinterp.py',51),
  ('exp -> var','exp',1,'p_var_exp','jh1_lrinterp.py',56),
  ('exp -> num','exp',1,'p_num_exp','jh1_lrinterp.py',60),
  ('var -> NAME','var',1,'p_var','jh1_lrinterp.py',64),
  ('num -> NUMBER','num',1,'p_num','jh1_lrinterp.py',68),
  ('empty -> <empty>','empty',0,'p_empty','jh1_lrinterp.py',72),
]