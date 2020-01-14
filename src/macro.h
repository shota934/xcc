// 
//
//  Copyright 2019 Shota Hisai. Released under the MIT license.
//
//
#ifndef __INCLUDE__MACRO__
#define __INCLUDE__MACRO__
#include "type.h"

enum macro_type_t {
  MACRO_OBJECT,
  MACRO_LINE_FUNCTION
};

struct macro_t {
  macro_type_t type;
  list_t *param;
  list_t *body;
};

#define MACRO_SET_TYPE(m,t)     m->type = t
#define MACRO_GET_TYPE(m)       m->type
#define MACRO_SET_PARAM(m,p)    m->param = p
#define MACRO_GET_PARAM(m)      m->param
#define MACRO_SET_BODY(m,b)     m->body = b
#define MACRO_GET_BODY(m)       m->body
#define MACRO_SET_OBJ(m,o)      m->body = o
#define MACRO_GET_OBJ(m)        m->body

macro_t *macro_create();

#endif

