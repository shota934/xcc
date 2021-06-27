// 
//
//  Copyright 2019 Shota Hisai. Released under the MIT license.
//
//
#ifndef __INCLUDE__CPP__
#define __INCLUDE__CPP__
#include "type.h"
#include "lex.h"
#include "mem.h"
#include "com.h"

source_info_t *create_source_info();
void cpreprocess(compile_info_t *com,lexer_t *lexer,source_info_t *srcinfo);
void dump_token_sequences(compile_info_t *com);
void dump_token_lst(list_t *lst);

#endif
