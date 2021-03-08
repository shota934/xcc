// 
//
//  Copyright 2019 Shota Hisai. Released under the MIT license.
//
//
#ifndef __INCLUDE__SYMBOL__
#define __INCLUDE__SYMBOL__
#include "type.h"
#include "obj.h"

#define SYMBOL_SET_VAR_TYPE(s,t) s->var_type = t
#define SYMBOL_SET_TYPE(s,t) s->type = t
#define SYMBOL_SET_OFFSET(s,o) s->offset = o
#define SYMBOL_SET_SIZE(s,si)  s->size = si
#define SYMBOL_SET_TYPE_LST(s,l) s->type_lst = l
#define SYMBOL_SET_SCOPE(s,sc) s->scope = sc

#define SYMBOL_GET_VAR_TYPE(s)  s->var_type
#define SYMBOL_GET_TYPE(s)      s->type
#define SYMBOL_GET_OFFSET(s)    s->offset
#define SYMBOL_GET_SIZE(s)      s->size
#define SYMBOL_GET_TYPE_LST(s)  s->type_lst
#define SYMBOL_GET_SCOPE(s)     s->scope

struct symbol_t {
  object_t obj_type;
  list_t *type_lst;
  scope_t scope;
  type_t type;
  int offset;
  int size;
};

symbol_t *create_symbol();

#endif
