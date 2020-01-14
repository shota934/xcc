// 
//
//  Copyright 2019 Shota Hisai. Released under the MIT license.
//
//
#include "macro.h"
#include "mem.h"
#include "list.h"

macro_t *macro_create(){

  macro_t *macro;
  
  macro = mem(sizeof(macro_t));
  macro->param = make_null();
  macro->body = make_null();
  
  return macro;
}
