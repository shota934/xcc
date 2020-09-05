// 
//
//  Copyright 2019 Shota Hisai. Released under the MIT license.
//
//
#ifndef __INCLUDE__SYMBOL__
#define __INCLUDE__SYMBOL__
#include "type.h"

#define SYMBOL_SET_VAR_TYPE(s,t) s->var_type = t
#define SYMBOL_SET_TYPE(s,t) s->type = t
#define SYMBOL_SET_OFFSET(s,o) s->offset = o
#define SYMBOL_SET_SIZE(s,si)  s->size = si
#define SYMBOL_SET_KIND(s,k)  s->kind = k
#define SYMBOL_SET_TYPE_LST(s,l) s->type_lst = l

#define SYMBOL_GET_VAR_TYPE(s)  s->var_type
#define SYMBOL_GET_TYPE(s)      s->type
#define SYMBOL_GET_OFFSET(s)    s->offset
#define SYMBOL_GET_SIZE(s)      s->size
#define SYMBOL_GET_KIND(s)      s->kind
#define SYMBOL_GET_TYPE_LST(s)  s->type_lst

struct symbol_t {
  list_t *type_lst;
  variable_type_t var_type;
  type_t type;
  int offset;
  int size;
  kind_t kind;
};

symbol_t *create_symbol();

#endif
