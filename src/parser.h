// 
//
//  Copyright 2019 Shota Hisai. Released under the MIT license.
//
//
#ifndef __INCLUDE__PARSER__
#define __INCLUDE__PARSER__
#include "list.h"
#include "lex.h"
#include "set.h"

typedef struct parser_t parser_t;
#define PARSER_GET_LEX(p) p->lex
#define PARSER_SET_LEX(p,l) p->lex = l
#define PARSER_SET_VAR_LST(p,l)  p->var_lst = l
#define PARSER_GET_VAR_LST(p)  p->var_lst
#define PARSER_GET_SET(p)      p->set

struct parser_t {
  lexer_t *lex;
  list_t *var_lst;
  set_t *set;
  int cnt;
};

parser_t *parser_create();
void parser_init(parser_t *parser);
list_t *parser_parse(parser_t *parser);
list_t *parser_constant_texts(parser_t *parser);
void parser_set_src(parser_t *parser,char *src);

#endif


