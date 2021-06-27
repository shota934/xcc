//
//
//  Copyright 2019 Shota Hisai. Released under the MIT license.
//
//
#ifndef __INCLUDE__ENUMDEF__
#define __INCLUDE__ENUMDEF__
#include "type.h"
#include "obj.h"

struct enumdef_t{
  object_t obj;
  int val;
  string_t enum_class;
};

#define ENUMDEF_GET_VAL(e) e->val

enumdef_t *enumdef_create(int val,string_t enum_class);

#endif
