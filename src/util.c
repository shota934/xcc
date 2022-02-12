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
#include "env.h"
#include "compound_type.h"

static bool_t is_func_qualifier(list_t *lst);
static type_t conv_compound_type(symbol_t *sym);

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

bool_t is_decimaltype(type_t type){
  return (TYPE_FLOAT == type) || (TYPE_DOUBLE == type);
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

bool_t  is_function(type_t type){
  return TYPE_FUNCTION == type;
}

type_t util_get_type(list_t *lst){
  return (*(type_t *)car(cdr(cdr(lst))));
}

bool_t is_float(list_t  *lst){

  string_t type;

  type = car(lst);
  if(STRCMP(type,FLOAT)
	 || STRCMP(type,DOUBLE)){
	return TRUE;
  }

  return FALSE;
}

bool_t is_structtype(list_t *lst){

  if(!IS_SYMBOL(lst)){
    return FALSE;
  }
  
  return STRCMP(STRUCT_ALLOC,car(lst));
}

bool_t is_compund_type(list_t *lst){

  string_t symbol;
  bool_t flag;
  if(!IS_SYMBOL(lst)){
    return FALSE;
  }
  
  flag = FALSE;
  symbol = car(lst);
  if(STRCMP(STRUCT_DEF,symbol)
     || STRCMP(STRUCT_DEF,symbol)){
    flag = TRUE;
  }
  
  return flag;
}

bool_t is_struct(symbol_t *sym){

  if(!sym){
    return FALSE;
  }

  return TYPE_STRUCT == SYMBOL_GET_TYPE(sym);
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

bool_t is_struct_ref(list_t *lst){

  if(!IS_SYMBOL(lst)){
    return FALSE;
  }

  return STRCMP(car(lst),"->");
}

bool_t is_struct_decl(list_t *lst){

  if(!IS_SYMBOL(lst)){
    return FALSE;
  }

  return STRCMP(car(lst),STRUCT_DECL);
}

bool_t is_deref(list_t *lst){

  if(!IS_SYMBOL(lst)){
    return FALSE;
  }
  
  return STRCMP(car(lst),"*");
}

bool_t is_nedd_cast(list_t *lst){
  return *(kind_t *)car(lst) == KIND_VARIABLE;
}

bool_t is_function_pointer(list_t *lst){
  return (3 <= length_of_list(lst)) && (IS_LIST(tail(lst)));
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

bool_t is_value(list_t *lst){
  return IS_LIST(lst);
}

bool_t is_name(list_t *lst){

  if(!IS_SYMBOL(lst)){
	return FALSE;
  }

  if(is_array(lst) || is_pointer(lst) || is_struct_ref(lst)){
	return FALSE;
  } else {
	return TRUE;
  }
}

kind_t select_type(list_t *lst){

  list_t *p;
  int cnt;

  p = lst;
  while(IS_LIST(p)){
    p = cdr(p);
  }

  cnt = 0;
  while(IS_SYMBOL(p)){
    if(!STRCMP(car(p),"*")){
      break;
    }
    p = cdr(p);
    cnt++;
  }
  
  return 0 < cnt ? KIND_POINTER : KIND_VARIABLE;
}

int calc_align(int n,int m){
  int rem = n % m;
  return (rem == 0) ? n : n - rem + m;
}

bool_t is_extern(list_t *lst){

  string_t name;
  name = (string_t)car(lst);
  if(STRCMP(EXTERN,name)){
	return TRUE;
  }

  return FALSE;
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

bool_t is_var_len_args(list_t *lst){
  
  list_t *arg;
  int flag;

  #ifdef __DEBUG__
  printf("is_var_len_args\n");
  #endif

  flag = FALSE;
  arg = cdr(lst);
  if(!IS_SYMBOL(arg)){
    exit(1);
  }

  if(STRCMP((string_t)car(arg),"...")){
    flag = TRUE;
  }
  
  return flag;
}

list_t *get_def_type_lst(list_t *lst){
  if(is_pointer(lst)){
	return cdr(lst);
  } else {
	return lst;
  }
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

bool_t is_compound_type(list_t *lst){

  if(STRCMP(car(lst),STRUCT)
	 || STRCMP(car(lst),UNION)){
	return TRUE;
  }

  return FALSE;
}

bool_t is_compound_alloc_type(list_t *lst){

  if(STRCMP(car(lst),STRUCT_ALLOC)
	 || STRCMP(car(lst),UNION_ALLOC)){
	return TRUE;
  }

  return FALSE;
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

type_t conv_type(env_t *env,list_t *type_lst,list_t *lst){

  string_t name;
  list_type_t type;

#ifdef __DEBUG__
  printf("conv_type\n");
#endif

  type = LIST_GET_TYPE(type_lst);
  switch(type){
  case NULL_LIST:
	exit(1);
	break;
  case INTEGER:
	return conv_type(env,cdr(type_lst),lst);
	break;
  case LIST:
	return conv_type(env,car(type_lst),lst);
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
	} else if(STRCMP(name,STRUCT_ALLOC)){
	  return TYPE_STRUCT;
	} else if(STRCMP(name,STRUCT)){
	  return TYPE_STRUCT;
	} else if(STRCMP(name,UNION_ALLOC)){
	  return TYPE_UNION;
	} else if(STRCMP(name,UNION)){
	  return TYPE_UNION;
	} else if(STRCMP(name,ARRAY)){
	  if(IS_NULL_LIST(lst)){
		return TYPE_ARRAY;
	  }
	  return conv_type(env,cdr(type_lst),lst);
	} else {
	  symbol_t *sym;
	  sym = lookup_obj(env,name);
	  if(!sym){
		exit(1);
	  }

	  if(sym->obj.type == TYPE_COMPOUND){
		return conv_compound_type(sym);
	  } else {
		switch(SYMBOL_GET_SYM_TYPE(sym)){
		case TYPE_ENUM:
		  return SYMBOL_GET_SYM_TYPE(sym);
		  break;
		case TYPE_TYPE:
		  return conv_type(env,SYMBOL_GET_TYPE_LST(sym),lst);
		  break;
		default:
		  printf("name : %s\n",name);
		  printf("TYPE : %d\n",SYMBOL_GET_SYM_TYPE(sym));
		  exit(1);
		  break;
		}
	  }
	  printf("name : %s\n",name);
	  exit(1);
	}
  }
}

type_t conv_compound_type(symbol_t *sym){

  compound_def_t *com;

  com = (compound_def_t *)sym;
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

