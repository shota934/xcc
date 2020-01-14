// 
//
//  Copyright 2019 Shota Hisai. Released under the MIT license.
//
//
#include "optinfo.h"
#include "common.h"

void init_optinfo(opt_info_t *opt_info){

  SET_ONLY_PREPROCESS(opt_info,FALSE);
  SET_ONLY_GENASSEMBLER(opt_info,FALSE);
  SET_ONLY_GEN_OBJFILE(opt_info,FALSE);
  SET_DUMPING_AST(opt_info,FALSE);
  
  return;
}
