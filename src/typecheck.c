// 
//
//  Copyright 2019 Shota Hisai. Released under the MIT license.
//
//
#include "typecheck.h"
#include "common.h"

bool_t check_floattype(env_t *env,list_t *l,list_t *r){

  if(IS_DECIMAL(l)){
    return TRUE;
  }

  if(IS_SYMBOL(l)){
    type_t type = *(type_t *)car(lookup(env,l));
    if(type == TYPE_FLOAT){
      return TRUE;
    }
  }

  return ((LIST_GET_TYPE(l) == DECIMAL) || (LIST_GET_TYPE(r) == DECIMAL));
}

bool_t check_type_lst(list_t *lst1,list_t *lst2){
  
  list_t *p;
  list_t *q;

  if(lst1 == NULL || lst2 == NULL){
    return FALSE;
  }

  if(IS_NULL_LIST(lst1) || IS_NULL_LIST(lst2)){
    return FALSE;
  }

  p = lst1;
  q = lst2;

  while(IS_NOT_NULL_LIST(p) || IS_NOT_NULL_LIST(q)){
    if(!STRCMP((string_t)car(p),(string_t)car(q))){
      return FALSE;
    }
  }

  return TRUE;
}
