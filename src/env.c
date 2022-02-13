// 
//
//  Copyright 2019 Shota Hisai. Released under the MIT license.
//
//
#include "env.h"
#include "mem.h"
#include "common.h"
#include "util.h"

env_t *genv = NULL;

#define NEXT_ENV(e) e->next
#define IS_TOP_LEVEL_ENV(e) (e == e->next)

static void init_env(env_t *env);
static env_t *alloc_env();

env_t *make_genv(){
  
  if(!genv){
    genv = alloc_env();
    init_env(genv);
    genv->next = genv;
  }
  
  return genv;
}

env_t *make_env(){

  env_t *env;
  env = alloc_env();
  init_env(env);
  env->next = env;
  
  return env;
}

env_t *extend_env(env_t *env){

  env_t *new_env;

  new_env = alloc_env();
  init_env(new_env);
  new_env->next = env;
  
  return new_env;
}

void insert_obj(env_t *env,string_t name,void *obj){
  
  int hash_val;
  list_t *lst;
  
  hash_val = hash(name);
  lst = cons(make_null(),obj);
  lst = add_symbol(lst,name);
  lst = add_list(make_null(),lst);
  env->table[hash_val] = concat(lst,env->table[hash_val]);

  return;
}

void *lookup_obj(env_t *env,string_t name){
  
  list_t *elst;
  env_t *e;
  int hash_val;

  hash_val = hash(name);
  e = env;
  while(TRUE){
    elst = e->table[hash_val];
    while(IS_NOT_NULL_LIST(elst)){
      list_t *n = car(elst);
      if(STRCMP(car(n),name)){
		return car(cdr(car(elst)));
      } else {
		elst = cdr(elst);
      }
    }

    if(IS_TOP_LEVEL_ENV(e) && IS_NULL_LIST(elst)){
      break;
    }
    e = NEXT_ENV(e);
  }
  
  return NULL;
}

static void init_env(env_t *env){

  int i = 0;
  
  env->next = NULL;
  for(i = 0; i < (int)(sizeof(env->table) / sizeof(env->table[0])); i++){
    env->table[i] = make_null();
  }
  
  return;
}

static env_t *alloc_env(){
  return mem(sizeof(env_t));
}

