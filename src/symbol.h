// 
//
//  Copyright 2019 Shota Hisai. Released under the MIT license.
//
//
#ifndef __INCLUDE__SYMBOL__
#define __INCLUDE__SYMBOL__
#include "type.h"
#include "obj.h"
#include "env.h"

#define SYMBOL_SET_TYPE(s,t) s->type = t
#define SYMBOL_SET_OFFSET(s,o) s->offset = o
#define SYMBOL_SET_SIZE(s,si)  s->size = si
#define SYMBOL_SET_TYPE_LST(s,l) s->type_lst = l
#define SYMBOL_SET_SCOPE(s,sc) s->obj.scope = sc
#define SYMBOL_SET_NAME(s,n) s->name = n
#define SYMBOL_SET_ARRAY_TYPE(s,t)  s->array.type = t;

#define SYMBOL_GET_OFFSET(s)    s->offset
#define SYMBOL_GET_SIZE(s)      s->size

#define SYMBOL_GET_TYPE(s)  s->type
#define SYMBOL_GET_TYPE_LST(s)  s->type_lst
#define SYMBOL_GET_SCOPE(s) s->obj.scope
#define SYMBOL_GET_RET_LST(s) s->func.ret_type
#define SYMBOL_GET_NAME(s) s->name
#define SYMBOL_GET_ARRAY_TYPE(s)  s->array.type
#define SYMBOL_SET_STATIC_VAR(s,n) s->static_var.name = n
#define SYMBOL_GET_STATIC_VAR(s)   s->static_var.name

struct symbol_t {
  object_t obj;
  list_t *type_lst;
  type_t type;
  int offset;
  int size;
  env_t *env;
  string_t name;
  struct {
	list_t *ret_type;
  } func;

  struct {
	type_t type;
  } array;

  struct {
	string_t name;
  } static_var;
};

symbol_t *create_symbol(list_t *lst);
list_t *get_ret_type(list_t *lst);

#endif
