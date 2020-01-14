// 
//
//  Copyright 2019 Shota Hisai. Released under the MIT license.
//
//
#ifndef __INCLUDE__SET__
#define __INCLUDE__SET__
#include "type.h"

struct set_t {
  list_t *lst;
  int size;
};

#define SET_GET_SIZE(s) s->size

set_t *set_create();
void set_add_obj(set_t *set,string_t obj);
int set_find_obj(set_t *set,string_t obj);

#endif
