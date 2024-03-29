// 
//
//  Copyright 2019 Shota Hisai. Released under the MIT license.
//
//
#include <stdio.h>
#include "symbol.h"
#include "mem.h"
#include "common.h"
#include "list.h"
#include "dump.h"
#include "tych.h"

#define FUNC "FUNC"

symbol_t *create_symbol(list_t *lst){

  symbol_t *sym;
  
  sym = mem(sizeof(symbol_t));
  sym->obj.type = TYPE_SYMBOL;
  sym->obj.scope = UNDEFINED;
  sym->obj.next = NULL;
  sym->size = 0;
  sym->offset = 0;
  sym->type_lst = lst;
  sym->name = NULL;
  sym->func.ret_type = get_ret_type(lst);
  
  return sym;
}

list_t *get_ret_type(list_t *lst){

  list_t *p;
  string_t name;

#ifdef __DEBUG__
  printf("get_ret_type\n");
#endif

  p = lst;
  while(IS_NOT_NULL_LIST(p)){
	if(IS_SYMBOL(p)){
	  name = (string_t )car(p);
	  if(STRCMP(name,FUNC)){
		return cdr(cdr(p));
	  }
	}
	p = cdr(p);
  }

  return make_null();
}
