// 
//
//  Copyright 2019 Shota Hisai. Released under the MIT license.
//
//
#include "com.h"
#include "common.h"
#include "mem.h"
#include "stack.h"
#include "map.h"
#include "xcc.h"
#include "macro.h"
#include "lex.h"

static void add_built_in_macro(compile_info_t *com);
static void add_path(compile_info_t *compile_info);
static macro_t *create_macro(list_t *obj);

compile_info_t *create_compile_info(){
  
  compile_info_t *compile_info = mem(sizeof(compile_info_t));

  compile_info->token_lst = make_null();
  compile_info->src_info_lst = make_null();
  compile_info->stack = create_stack();
  compile_info->src_info = NULL;
  compile_info->macros = map_create();
  compile_info->std_path_lst = make_null();
  compile_info->size = 0;
  
  add_path(compile_info);
  add_built_in_macro(compile_info);

  return compile_info;
}

static void add_path(compile_info_t *compile_info){
  
  list_t *std_path_lst;

  std_path_lst = compile_info->std_path_lst;

  std_path_lst = concat(std_path_lst,cons(make_null(),INCLUDE_XCC));
  std_path_lst = concat(std_path_lst,cons(make_null(),INCLUDE_PATH));
  std_path_lst = concat(std_path_lst,cons(make_null(),INCLUDE_LINUX_PATH));
  std_path_lst = concat(std_path_lst,cons(make_null(),INCLUDE_GNU_LINUX_PATH));
  std_path_lst = concat(std_path_lst,cons(make_null(),INCLUDE_GNU_LINUX_LIB_PATH));
  
  compile_info->std_path_lst = std_path_lst;
  
  return;
}

static void add_built_in_macro(compile_info_t *com){

  map_t *map;

#ifdef __DEBUG__
  printf("add_built_in_macro\n");
#endif

  map = COM_GET_MACROS(com);
  map_put(map,X86_64,create_macro(cons(make_null(),create_token(TOKEN_NUMBER,TRUE_OBJ,0,NULL))));
  map_put(map,LP64,create_macro(cons(make_null(),create_token(TOKEN_NUMBER,TRUE_OBJ,0,NULL))));
  map_put(map,LINUX,create_macro(cons(make_null(),create_token(TOKEN_NUMBER,TRUE_OBJ,0,NULL))));
  map_put(map,STDC_VERSION,create_macro(cons(make_null(),create_token(TOKEN_NUMBER,"201112L",0,NULL))));
  
  return;
}
static macro_t *create_macro(list_t *obj){
  
  macro_t *macro;

#ifdef __DEBUG__
  printf("create_macro\n");
#endif

  macro = macro_create();
  MACRO_SET_TYPE(macro,MACRO_OBJECT);
  MACRO_SET_OBJ(macro,obj);
  
  return macro;
}

void add_src_info_lst(compile_info_t *com,source_info_t *src_info){

#ifdef __DEBUG__
  printf("add_src_info_lst\n");
#endif
  
  com->src_info_lst = cons(com->src_info_lst,src_info);
  com->size++;
  
  return;
}
