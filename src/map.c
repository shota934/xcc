// 
//
//  Copyright 2019 Shota Hisai. Released under the MIT license.
//
//
#include "map.h"
#include "mem.h"
#include "util.h"
#include "common.h"

map_t *map_create(){
  
  int i;
  map_t *m;

  m = mem(sizeof(map_t));

  for(i = 0; i < (int)(sizeof(m->objs) / sizeof(m->objs[0])); i++){
    m->objs[i] = make_null();
  }
  m->size = 0;

  return m;
}

void map_put(map_t *map,string_t key,void *obj){

  int hash_val;
  list_t *lst;
  list_t *pair;

  hash_val = hash(key);
  lst = map->objs[hash_val];
  pair = make_null();
  pair = cons(pair,obj);
  
  pair = add_symbol(pair,key);
  map->objs[hash_val] = concat(lst,add_list(make_null(),pair));
  map->size++;
  
  return;
}

void *map_get(map_t *map,string_t key){

  list_t *lst;
  int hash_val;

  hash_val = hash(key);
  lst = map->objs[hash_val];
  while(TRUE){
    if(IS_NULL_LIST(lst)){
      break;
    }
    
    if(STRCMP(car(car(lst)),key)){
      return car(cdr(car(lst)));
    }
    lst = cdr(lst);
  }

  return NULL;
}

void map_remove(map_t *map,string_t key){
  
  list_t *lst;
  list_t *pre;
  int hash_val;

  hash_val = hash(key);
  lst = map->objs[hash_val];
  pre = make_null();
  while(TRUE){
    if(IS_NULL_LIST(lst)){
      break;
    }
    
    if(STRCMP(car(car(lst)),key)){
	  if(IS_NULL_LIST(pre)){
		map->objs[hash_val] = cdr(lst);
	  } else {
		pre->next = cdr(lst);
	  }
	  map->size--;
      break;
    }
    pre = lst;
    lst = cdr(lst);
  }
  
  return;
}
