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

char* make_label(){
  
  static int c = 0;
  char buf[256];
  char *str;
  sprintf(buf,".L%d",c++);
  str = mem(strlen(buf) + 1);
  strcpy(str,buf);

  return str;
}

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

bool_t is_inttype(type_t type){

  int flag;
#ifdef __DEBUG__
  printf("is_inttype\n");
#endif
  switch(type){
  case TYPE_CHAR:
  case TYPE_INT:
  case TYPE_SHORT:
  case TYPE_LONG:
    flag = TRUE;
    break;
  default:
    flag = FALSE;
    break;
  }

  return flag;
}

bool_t  is_function(type_t type){
  return TYPE_FUNCTION == type;
}

type_t util_get_type(list_t *lst){
  return (*(type_t *)car(cdr(cdr(lst))));
}

bool_t is_float(symbol_t *sym){

  if(!sym){
    return FALSE;
  }

  if((SYMBOL_GET_TYPE(sym) == TYPE_FLOAT)
     || (SYMBOL_GET_TYPE(sym) == TYPE_DOUBLE)){
    return TRUE;
  } else {
    return FALSE;
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

bool_t is_deref(list_t *lst){

  if(!IS_SYMBOL(lst)){
    return FALSE;
  }
  
  return STRCMP(car(lst),"*");
}

bool_t is_variable_kind(symbol_t *sym){
  
  if(!sym){
    return FALSE;
  }
  
  return KIND_VARIABLE == SYMBOL_GET_KIND(sym);
}

bool_t is_nedd_cast(list_t *lst){
  return *(kind_t *)car(lst) == KIND_VARIABLE;
}

bool_t is_function_pointer(list_t *lst){
  return (3 <= length_of_list(lst)) && (IS_LIST(tail(lst)));
}

bool_t is_array(list_t *lst){
  return (2 <= length_of_list(lst)) && (IS_LIST(cdr(lst)));
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

type_t gen_type(list_t *lst){
  
  type_t type;
  
  if(STRCMP(INT,car(lst))){
    type = TYPE_INT;
  } else if(STRCMP(CHAR,car(lst))){
    type = TYPE_CHAR;
  } else if(STRCMP(FLOAT,car(lst))){
    type = TYPE_FLOAT;
  } else if(STRCMP(DOUBLE,car(lst))){
    type = TYPE_DOUBLE;
  } else if(STRCMP(SHORT,car(lst))){
    type = TYPE_SHORT;
  } else if(STRCMP(LONG,car(lst))){
    type = TYPE_LONG;
  } else if(STRCMP(STRUCT,car(lst))){
    type = TYPE_STRUCT;
  } else {
    type = TYPE_UNDEFINE;
  }
  
  return type;
}

int calc_align(int n,int m){
  int rem = n % m;
  return (rem == 0) ? n : n - rem + m;
}

bool_t is_static(list_t *lst){
  
  if(!IS_INTEGER(lst)){
    return FALSE;
  }
  
  return INTERNAL == *(access_type *)car(lst);
}

bool_t is_extern(list_t *lst){
  
  if(!IS_INTEGER(lst)){
    return FALSE;
  }

  return EXTERNAL == *(access_type *)car(lst);
}

bool_t is_typdef(list_t *lst){
  
  if(!IS_INTEGER(lst)){
    return FALSE;
  }
  
  return (TYPEDEF == *(access_type *)car(lst));
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
