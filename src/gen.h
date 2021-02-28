// 
//
//  Copyright 2019 Shota Hisai. Released under the MIT license.
//
//
#ifndef __INCLUDE__GEN__
#define __INCLUDE__GEN__
#include "list.h"
#include "env.h"

typedef struct gen_info_t gen_info_t;
#define GEN_INF_SET_FILE(gi,f)   gi->output_file = f
struct gen_info_t {
  file_t *output_file;
  int stack_pos;
  int offset;
};

gen_info_t *create_gen_info();
list_t *gen(gen_info_t *gi,env_t *env,list_t *lst);
void delete_gen_info(gen_info_t *gi);

#endif
