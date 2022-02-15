// 
//
//  Copyright 2019 Shota Hisai. Released under the MIT license.
//
//
#ifndef __INCLUDE__STRUCT__DEF__
#define __INCLUDE__STRUCT__DEF__
#include "type.h"
#include "list.h"
#include "obj.h"
#include "map.h"

#define COMPOUND_TYPE_SET_TYPE(c,t)     c->type = t
#define COMPOUND_TYPE_SET_SIZE(c,s)     c->size = s
#define COMPOUND_TYPE_SET_ENV(c,e)      c->env = e
#define COMPOUND_TYPE_SET_CENV(c,e)     c->cenv = e

#define COMPOUND_TYPE_GET_TYPE(c)       c->type
#define COMPOUND_TYPE_GET_SIZE(c)       c->size
#define COMPOUND_TYPE_GET_ENV(c)        c->env
#define COMPOUND_TYPE_GET_CENV(c)       c->env
#define COMPOUND_TYPE_GET_MEMBERS(c)        c->members

struct compound_def_t {
  object_t obj_type;
  compound_type_t type;
  int size;
  env_t *env;
  env_t *cenv;
  list_t *members;
};

compound_def_t *create_compound_type_def(type_type_t type,scope_t scope);

#endif
