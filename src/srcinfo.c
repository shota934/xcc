// 
//
//  Copyright 2019 Shota Hisai. Released under the MIT license.
//
//
#include "srcinfo.h"
#include "com.h"
#include "mem.h"
#include "lex.h"
#include "list.h"
#include "cpp.h"

source_info_t *create_source_info(file_t *file,token_t *tk){
  
  source_info_t *src_info = mem(sizeof(source_info_t));

#ifdef __DEBUG__
  printf("create_source_info\n");
#endif

  src_info->lst = make_null();
  src_info->file = file;
  src_info->tk = tk;
  src_info->src = NULL;
  src_info->cur = NULL;
  src_info->line_no = 0;
  src_info->file = file;
  
  return src_info;
}

void load_lexinfo(source_info_t *src_info,lexer_t *lex){

#ifdef ___DEBUG__
  printf("load_lexinfo\n");
#endif
  
  lex->lst = src_info->lst;
  lex->src = src_info->src;
  lex->cur = src_info->cur;
  lex->t = src_info->tk;
  lex->line_no = src_info->line_no;
  lex->file = src_info->file;
  
  return;
}

void save_lexinfo(source_info_t *src_info,lexer_t *lex){
  
  #ifdef ___DEBUG__
  printf("save_lexinfo\n");
  #endif
  src_info->lst = lex->lst;
  src_info->src = lex->src;
  src_info->src = lex->src;
  src_info->cur = lex->cur;
  src_info->tk = lex->t;
  src_info->line_no = lex->line_no;
  src_info->file = lex->file;

  return;
}

