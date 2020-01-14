// 
//
//  Copyright 2019 Shota Hisai. Released under the MIT license.
//
//
#ifndef  __COMPILE__INCLUDE__
#define  __COMPILE__INCLUDE__
#include "type.h"

#define COM_GET_SIZE(c)  c->size
#define COM_GET_SRC_INFO_LST(c) c->src_info_lst
#define COM_GET_STACK(c) c->stack
#define COM_SET_SRC_INFO(c,s) c->src_info = s
#define COM_GET_SRC_INFO(c)   c->src_info
#define COM_GET_MACROS(c)     c->macros
#define COM_GET_STD_PATH_LST(c) c->std_path_lst

struct compile_info_t {
  list_t *src_info_lst;
  stack_ty *stack;
  source_info_t *src_info;
  map_t *macros;
  list_t *std_path_lst;
  int size;
};

compile_info_t *create_compile_info();
void add_src_info_lst(compile_info_t *com,source_info_t *src_info);


#endif
