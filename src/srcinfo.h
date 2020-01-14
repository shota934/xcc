// 
//
//  Copyright 2019 Shota Hisai. Released under the MIT license.
//
//
#include "type.h"

#define SOURCE_INFO_GET_FILE(s)    s->file
#define SOURCE_INFO_SET_TOKEN(s,t)   s->tk = t
#define SOURCE_INFO_SET_FILE(s,f)  s->f
#define SOURCE_INFO_GET_TOKEN(s)   s->tk
#define SOURCE_INFO_SET_SRC(s,src) s->src = src
#define SOURCE_INFO_SET_CUR(s,c)   s->cur = c
#define SOURCE_INFO_GET_SRC(s)     s->src
#define SOURCE_INFO_GET_CUR(s)     s->cur
#define SOURCE_INFO_GET_LST(s)     s->lst

struct source_info_t {
  list_t *lst;
  file_t *file;
  token_t *tk;
  char *src;
  char *cur;
  char *eoc;
  int line_no;
};

source_info_t *create_source_info(file_t *file,token_t *tk);
void load_lexinfo(source_info_t *src_info,lexer_t *lex);
void save_lexinfo(source_info_t *src_info,lexer_t *lex);
