// 
//
//  Copyright 2019 Shota Hisai. Released under the MIT license.
//
//
#ifndef __INCLUDE__ENV__
#define __INCLUDE__ENV__
#include "list.h"
#include "env.h"
#include "type.h"
#include "util.h"

struct env_t {
  list_t *table[TABLE_SIZE];
  env_t *next;
};

env_t *make_env();
env_t *extend_env(env_t *env);
void insert_obj(env_t *env,string_t name,void *obj);
void *lookup_obj(env_t *env,string_t name);

#endif
