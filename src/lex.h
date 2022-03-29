// 
//
//  Copyright 2019 Shota Hisai. Released under the MIT license.
//
//
#ifndef __INCLUDE__LEX__
#define __INCLUDE__LEX__
#include <stdio.h>
#include "common.h"
#include "type.h"
#include "file.h"

enum token_type_t {
  TOKEN_NUMBER,
  TOKEN_FLOAT,
  TOKEN_HEX,
  TOKEN_STRING,
  TOKEN_CHARACTER,
  TOKEN_LETTER,
  TOKEN_ADD,
  TOKEN_SUB,
  TOKEN_MUL,
  TOKEN_DIV,
  TOKEN_NONE,
  TOKEN_RPAREN,
  TOKEN_LPAREN,
  TOKEN_ASSIGN,
  TOKEN_COMMA,
  TOKEN_RET,
  TOKEN_LBRACE,
  TOKEN_RBRACE,
  TOKEN_SEMI_COLON,
  TOKEN_COLON,
  TOKEN_IF,
  TOKEN_ELSE,
  TOKEN_AND,
  TOKEN_OR,
  TOKEN_BOOL_AND,
  TOKEN_BOOL_OR,
  TOKEN_EQUAL,
  TOKEN_NOT_EQUAL,
  TOKEN_LESS,
  TOKEN_GREATER,
  TOKEN_LESS_EQUAL,
  TOKEN_GREATER_EQUAL,
  TOKEN_NOT,
  TOKEN_DOT,
  TOKEN_DOUBLE_QUOTE,
  TOKEN_WHILE,
  TOKEN_CHAR,
  TOKEN_LBRACKET,
  TOKEN_RBRACKET,
  TOKEN_FOR,
  TOKEN_BREAK,
  TOKEN_INT,
  TOKEN_DOUBLE,
  TOKEN_VOID,
  TOKEN_SIZE_OF,
  TOKEN_INCREMENT_BY_ONE,
  TOKEN_DECREMENT_BY_ONE,
  TOKEN_INCREMENT,
  TOKEN_DECREMENT,
  TOKEN_MUL_INCREMENT,
  TOKEN_DIV_DECREMENT,
  TOKEN_STRUCT,
  TOKEN_ENUM,
  TOKEN_TYPEDEF,
  TOKEN_INCLUDE,
  TOKEN_SHAPE,
  TOKEN_DEFINE,
  TOKEN_SWITCH,
  TOKEN_CASE,
  TOKEN_DEFAULT,
  TOKEN_ARROW_OP,
  TOKEN_CONTINUE,
  TOKEN_STATIC,
  TOKEN_EXTERN,
  TOKEN_CONST,
  TOKEN_IFDEF,
  TOKEN_IFNDEF,
  TOKEN_ELIF,
  TOKEN_ENDIF,
  TOKEN_UNDEF,
  TOKEN_REGISTER,
  TOKEN_AUTO,
  TOKEN_UNION,
  TOKEN_NEWLINE,
  TOKEN_GOTO,
  TOKEN_SPACE,
  TOKEN_VTAB,
  TOKEN_TAB,
  TOKEN_ARGMENT_LIST,
  TOKEN_DEFINED,
  TOKEN_ERROR,
  TOKEN_SHORT,
  TOKEN_LONG,
  TOKEN_TILDE,
  TOKEN_BIT_LEFT_SHIFT,
  TOKEN_BIT_RIGHT_SHIFT,
  TOKEN_XOR,
  TOKEN_BACK_SLASH,
  TOKEN_SIGNED,
  TOKEN_UNSIGNED,
  TOKEN_MOD,
  TOKEN_VOLATILE,
  TOKEN_RESTRICT,
  TOKEN_QUESTION,
  TOKEN_INLINE,
  TOKEN_ATTRIBUTE,
  TOKEN_DO,
  TOKEN_EOT,
  TOKEN_ASM
};

struct token_t {
  token_type_t type;
  char *str;
  int line_no;
  string_t name;
};

struct lexer_t{
  list_t *lst;
  token_t *t;
  char *src;
  char *cur;
  char *str;
  char eoc;
  int line_no;
  file_t *file;
};

#define LEXER_GET_LST(l)      l->lst
#define LEXER_SET_LST(lex,l)  lex->lst = l 
#define LEXER_GET_LINE_NO(l)  l->line_no
#define LEXER_GET_NAME(l)     FILE_GET_NAME(l->file)
#define LEXER_SET_NAME(l,f)     l->file = f

#define TOKEN_GET_TYPE(t) t->type
#define TOKEN_GET_STR(t) t->str
#define TOKEN_GET_LINE_NO(t) t->line_no
#define TOKEN_GET_NAME(t)    t->name

token_t *create_token(token_type_t type,char *str,int line_no,string_t name);

lexer_t *create_lexer();
void lexer_init(lexer_t *l);
void lexer_set_src(lexer_t *l,char *src);
void lexer_set_eoc(lexer_t *l,char ch);
token_t *scan(lexer_t *lex);
token_t *scan_by_no_skip(lexer_t *lex);
void put_token(lexer_t *l,token_t *t);
token_t *lexer_get_token(lexer_t *l);
void lexer_put_token(lexer_t *l,token_t *t);
char read_ch(lexer_t *l);

#endif

