// 
//
//  Copyright 2019 Shota Hisai. Released under the MIT license.
//
//
#ifndef __INCLUDE__EMIT__
#define __INCLUDE__EMIT__
#include <stdio.h>
#include "list.h"
#include "env.h"
#include "file.h"
#include "map.h"

typedef struct gen_info_t gen_info_t;
#define GEN_INF_SET_FILE(ei,f)   ei->output_file = f

struct gen_info_t {
  file_t *output_file;
  int stack_pos;
  int off_set;
  int off_set_index;
  int assing_flag;
  int array_assing;
  int off_set_struct;
  int enum_value;
  int no_name_label_no;
  map_t *map;
};

gen_info_t *create_gen_info();
list_t *gen(gen_info_t *ei,env_t *env,list_t *lst,bool_t recursive_flag);
void delete_gen_info(gen_info_t *ei);

#endif
