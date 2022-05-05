// 
//
//  Copyright 2019 Shota Hisai. Released under the MIT license.
//
//
#ifndef __INCLUDE__OPT_INFO__
#define __INCLUDE__OPT_INFO__
#include "type.h"

struct opt_info_t{
  bool_t only_preprocess;
  bool_t only_genassembler;
  bool_t only_gen_objfile;
  bool_t dumping_ast;
  bool_t file_name;
};

#define SET_ONLY_PREPROCESS(oi,f)    oi->only_preprocess = f
#define SET_ONLY_GENASSEMBLER(oi,f)  oi->only_genassembler = f
#define SET_ONLY_GEN_OBJFILE(oi,f)   oi->only_gen_objfile = f
#define SET_DUMPING_AST(oi,f)        oi->dumping_ast = f
#define SET_FILE_NAME(oi,f)          oi->file_name = f
#define IS_ONLY_PREPROCESS(oi)       oi->only_preprocess
#define IS_ONLY_GENASSEMBLER(oi)     oi->only_genassembler
#define IS_ONLY_GEN_OBJFILE(oi)      oi->only_gen_objfile
#define IS_DUMPING_AST(oi)           oi->dumping_ast
#define IS_FILE_NAME(oi)             oi->file_name

void init_optinfo(opt_info_t *opt_info);

#endif
