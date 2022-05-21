#ifndef  __INCLUDE__VALUE__
#define  __INCLUDE__VALUE__
#include "obj.h"
#include "mem.h"
#include "type.h"

#define VALUE_GET_TYPE(v) v->type
#define VALUE_SET_STRING_LEN(v,l) v->string.len = l
#define VALUE_GET_STRING_LEN(v,l) v->string.len

struct value_t {
  object_t obj;
  type_t type;
  int size;
  
  union {
	long int iv;
	float fv;
	double dv;
	string_t s;
  } value;

  union {
	string_t s;	
	long int len;
  } string;
};

value_t *create_value(type_t type,int size);

#endif
