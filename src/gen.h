// 
//
//  Copyright 2019 Shota Hisai. Released under the MIT license.
//
//
#ifndef __INCLUDE__GEN__
#define __INCLUDE__GEN__
#include "list.h"
#include "env.h"
#include "enumdef.h"
#include "set.h"

typedef struct gen_info_t gen_info_t;
#define GEN_INF_SET_FILE(gi,f)   gi->output_file = f
#define GEN_INFO_SET_SRC(gi,s)   gi->src = s
#define GEN_INFO_GET_SRC(gi)     gi->src
#define GEN_INFO_SET_SET(gi,s)   gi->set = s
#define GEN_INFO_GET_SET(gi)   gi->set

struct gen_info_t {
  file_t *src;
  file_t *output_file;
  int stack_pos;
  int pos;
  int localarea;
  int no_align_localarea;
  bool_t flag_of_assign;
  int enum_value;
  bool_t call_flag;
  map_t *map;
  int label;
  int clabel;
  int no_name_label_no;
  set_t *set;
  bool_t eval_type;
  list_t *lhs_type;
  int offset_onstack;
  type_t assign_type;
  int int_regs;
  int float_regs;
  int num_of_gp;
  int num_of_fp;
  string_t func_name;
  list_t *lst_of_sv;
  list_t *args;
  bool_t flag_of_global_assign;
};

gen_info_t *create_gen_info();
list_t *gen(gen_info_t *gi,env_t *env,env_t *cenv,list_t *lst);
void delete_gen_info(gen_info_t *gi);
void init_stack_pos(gen_info_t *gi);

#endif

