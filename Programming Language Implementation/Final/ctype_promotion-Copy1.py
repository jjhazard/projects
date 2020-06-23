# Cuppa5 type promotion tables for builtin primitive types.  these tables
# implement the type hierarchy
#
#      integer < double < string
#      array < string
#      void

def _id(a):
    'identity function for type conversions'
    return a

_promote_table = {
  'string'    : {'string': 'string', 'double': 'string', 'integer': 'string',  'array-type': 'string',     'void': 'None'},
  'double'    : {'string': 'string', 'double': 'double', 'integer': 'double',  'array-type': 'None',       'void': 'None'},
  'integer'   : {'string': 'string', 'double': 'double', 'integer': 'integer', 'array-type': 'None',       'void': 'None'},
  'array-type': {'string': 'string', 'double': 'None',   'integer': 'None',    'array-type': 'array-type', 'void': 'None'},
  'void'      : {'string': 'None',   'double': 'None',   'integer': 'None',    'array-type': 'None',       'void': 'void'},
}

_conversion_table = {
  'string'    : {'string': str,  'double': str,   'integer': str,   'array-type': str,  'void': None},
  'double'    : {'string': str,  'double': float, 'integer': float, 'array-type': None, 'void': None},
  'integer'   : {'string': str,  'double': float, 'integer': int,   'array-type': None, 'void': None},
  'array-type': {'string': str,  'double': None,  'integer': None,  'array-type': _id,  'void': None},
  'void'      : {'string': None, 'double': None,  'integer': None,  'array-type': None, 'void': _id},
}

_safe_assign_table = {
  'string' :    {'string': True,  'double': True,  'integer': True,  'array-type': True,  'void': False},
  'double' :    {'string': False, 'double': True,  'integer': True,  'array-type': False, 'void': False},
  'integer':    {'string': False, 'double': False, 'integer': True,  'array-type': False, 'void': False},
  'array-type': {'string': False, 'double': False, 'integer': False, 'array-type': True,  'void': False},
  'void'   :    {'string': False, 'double': False, 'integer': False, 'array-type': False, 'void': False},
}

def promote(type1, type2):
    return (_promote_table.get(type1[0]).get(type2[0]),)

def conversion_fun(ltype, rtype):
    return _conversion_table.get(ltype[0]).get(rtype[0])

def safe_assign(ltype, rtype):
        return _safe_assign_table.get(ltype[0]).get(rtype[0])

def is_primitive(type):
    return type[0] in ['integer', 'double', 'string']

def cast_string(to_data_type, string_arg):
    try:
        if to_data_type[0] == 'integer':
            return (('integer',), int(string_arg))
        elif to_data_type[0] == 'double':
            return (('double',), float(string_arg))
        elif to_data_type[0] == 'string':
            return (('string',), string_arg)
    except ValueError:
        raise ValueError(\
            "value {} not supported for type {}"\
            .format(string_arg, to_data_type))
    else:
        raise ValueError(\
            "type {} and not supported in 'cast_string'"\
            .format(to_data_type))