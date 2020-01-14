// 
//
//  Copyright 2019 Shota Hisai. Released under the MIT license.
//
//
#include <stdio.h>
#include "set.h"
#include "list.h"
#include "mem.h"

set_t *set_create(){

  set_t *set;

  set = mem(sizeof(set_t));
  set->lst = make_null();
  set->size = 0;
  
  return set;
}

void set_add_obj(set_t *set,string_t obj){

  list_t *lst;
  set->lst = add_symbol(set->lst,obj);
  set->size++;
  
  return;
}

int set_find_obj(set_t *set,string_t obj){

  list_t *p;
  if(!obj){
    return FALSE;
  }
  
  p = set->lst;
  while(IS_NOT_NULL_LIST(p)){
    if(STRCMP(obj,(string_t)car(p))){
      return TRUE;
    }
    p = cdr(p);
  }

  return FALSE;
}
