// 
//
//  Copyright 2019 Shota Hisai. Released under the MIT license.
//
//
#include <stdio.h>
#include <string.h>
#include "util.h"
#include "list.h"
#include "mem.h"
#include "common.h"
#include "inst.inc"
#include "dump.h"
#include "symbol.h"
#include "value.h"
#include "ope.h"
#include "env.h"
#include "compound_type.h"

static bool_t is_func_qualifier(list_t *lst);
static type_t conv_compound_type(compound_def_t *com);

int hash(char *name){

  char *p;
  int sum;

  sum = 0;
  p = name;
  while(*p){
    sum += *p++;
  }

  return sum % TABLE_SIZE;
}

bool_t is_inttype(list_t *lst){

  string_t type;
#ifdef __DEBUG__
  printf("is_inttype\n");
#endif

  type = car(lst);
  if(STRCMP(type,INT) ||
	 STRCMP(type,LONG) ||
	 STRCMP(type,CHAR) ||
	 STRCMP(type,SHORT)){
	return TRUE;
  }

  return FALSE;
}

bool_t is_float(list_t *lst){

  string_t type;

  type = car(lst);
  if(STRCMP(type,FLOAT)
	 || STRCMP(type,DOUBLE)){
	return TRUE;
  }

  return FALSE;
}

bool_t is_pointer(list_t *lst){
  
  if(!IS_SYMBOL(lst)){
    return FALSE;
  }

  return STRCMP(car(lst),"*");
}


bool_t is_address(list_t *lst){
  
   if(!IS_SYMBOL(lst)){
    return FALSE;
  }

  return STRCMP(car(lst),"&");
}

bool_t is_func(object_t *obj){
  return obj->type == TYPE_FUNCTION;
}

bool_t is_symbol(object_t *obj){
  return obj->type == TYPE_SYMBOL;
}

bool_t is_array(list_t *lst){

  string_t symbol;
  if(IS_SYMBOL(lst)){
	symbol = car(lst);
	if(STRCMP(symbol,ARRAY)){
	  return TRUE;
	}
  }

  return FALSE;
}

bool_t is_integer_type(env_t *env,list_t *lst){

  symbol_t *sym;
  string_t name;
  
  if(is_inttype(lst) || is_pointer(lst) || is_array(lst)){
	return TRUE;
  }

  name = car(lst);
  sym = lookup_obj(env,name);
  if(!sym){
	return FALSE;
  }

  return is_integer_type(env,SYMBOL_GET_TYPE_LST(sym));
}

int convert_hex_to_int(char *hex){

  int num;
  char *p;
  
  for(p = hex + 2, num = 0; *p; p++){
    if(('0' <= *p) && (*p <= '9')){
      num = num * 16 + *p - '0';
    } else if((('A' <= *p) && (*p <= 'F'))){
      num = num * 16 + (*p - 'A' + 10);
    } else if(('a' <= *p) && (*p <= 'f')){
      num = num * 16 + (*p - 'a' + 10);
    } else {
      return -1;
    }
  }
  
  return num;
}

bool_t is_var_args(list_t *lst){

  list_t *l;

  l = tail(lst);
  if(STRCMP(car(l),VAR_ARGS)){
	return TRUE;
  } else {
	return FALSE;
  }

}

bool_t is_qualifier(list_t *lst){

  string_t name;

  name = car(lst);
  if(STRCMP(STATIC,name)
	 || STRCMP(EXTERN,name)){
	return TRUE;
  }

  return FALSE;
}

bool_t is_static(list_t *lst){

  string_t name;

#ifdef __DEBUG__
  printf("is_static\n");
#endif
  name = car(lst);
  if(STRCMP(name,STATIC)){
	return TRUE;
  }

  return FALSE;
}

list_t *get_func_name(list_t *lst){

  list_t *p;
  string_t name;
  
#ifdef __DEBUG__
  printf("get_func_name\n");
#endif

  p = lst;
  while(IS_NOT_NULL_LIST(p)){
	if(IS_SYMBOL(p)
	   && !is_qualifier(p)
	   && !is_func_qualifier(p)){
	  break;
	}
	p = cdr(p);
  }

  if(IS_NULL_LIST(p)){
	exit(1);
  }

  return p;
}

static bool_t is_func_qualifier(list_t *lst){

  string_t name;
  
  name = car(lst);
  if(STRCMP(FUNC_DEF,name)
	 || STRCMP(FUNC_DECL,name)){
	return TRUE;
  }
  
  return FALSE;
  
}

symbol_t *lookup_member(env_t *env,string_t name){

  symbol_t *sym;
  
#ifdef __DEBUG__
  printf("lookup_member\n");
#endif

  sym = lookup_obj(env,name);
  if(!sym){
	exit(1);
  }

  return sym;
}

type_t conv_type(env_t *env,env_t *cenv,list_t *type_lst,list_t *lst){

  string_t name;
  list_type_t type;
  symbol_t *sym;
  compound_def_t *com;

#ifdef __DEBUG__
  printf("conv_type\n");
#endif

  type = LIST_GET_TYPE(type_lst);
  switch(type){
  case NULL_LIST:
	exit(1);
	break;
  case INTEGER:
	return conv_type(env,cenv,cdr(type_lst),lst);
	break;
  case LIST:
	return conv_type(env,cenv,car(type_lst),lst);
	break;
  default:
	name = car(type_lst);
	if(STRCMP(name,CHAR)){
	  return TYPE_CHAR;
	} else if(STRCMP(name,SHORT)){
	  return TYPE_SHORT;
	} else if(STRCMP(name,INT)){
	  return TYPE_INT;
	} else if(STRCMP(name,LONG)){
	  return TYPE_LONG;
	} else if(STRCMP(name,UNSIGNED)){
	  return TYPE_UNSIGNED;
	} else if(STRCMP(name,SIGNED)){
	  return TYPE_SIGNED;
	} else if(STRCMP(name,VOID)){
	  return TYPE_VOID;
	} else if(STRCMP(name,POINTER)){
	  return TYPE_POINTER;
	} else if(STRCMP(name,DOUBLE)){
	  return TYPE_DOUBLE;
	} else if(STRCMP(name,FLOAT)){
	  return TYPE_FLOAT;
	} else if(STRCMP(name,STRUCT)){
	  return TYPE_STRUCT;
	} else if(STRCMP(name,UNION)){
	  return TYPE_UNION;
	} else if(STRCMP(name,COMP_DEF)){
	  return conv_type(env,cenv,car(cdr(car(cdr(type_lst)))),lst);
	} else if(STRCMP(name,ARRAY)){
	  if(IS_NULL_LIST(lst)){
		return TYPE_ARRAY;
	  }
	  return conv_type(env,cenv,cdr(type_lst),lst);
	} else {
	  sym = lookup_obj(env,name);
	  if(!sym){
		sym = lookup_obj(cenv,name);
	  }

	  if(!sym){
		printf("name : %s\n",name);
		exit(1);
	  }

	  switch(sym->obj.type){
	  case TYPE_COMPOUND:
		return conv_compound_type((compound_def_t *)sym);
	  case TYPE_FUNCTION:
		return TYPE_FUNC;
	  default:
		switch(SYMBOL_GET_TYPE(sym)){
		case TYPE_ENUM:
		  return SYMBOL_GET_TYPE(sym);
		  break;
		case TYPE_TYPE:
		  com = lookup_obj(cenv,car(tail(SYMBOL_GET_TYPE_LST(sym))));
		  if(com){
			return conv_compound_type(com);
		  }
		  return conv_type(env,cenv,SYMBOL_GET_TYPE_LST(sym),lst);
		  break;
		default:
		  exit(1);
		  break;
		}
	  }
	  exit(1);
	}
  }
}

type_t conv_compound_type(compound_def_t *com){

  switch(COMPOUND_TYPE_GET_TYPE(com)){
  case STRUCT_COMPOUND_TYPE:
	return TYPE_STRUCT;
	break;
  case UNION_COMPOUND_TYPE:
	return TYPE_UNION;
	break;
  }
}

int calc_array_size(list_t *lst){

  list_t *p;
  string_t name;
  int size;

#ifdef __DEBUG__
  printf("calc_array_size\n");
#endif

  size = 0;
  p = lst;
  while(IS_NOT_NULL_LIST(p)){
	name = car(p);
	if(STRCMP(name,ARRAY)){
	  p = cdr(p);
	  size += *(integer_t *)car(p);
	  p = cdr(p);
	} else {
	  exit(1);
	}
  }

  return size;
}

bool_t is_integertype(list_t *lst){

  list_t *l;
  string_t name;

#ifdef __DEBUG__
  printf("is_integertype\n");
#endif
  name = car(lst);
  if(STRCMP(name,POINTER)){
	return TRUE;
  }

  l = tail(lst);
  name = car(l);
  if(STRCMP(name,INT)
	 || (STRCMP(name,SHORT))
	 || (STRCMP(name,LONG))
	 || (STRCMP(name,CHAR))){
	return TRUE;
  }

  return FALSE;
}

bool_t is_global_var(list_t *lst){

  string_t name;
  if(!IS_SYMBOL(lst)){
	return FALSE;
  }
  
  name = car(lst);
  if(STRCMP(name,GLOBAL_VAR)){
	return TRUE;
  }
  
  return FALSE;
}

bool_t is_compound_type(list_t *lst){

  string_t name;
#ifdef __DEBUG__
  printf("is_compound_type\n");
#endif

  if(!IS_SYMBOL(lst)){
	return FALSE;
  }

  name = car(lst);
  if(STRCMP(name,STRUCT) || STRCMP(name,UNION)){
	return TRUE;
  }

  return FALSE;
}

compound_def_t *get_comp_obj(env_t *env,env_t *cenv,string_t name){

  object_t *obj;
  symbol_t *sym;
  list_t *l;

  obj = lookup_obj(cenv,name);
  if(obj){
	switch(OBJ_GET_TYPE(obj)){
	case TYPE_SYMBOL:
	  sym = (symbol_t *)obj;
	  switch(SYMBOL_GET_TYPE(sym)){
	  case TYPE_TYPE:
		l = tail(SYMBOL_GET_TYPE_LST(sym));
		if(IS_NULL_LIST(l)){
		  return NULL;
		}
		return get_comp_obj(env,cenv,car(l));
		break;
	  default:
		exit(1);
	  }
	case TYPE_COMPOUND:
	  return (compound_def_t *)obj;
	  break;
	}
  }

  obj = lookup_obj(env,name);
  if(obj){
	switch(OBJ_GET_TYPE(obj)){
	case TYPE_SYMBOL:
	  sym = (symbol_t *)obj;
	  switch(SYMBOL_GET_TYPE(sym)){
	  case TYPE_TYPE:
		l = tail(SYMBOL_GET_TYPE_LST(sym));
		if(IS_NULL_LIST(l)){
		  return NULL;
		}
		return get_comp_obj(env,cenv,car(l));
		break;
	  default:
		exit(1);
	  }
	}
  }

  return NULL;
}

string_t concat_strs(string_t s1,string_t s2){

  int len;
  char *s3;

  len = strlen(s1) + 1 + strlen(s2) + NULL_LEN;
  s3 = malloc(sizeof(char) * len);
  strcat(s3,s1);
  strcat(s3,".");
  strcat(s3,s2);

  return s3;
}

int get_obj_size(object_t *obj){

  int size;
  switch(OBJ_GET_TYPE(obj)){
  case TYPE_VALUE:
	size = ((value_t *)obj)->size;
	break;
  case TYPE_SYMBOL:
	size = ((symbol_t *)obj)->size;
	break;
  case TYPE_OPE:
	size = ((operator_t *)obj)->size;
  default:
	break;
  }

  return size;
}
