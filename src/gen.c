// 
//
//  Copyright 2019 Shota Hisai. Released under the MIT license.
//
//
#include <stdio.h>
#include "gen.h"
#include "mem.h"

gen_info_t *create_gen_info(){

  gen_info_t *gi;
#ifdef __DEBUG__
  printf("create_gen_info\n");
#endif

  gi = mem(sizeof(gen_info_t));
  gi->stack_pos = 0;
  gi->offset = 0;

  return gi;
}

void init_gen_info(gen_info_t *gi){

#ifdef __DEBUG__
  printf("init_gen_info\n");
#endif
  gi->stack_pos = 0;
  gi->offset = 0;

  return;
}

list_t *gen(gen_info_t *gi,env_t *env,list_t *lst){

  list_t *val;
#ifdef __DEBUG__
  printf("gen\n");
#endif

  return val;
}

void delete_gen_info(gen_info_t *gi){

  fre(gi);

  return;
}


