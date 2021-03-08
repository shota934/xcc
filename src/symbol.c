// 
//
//  Copyright 2019 Shota Hisai. Released under the MIT license.
//
//
#include "symbol.h"
#include "mem.h"
#include "common.h"
#include "list.h"

symbol_t *create_symbol(){

  symbol_t *sym;
  
  sym = mem(sizeof(symbol_t));
  sym->obj_type.type = TYPE_SYMBOL;
  sym->scope = UNDEFINED;
  sym->size = 0;
  sym->offset = 0;
  sym->type_lst = make_null();
  
  return sym;
}


