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

#define COMPOUND_TYPE_SET_TYPE(c,t)     c->type = t;
#define COMPOUND_TYPE_SET_MEMBERS(c,m)  c->members = m
#define COMPOUND_TYPE_SET_SIZE(c,s)     c->size = s

#define COMPOUND_TYPE_GET_TYPE(c,t)     c->type;
#define COMPOUND_TYPE_GET_MEMBERS(c)    c->members
#define COMPOUND_TYPE_GET_SIZE(c)       c->size

struct compound_def_t {
  object_t obj_type;
  compound_type_t type;
  list_t *members;
  int size;
};

compound_def_t *create_compound_type_def();

#endif
