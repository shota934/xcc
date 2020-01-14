// 
//
//  Copyright 2019 Shota Hisai. Released under the MIT license.
//
//
#ifndef __INCLUDE__MAP__
#define __INCLUDE__MAP__
#include "list.h"
#include "common.h"
#include "type.h"

struct map_t {
  list_t *objs[TABLE_SIZE];
  int size;
};

#define MAP_GET_SIZE(m) m->size

map_t *map_create();
void map_put(map_t *map,string_t key,void *obj);
void *map_get(map_t *map,string_t key);
void map_remove(map_t *map,string_t key);

#endif
