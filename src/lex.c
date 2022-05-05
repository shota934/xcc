// 
//
//  Copyright 2019 Shota Hisai. Released under the MIT license.
//
//
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "lex.h"
#include "mem.h"
#include "list.h"
#include "common.h"

#define SPACE        ' '
#define TAB          '\t'
#define VTAB         '\v'
#define NEW_LINE     '\n'
#define NULL_CHAR    '\0'
#define QUOTE        '\''
#define DOUBLE_QUOTE '\"'
#define UNDER_BAR    '_'

#define LEXER_GET_NEXT_CHAR(l) lexer_get_ch(l)
#define LEXER_GET_EOC(l) l->eoc
#define IS_EOC(l) ((*l->cur) == l->eoc)
#define IS_QUOTE(c)  (c == QUOTE)
#define IS_DOUBLE_QUOTE(c)  (c == DOUBLE_QUOTE)
#define IS_NOT_DOUBLE_QUOTE(c)  (c != DOUBLE_QUOTE)
#define IS_NEWLINE(c) (c == NEW_LINE)
#define IS_NEWLINE_TYPE(t) (t->type == TOKEN_NEWLINE)
#define IS_SPACE_TYPE(t)  (t->type == TOKEN_SPACE)

token_t keywords[] = {
  {TOKEN_ADD,"+"},
  {TOKEN_SUB,"-"},
  {TOKEN_MUL,"*"},
  {TOKEN_DIV,"/"},
  {TOKEN_LPAREN,"("},
  {TOKEN_RPAREN,")"},
  {TOKEN_LBRACE,"{"},
  {TOKEN_RBRACE,"}"},  
  {TOKEN_COMMA,","},
  {TOKEN_SEMI_COLON,";"},
  {TOKEN_ASSIGN,"="},
  {TOKEN_RET,"return"},
  {TOKEN_IF,"if"},
  {TOKEN_ELSE,"else"},
  {TOKEN_EQUAL,"=="},
  {TOKEN_NOT_EQUAL,"!="},
  {TOKEN_LESS,"<"},
  {TOKEN_GREATER,">"},
  {TOKEN_LESS_EQUAL,"<="},
  {TOKEN_GREATER_EQUAL,">="},
  {TOKEN_COLON,":"},
  {TOKEN_DOT,"."},
  {TOKEN_WHILE,"while"},
  {TOKEN_SIZE_OF,"sizeof"},
  {TOKEN_LBRACKET,"["},
  {TOKEN_RBRACKET,"]"},
  {TOKEN_BOOL_AND,"&&"},
  {TOKEN_BOOL_OR,"||"},
  {TOKEN_AND,"&"},
  {TOKEN_OR,"|"},
  {TOKEN_FOR,"for"},
  {TOKEN_BREAK,"break"},
  {TOKEN_INT,"int"},
  {TOKEN_VOID,"void"},
  {TOKEN_CHAR,"char"},
  {TOKEN_FLOAT,"float"},
  {TOKEN_DOUBLE,"double"},
  {TOKEN_INCREMENT_BY_ONE,"++"},
  {TOKEN_DECREMENT_BY_ONE,"--"},
  {TOKEN_INCREMENT,"+="},
  {TOKEN_DECREMENT,"-="},
  {TOKEN_MUL_INCREMENT,"*="},
  {TOKEN_DIV_DECREMENT,"/="},  
  {TOKEN_STRUCT,"struct"},
  {TOKEN_ENUM,"enum"},
  {TOKEN_TYPEDEF,"typedef"},
  {TOKEN_SHAPE,"#"},
  {TOKEN_SWITCH,"switch"},
  {TOKEN_CASE,"case"},
  {TOKEN_DEFAULT,"default"},
  {TOKEN_ARROW_OP,"->"},
  {TOKEN_CONTINUE,"continue"},
  {TOKEN_TYPEDEF,"typedef"},
  {TOKEN_EXTERN,"extern"},
  {TOKEN_STATIC,"static"},
  {TOKEN_CONST,"const"},
  {TOKEN_RESTRICT,"restrict"},
  {TOKEN_VOLATILE,"volatile"},
  {TOKEN_ELIF,"elif"},
  {TOKEN_ENDIF,"endif"},
  {TOKEN_UNDEF,"undef"},
  {TOKEN_REGISTER,"register"},
  {TOKEN_AUTO,"auto"},
  {TOKEN_UNION,"union"},
  {TOKEN_GOTO,"goto"},
  {TOKEN_SWITCH,"switch"},
  {TOKEN_CASE,"case"},
  {TOKEN_DEFAULT,"default"},
  {TOKEN_ARGMENT_LIST,"..."},
  {TOKEN_DEFINED,"defined"},
  {TOKEN_SHORT,"short"},
  {TOKEN_LONG,"long"},
  {TOKEN_NOT,"!"},
  {TOKEN_TILDE,"~"},
  {TOKEN_BIT_LEFT_SHIFT,">>"},
  {TOKEN_BIT_RIGHT_SHIFT,"<<"},
  {TOKEN_XOR,"^"},
  {TOKEN_BACK_SLASH,"\\"},
  {TOKEN_SIGNED,"signed"},
  {TOKEN_UNSIGNED,"unsigned"},
  {TOKEN_MOD,"%"},
  {TOKEN_QUESTION,"?"},
  {TOKEN_INLINE,"inline"},
  {TOKEN_ATTRIBUTE,"__attribute__"},
  {TOKEN_DO,"do"},
  {TOKEN_ASM,"asm"},
};

static char skip(lexer_t *lex);
static int is_comment(lexer_t *lex,char ch);
static int is_skip(char ch);
static int is_hex(char ch);
static int is_symbol(char ch);
static token_type_t search_keyword(char *text);
static char lexer_get_ch(lexer_t *lex);
static void lexer_put_ch(lexer_t *lex);

static token_t *scan_letter(lexer_t *lex);
static token_t *scan_number(lexer_t *lex,char ch);
static token_t *scan_float(lexer_t *lex);
static token_t *scan_string(lexer_t *lex);
static token_t *scan_char(lexer_t *lex);
static token_t *scan_operator(lexer_t *lex,char ch);

token_t *create_token(token_type_t type,char *str,int line_no,string_t name){
  
  token_t *t;
  
  t = mem(sizeof(token_t));
  t->str = str;
  t->type = type;
  t->line_no = line_no;
  t->name = name;
  
  return t;
}

lexer_t *create_lexer(){
  
  lexer_t *lexer = mem(sizeof(lexer_t));
  lexer_init(lexer);
  lexer_set_eoc(lexer,END_OF_CHAR);

  return lexer;
}

void lexer_init(lexer_t *l){

  l->lst = make_null();
  l->src = NULL;
  l->cur = NULL;
  l->str = NULL;
  l->eoc = INIT_VALUE;
  l->line_no = 1;
  l->file = NULL;
  
  return;
}

void lexer_set_src(lexer_t *l,char *src){

  l->src = src;
  l->cur = l->src; 
  l->t = NULL;

  return;
}

void lexer_set_eoc(lexer_t *l,char ch){

  l->eoc = ch;

  return;
}

token_t *scan(lexer_t *lex){
  
  token_t *t;
  char *text;
  char ch;

  if(lex->t){
    t = lex->t;
    lex->t = NULL;
  } else {
    
    ch = skip(lex);
    
    if(is_comment(lex,ch)){
      LEXER_GET_NAME(lex);
      return create_token(TOKEN_SPACE,"",lex->line_no,LEXER_GET_NAME(lex));
    } else {
    
      lex->str = lex->cur - 1;
      if(ch == NEW_LINE){
		t = create_token(TOKEN_NEWLINE,"\n",lex->line_no,LEXER_GET_NAME(lex));
		lex->line_no++;
      } else if(ch == 0x00){
		t = create_token(TOKEN_EOT,NULL,0,LEXER_GET_NAME(lex));
		lex->t = t;
      } else if(IS_QUOTE(ch)){
		t = scan_char(lex);
      } else if(IS_DOUBLE_QUOTE(ch)){
		t = scan_string(lex);
      } else if(isdigit(ch)){
		t = scan_number(lex,ch);
      } else if(isalpha(ch) || UNDER_BAR == ch){
		t = scan_letter(lex);
      } else {
		t = scan_operator(lex,ch);
    }
      lex->str = NULL;
    }
  }

  return t;
}

token_t *scan_by_no_skip(lexer_t *lex){

  token_t *t;
  char *text;
  char ch;

  if(lex->t){
    t = lex->t;
    lex->t = NULL;
  } else {
	ch = LEXER_GET_NEXT_CHAR(lex);
    if(is_comment(lex,ch)){
      LEXER_GET_NAME(lex);
      return create_token(TOKEN_SPACE,"",lex->line_no,LEXER_GET_NAME(lex));
    } else {
      lex->str = lex->cur - 1;
      if(ch == NEW_LINE){
		t = create_token(TOKEN_NEWLINE,"\n",lex->line_no,LEXER_GET_NAME(lex));
		lex->line_no++;
	  } else if(ch == SPACE){
		t = create_token(TOKEN_SPACE," ",lex->line_no,LEXER_GET_NAME(lex));
	  } else if(ch == TAB){
		t = create_token(TOKEN_TAB,"\t",lex->line_no,LEXER_GET_NAME(lex));
	  } else if(ch == VTAB){
		t = create_token(TOKEN_VTAB,"\v",lex->line_no,LEXER_GET_NAME(lex));
      } else if(ch == 0x00){
		t = create_token(TOKEN_EOT,NULL,0,LEXER_GET_NAME(lex));
		lex->t = t;
      } else if(IS_QUOTE(ch)){
		t = scan_char(lex);
      } else if(IS_DOUBLE_QUOTE(ch)){
		t = scan_string(lex);
      } else if(isdigit(ch)){
		t = scan_number(lex,ch);
      } else if(isalpha(ch) || UNDER_BAR == ch){
		t = scan_letter(lex);
      } else {
		t = scan_operator(lex,ch);
    }
      lex->str = NULL;
    }
  }

  return t;
}

void put_token(lexer_t *l,token_t *t){

  l->t = t;

  return;
}

token_t *lexer_get_token(lexer_t *l){
  
  token_t *t;
  list_t *lst;
  
  lst = l->lst;
  t = car(lst);
  
  while(IS_NEWLINE_TYPE(t) || IS_SPACE_TYPE(t)){
    lst = cdr(lst);
    t = car(lst);
  }
  
  l->lst = cdr(lst);
  
  return t;
}

static token_t *scan_letter(lexer_t *lex){

  char *text;
  char *end;
  token_type_t type;
  int len;
  char ch;

  ch = LEXER_GET_NEXT_CHAR(lex);
  while((isalpha(ch)) || (isdigit(ch)) || ch == UNDER_BAR){
    ch = LEXER_GET_NEXT_CHAR(lex);
  }
  
  end = lex->cur - 1;
  len = end - lex->str;
  text = mem(len + NULL_LEN);
  memset(text,INIT_VALUE,len + NULL_LEN);
  memcpy(text,lex->str,len);
  lex->cur = end;
  type = search_keyword(text);
  
  return create_token(type,text,lex->line_no,LEXER_GET_NAME(lex));
}

static token_t *scan_number(lexer_t *lex,char ch){
  
  char *text;
  char *end;
  int len;
  token_type_t type;
  char c;
  
  if(ch == '0'){
    c = LEXER_GET_NEXT_CHAR(lex);
    if(('X' == c) || ('x' == c)){
      type = TOKEN_HEX;
      while(is_hex(LEXER_GET_NEXT_CHAR(lex))){
		;
      }
    } else {
      type = TOKEN_NUMBER;
    }
  } else {
	type = TOKEN_NUMBER;
	while(TRUE){
	  c = LEXER_GET_NEXT_CHAR(lex);
	  if(c == '.'){
		return scan_float(lex);
	  } else if(!isdigit(c)){
		break;
	  }
    }
  }

  while(TRUE){
	if((c == 'L') || (c == 'l')){
	  c = LEXER_GET_NEXT_CHAR(lex);
	  continue;
	} else {
	  break;
	}
  }
  
  end = lex->cur - 1;
  len = end - lex->str;
  text = mem(len + NULL_LEN);
  memset(text,INIT_VALUE,len + NULL_LEN);
  memcpy(text,lex->str,len);
  lex->cur = end;

  return create_token(type,text,TOKEN_GET_LINE_NO(lex),LEXER_GET_NAME(lex));
}

static token_t *scan_float(lexer_t *lex){

  char *text;
  char *end;
  int len;
  token_type_t type;
  char c;

  type = TOKEN_FLOAT;
  while(TRUE){
	c = LEXER_GET_NEXT_CHAR(lex);
	if(!isdigit(c)){
	  break;
	}
  }

  end = lex->cur - 1;
  len = end - lex->str;
  text = mem(len + NULL_LEN);
  memset(text,INIT_VALUE,len + NULL_LEN);
  memcpy(text,lex->str,len);
  lex->cur = end;
  
  return create_token(type,text,TOKEN_GET_LINE_NO(lex),LEXER_GET_NAME(lex));
}

static token_t *scan_string(lexer_t *lex){
  
  char *text;
  char *end;
  token_type_t type;
  int len;
  char ch;

  ch = LEXER_GET_NEXT_CHAR(lex);
  while(IS_NOT_DOUBLE_QUOTE(ch)){
    ch = LEXER_GET_NEXT_CHAR(lex);
  }

  ch = LEXER_GET_NEXT_CHAR(lex);
  end = lex->cur - 1;
  len = end - lex->str;
  text = mem(len + NULL_LEN);
  memset(text,INIT_VALUE,len + NULL_LEN);
  memcpy(text,lex->str,len);
  lex->cur = end;

  return create_token(TOKEN_STRING,text,TOKEN_GET_LINE_NO(lex),LEXER_GET_NAME(lex));
}

static token_t *scan_char(lexer_t *lex){

  char *text;
  char *end;
  token_type_t type;
  int len;
  char ch;

  ch = LEXER_GET_NEXT_CHAR(lex);
  if('\\' == ch){
    LEXER_GET_NEXT_CHAR(lex);
  }
  ch = LEXER_GET_NEXT_CHAR(lex);
  
  ch = LEXER_GET_NEXT_CHAR(lex);
  end = lex->cur - 1;
  len = end - lex->str;
  text = mem(len + NULL_LEN);
  memset(text,INIT_VALUE,len + NULL_LEN);
  memcpy(text,lex->str,len);
  lex->cur = end;
  
  return create_token(TOKEN_CHARACTER,text,TOKEN_GET_LINE_NO(lex),LEXER_GET_NAME(lex));
}

static token_t *scan_operator(lexer_t *lex,char c){

  char *text;
  char *end;
  token_type_t type;
  int len;
  char ch;
  char tmp;

  ch = c;
  if(is_symbol(ch)){
    tmp = ch;
    ch = LEXER_GET_NEXT_CHAR(lex);
    if(('!' == tmp) && ('=' == ch)){
      ch = LEXER_GET_NEXT_CHAR(lex);
    }
  } else {
    ch = LEXER_GET_NEXT_CHAR(lex);
    while((!isalpha(ch)) && (!isdigit(ch)) && (!is_skip(ch))
	  && (ch != NEW_LINE) && (ch != lex->eoc) && (!is_symbol(ch))){
      ch = LEXER_GET_NEXT_CHAR(lex);
    }
  }
  
  end = lex->cur - 1;
  len = end - lex->str;
  text = mem(len + NULL_LEN);
  memset(text,INIT_VALUE,len + NULL_LEN);
  memcpy(text,lex->str,len);
  lex->cur = end;
  type = search_keyword(text);
  
  return create_token(type,text,lex->line_no,LEXER_GET_NAME(lex));
}


void lexer_put_token(lexer_t *l,token_t *t){
  
  l->lst = cons(l->lst,t);
  
  return;
}

static char skip(lexer_t *lex){
  
  char ch;
  
  ch = LEXER_GET_NEXT_CHAR(lex);
  while(is_skip(ch)){
    ch = LEXER_GET_NEXT_CHAR(lex);
  }

  return ch;
}

static int is_comment(lexer_t *lex,char ch){

  int flag;
  char c;
  
  flag = FALSE;
  c = ch;
  if(c == '/'){
    c = LEXER_GET_NEXT_CHAR(lex);
    if(c == '/'){
      while(c != NEW_LINE){
	c = LEXER_GET_NEXT_CHAR(lex);
      }
      lex->line_no++; 
      flag = TRUE;
    } else if(c == '*'){
      c = LEXER_GET_NEXT_CHAR(lex);
      while(TRUE){
	if('\n' == c){
	  lex->line_no++;
	}
	if('*' == c){  
	  c = LEXER_GET_NEXT_CHAR(lex);
	  if('/' == c){
	    flag = TRUE;
	    break;
	  }
	} else if(LEXER_GET_EOC(lex)){
	  c = LEXER_GET_EOC(lex);
	  break;
	} else {
	  c = LEXER_GET_NEXT_CHAR(lex);
	}
      }
    } else {
      lexer_put_ch(lex);
    }
  }

  return flag;
}

static int is_skip(char ch){

  if((ch == SPACE) || (ch == TAB) || (ch == VTAB)){
	return TRUE;
  }

  return FALSE;
}

static int is_symbol(char ch){

  return ((ch == '{')
	  || (ch == '}') 
	  || (ch == '[')
	  || (ch == ']')
	  || (ch == '(')
	  || (ch == ')')
	  || (ch == '*')
	  || (ch == '/')
	  || (ch == ':')
	  || (ch == ';')
	  || (ch == ',')
	  || (ch == '?')
	  || (ch == '!'));
}

static int is_hex(char ch){
  return (('0' <= ch) && (ch <= '9'))
    || (('A' <= ch) && (ch <= 'F'))
    || (('a' <= ch) && (ch <= 'f'));
}


static char lexer_get_ch(lexer_t *lex){

  char ch;

  ch = *lex->cur;
  if(*lex->cur != lex->eoc){
    lex->cur++;
  }

  return ch;
}

static void lexer_put_ch(lexer_t *lex){
  
  lex->cur--;
  
  return;
}

static token_type_t search_keyword(char *text){

  for(int i = 0; i < (int)(sizeof(keywords) / sizeof(keywords[0])); i++){
    if(STRCMP(text,TOKEN_GET_STR((&keywords[i])))){
      return TOKEN_GET_TYPE((&keywords[i]));
    }
  }

  return TOKEN_LETTER;
}

char read_ch(lexer_t *l){
  return LEXER_GET_NEXT_CHAR(l);
}
