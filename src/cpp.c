// 
//
//  Copyright 2019 Shota Hisai. Released under the MIT license.
//
//
#include "cpp.h"
#include "lex.h"
#include "mem.h"
#include "common.h"
#include "file.h"
#include "stack.h"
#include "srcinfo.h"
#include "list.h"
#include "map.h"
#include "macro.h"
#include "env.h"
#include "parser.h"
#include "dump.h"
#include "inst.inc"

#define IS_TAB(t)      (TOKEN_GET_TYPE(t) == TOKEN_TAB)
#define IS_VTAB(t)     (TOKEN_GET_TYPE(t) == TOKEN_VTAB)
#define IS_NEWLINE(t)  (TOKEN_GET_TYPE(t) == TOKEN_NEWLINE)
#define IS_NUMBER(t)   (TOKEN_GET_TYPE(t) == TOKEN_NUMBER)
#define IS_FLOAT(t)    (TOKEN_GET_TYPE(t) == TOKEN_FLOAT)
#define IS_COMMA(t)    (TOKEN_GET_TYPE(t) == TOKEN_COMMA)
#define IS_INCLUDE(t)     (TOKEN_GET_TYPE(t) == TOKEN_INCLUDE)
#define IS_DEFINE(t)      (TOKEN_GET_TYPE(t) == TOKEN_DEFINE)
#define IS_IFDEF(t)       (TOKEN_GET_TYPE(t) == TOKEN_IFDEF)
#define IS_IFNDEF(t)      (TOKEN_GET_TYPE(t) == TOKEN_IFNDEF)
#define IS_SHAPE(t)       (TOKEN_GET_TYPE(t) == TOKEN_SHAPE)
#define IS_LETTER(t)      (TOKEN_GET_TYPE(t) == TOKEN_LETTER)
#define IS_LPAREN(t)      (TOKEN_GET_TYPE(t) == TOKEN_LPAREN)
#define IS_RPAREN(t)      (TOKEN_GET_TYPE(t) == TOKEN_RPAREN)
#define IS_ELIF(t)        (TOKEN_GET_TYPE(t) == TOKEN_ELIF)
#define IS_ELSE(t)        (TOKEN_GET_TYPE(t) == TOKEN_ELSE)
#define IS_UNDEF(t)       (TOKEN_GET_TYPE(t) == TOKEN_UNDEF)
#define IS_ENDIF(t)       (TOKEN_GET_TYPE(t) == TOKEN_ENDIF)
#define IS_EOT(t)         (TOKEN_GET_TYPE(t) == TOKEN_EOT)
#define IS_IF(t)          (TOKEN_GET_TYPE(t) == TOKEN_IF)
#define IS_ELSE(t)        (TOKEN_GET_TYPE(t) == TOKEN_ELSE)
#define IS_DEFINED(t)     (TOKEN_GET_TYPE(t) == TOKEN_DEFINED)
#define IS_SPACE(t)       (TOKEN_GET_TYPE(t) == TOKEN_SPACE)
#define IS_ERROR(t)       (TOKEN_GET_TYPE(t) == TOKEN_ERROR)
#define IS_BACKSLASH(t)   (TOKEN_GET_TYPE(t) == TOKEN_BACK_SLASH)

#define IS_MACRO_OBJ(m)  m->type == MACRO_OBJECT

static void read_include(compile_info_t *com,lexer_t *lexer);
static void read_next_include(compile_info_t *com,lexer_t *lexer,source_info_t *src_info);
static void read_define(compile_info_t *com,lexer_t *lexer);
static void read_define_macro_obj(compile_info_t *com,lexer_t *lexer,token_t *name,list_t *obj);
static void read_define_func(compile_info_t *com,lexer_t *lexer,token_t *name);
static void read_ifndef(compile_info_t *com,lexer_t *lexer);
static void read_ifdef(compile_info_t *com,lexer_t *lexer);
static void read_undef(compile_info_t *com,lexer_t *lexer);
static void read_elif(compile_info_t *com,lexer_t *lexer);
static void read_if(compile_info_t *com,lexer_t *lexer);
static void read_error(compile_info_t *com,lexer_t *lexer);
static token_t *read_constants(compile_info_t *com,lexer_t *lexer);
static bool_t read_constant(compile_info_t *com,lexer_t *lexer);
static list_t *read_constant_text(compile_info_t *com,lexer_t *lexer);
static list_t *read_constant_newline(compile_info_t *com,lexer_t *lexer);
static list_t *read_macro_list(compile_info_t *com,lexer_t *lexer);
static token_t *read_defined_op(compile_info_t *com,lexer_t *lexer);
static void cprereaddefs(compile_info_t *com,lexer_t *lexer,bool_t flag,bool_t predic);
static void cprereaddef(compile_info_t *com,lexer_t *lexer,bool_t flag,bool_t predic);
static string_t read_header_file(compile_info_t *com,lexer_t *lexer,bool_t *std);
static string_t read_header_path(compile_info_t *com,string_t path,string_t file_name);

static list_t *read_like_func_parm(compile_info_t *com,lexer_t *lexer);
static list_t *read_like_func_body(compile_info_t *com,lexer_t *lexer);
static void expand_macro(compile_info_t *com,lexer_t *lexer,macro_t *macro);
static void expand_like_func_macro(lexer_t *lexer,macro_t *macro);
static list_t *get_body(lexer_t *lexer);
static list_t *get_nest_body(lexer_t *lexer,list_t *token_lst);
static void bind(env_t *env,list_t *args,list_t *vals);
static void apply_macro_expansion(list_t *token_seqs,env_t *env,list_t *body);
static void read_cond_directive(compile_info_t *com,lexer_t *lexer,int flag);
static void read_cond_directives(compile_info_t *com,lexer_t *lexer,token_t *t);
static void read_if_directive(compile_info_t *com,lexer_t *lexer,token_t *t,int not_flag);
static list_t *copy_macro_lst(list_t *lst);

static void dump_lst(list_t *lst);
static void dump_macro(macro_t *macro);
static void dump_macro_obj(macro_t *macro);
static void dump_macro_func_like_obj(macro_t *macro);
static void dump_map(map_t *map);
static void dump_map_obj(list_t *lst);
static void dump_token_sequence(list_t  *lst);

static list_t *create_token_seaquence(lexer_t *lexer);
static bool_t is_terminate_symbol(token_t *t);
static void skip_until_newline(lexer_t *lexer);
static list_t *replace_macro(compile_info_t *com,list_t *lst);

//
//
//   eval 
//
//
static list_t *eval(list_t *expr,map_t *env);
static list_t *eval_operand(list_t *expr,map_t *env);
static list_t *eval_symbol(list_t *expr,map_t *env);
static list_t *eval_number(list_t *expr,map_t *env);
static list_t *eval_defined(list_t *expr,map_t *env);
static list_t *eval_logical_op(list_t *expr,map_t *env);
static list_t *eval_unary_op(list_t *expr,map_t *env);
static list_t *eval_macro_obj(map_t *env,string_t symbol);
static list_t *eval_macro(macro_t *macro);
static list_t *eval_op(list_t *expr,map_t *env);
static list_t *eval_bin_op(list_t *expr,map_t *env);
static list_t *eval_func_call(list_t *expr,map_t *env);
static list_t *eval_ternary(list_t *expr,map_t *env);


void cpreprocess(compile_info_t *com,lexer_t *lexer,source_info_t *srcinfo){

  token_t *t;
  
#ifdef __DEBUG__
  printf("cpreprocess\n");
#endif

  push(COM_GET_STACK(com),srcinfo);
  add_src_info_lst(com,srcinfo);
  cprereaddefs(com,lexer,FALSE,TRUE);

  t = scan(lexer);
  if(!IS_EOT(t)){
	printf("Error\n");
	exit(1);
  }

  lexer->lst = concat(lexer->lst,cons(make_null(),t));
  save_lexinfo(srcinfo,lexer);
  com->token_lst = concat(com->token_lst,SOURCE_INFO_GET_LST(srcinfo));

  pop(COM_GET_STACK(com));

  ASSERT(empty(COM_GET_STACK(com)));

#ifdef __CPP__DEBUG__
  printf("================================\n");
  dump_map(COM_GET_MACROS(com));
  printf("================================\n");
#endif

  return;
}

void dump_token_sequences(compile_info_t *com){

  list_t *p;
  source_info_t *srcinfo;
  
#ifdef __DEBUG__
  printf("dump_token_sequences\n");
#endif

  dump_token_sequence(com->token_lst);
  
  return;
}

void dump_token_lst(list_t *lst){

#ifdef __DEBUG__
  printf("dump_token_lst\n");
#endif

  dump_token_sequence(lst);

  return;
}

static void dump_map(map_t *map){
  
  int i;
#ifdef __DEBUG__
  printf("dump_map\n");
#endif
  
  for(i = 0; i < (int)(sizeof(map->objs) / sizeof(map->objs[0])); i++){
    dump_map_obj(map->objs[i]);
  }
  
  return;
}

static void dump_map_obj(list_t *lst){
  
  token_t *t;
  list_t *p;
  
#ifdef __DEBUG__
  printf("dump_map_obj\n");
#endif
  
  p = lst;
  while(TRUE){
    switch(LIST_GET_TYPE(p)){
    case LIST:
      printf("( ");
      dump_map_obj(car(p));
      printf(")\n");
      break;
    case NULL_LIST:
      return;
      break;
    case OBJECT:
      dump_macro(car(p));
      break;
    case SYMBOL:
      printf(" %s ",(string_t)car(p));
      break;
    default:
      break;
    }
    p = cdr(p);
  }
  
  return;
}

static void cprereaddefs(compile_info_t *com,lexer_t *lexer,bool_t flag,bool_t predic){

  token_t *t;
  stack_ty *stack;
  source_info_t *src_info;
  
#ifdef __DEBUG__
  printf("cprereaddefs\n");
#endif

  while(TRUE){
	t = scan(lexer);
    if(IS_SHAPE(t)){
      cprereaddef(com,lexer,flag,predic);
	} else {
      if(is_terminate_symbol(t) && flag){
		put_token(lexer,t);
		return;
      } else if(IS_EOT(t)){
		return;
	  }

	  if(!predic){
		continue;
	  }

      if(!IS_LETTER(t)){
		lexer->lst = concat(lexer->lst,cons(make_null(),t));
      } else {
		macro_t *macro = map_get(COM_GET_MACROS(com),TOKEN_GET_STR(t));
		if(macro){
		  expand_macro(com,lexer,macro);
		} else {
		  lexer->lst = concat(lexer->lst,cons(make_null(),t));
		}
      }
    }
  }
  
  return;
}

static void cprereaddef(compile_info_t *com,lexer_t *lexer,bool_t flag,bool_t predic){
  
  token_t *t;

#ifdef __DEBUG__
  printf("cprereaddef\n");
#endif
  t = scan(lexer);
  if(is_terminate_symbol(t)){
    put_token(lexer,t);
    return;
  }
  
  put_token(lexer,t);
  if(!predic){
    skip_until_newline(lexer);
    return;
  }
  
  t = scan(lexer);
  if(IS_INCLUDE(t)){
    read_include(com,lexer);
  } else if(IS_DEFINE(t)){
    read_define(com,lexer);
  } else if(IS_IFDEF(t)){
    read_ifdef(com,lexer);
  } else if(IS_IFNDEF(t)){
    read_ifndef(com,lexer);
  } else if(IS_IF(t)){
    read_if(com,lexer);
  } else if(IS_UNDEF(t)){
    read_undef(com,lexer);
  } else if(IS_ERROR(t)){
    read_error(com,lexer);
  } else {
    if(flag){
      put_token(lexer,t);
    }
  }
  
  return;
}

static void read_include(compile_info_t *com,lexer_t *lexer){

  string_t file_name;
  string_t path;
  string_t name;
  bool_t std;
  file_t *file;
  stack_ty *stack;
  list_t *p;
  bool_t flag;
  source_info_t *src_info;
  
#ifdef __DEBUG__
  printf("read_include\n");
#endif
  
  COM_GET_STD_PATH_LST(com);
  stack = COM_GET_STACK(com);
  std = FALSE;
  file_name = read_header_file(com,lexer,&std);
  if(std){
    flag = FALSE;
    for(p = COM_GET_STD_PATH_LST(com); IS_NOT_NULL_LIST(p); p = cdr(p)){
      path = (string_t)car(p);
      name = read_header_path(com,path,file_name);
	  file = file_create();
      FILE_SET_NAME(file,name);
      FILE_SET_MODE(file,"r");
      if(file_open(file)){
		flag = TRUE;
		break;
      }
      fre(file);
      fre(name);
    }

    if(!flag){
      error_no_info("Not found file [%s]\n",file_name);
      exit(1);
    }
  } else {
    file = file_create();
    FILE_SET_NAME(file,file_name);
    FILE_SET_MODE(file,"r");
    if(!file_open(file)){
      error_no_info("Not found file [%s]\n",file_name);
    }
  }
  src_info = create_source_info(file,NULL);
  read_next_include(com,lexer,src_info);

  return;
}


static void read_next_include(compile_info_t *com,lexer_t *lexer,source_info_t *srcinfo){

  source_info_t *cur_srcinfo;
  stack_ty *stack;
  file_t *file;
  string_t src;

#ifdef __DEBUG__
  printf("read_next_include\n");
#endif

  add_src_info_lst(com,srcinfo);
  stack = COM_GET_STACK(com);
  
  cur_srcinfo = top(stack);
  save_lexinfo(cur_srcinfo,lexer);
  com->token_lst = concat(com->token_lst,SOURCE_INFO_GET_LST(cur_srcinfo));


  src_info_init_lst(cur_srcinfo);

  push(stack,srcinfo);

  load_lexinfo(srcinfo,lexer);
  file = SOURCE_INFO_GET_FILE(srcinfo);
  src = file_read_as_string(SOURCE_INFO_GET_FILE(srcinfo));
  lexer_set_src(lexer,src);
  cprereaddefs(com,lexer,FALSE,TRUE);
  save_lexinfo(srcinfo,lexer);
  com->token_lst = concat(com->token_lst,SOURCE_INFO_GET_LST(srcinfo));

  pop(stack);
  load_lexinfo(cur_srcinfo,lexer);

  return;
}

static void read_define(compile_info_t *com,lexer_t *lexer){

#ifdef __DEBUG__
  printf("read_define\n");
#endif
  token_t *name;
  token_t *t;
  name = scan(lexer);

  if(!IS_LETTER(name)){
    error(TOKEN_GET_LINE_NO(name),TOKEN_GET_NAME(name),"Definition error [%s]",TOKEN_GET_STR(name));
  }

  t = scan_by_no_skip(lexer);
  if(IS_SPACE(t) || IS_TAB(t) || IS_VTAB(t)){
	read_define_macro_obj(com,lexer,name,make_null());
  } else {
	if(IS_LPAREN(t)){
	  token_t *lparen;
	  lparen = t;
	  t = scan(lexer);
	  if(IS_LETTER(t)){
		put_token(lexer,t);
		read_define_func(com,lexer,name);
	  } else {
		list_t *lst = make_null();
		lst = concat(lst,cons(make_null(),lparen));
		lst = concat(lst,cons(make_null(),t));
		read_define_macro_obj(com,lexer,name,lst);
	  }
	} else {
	  put_token(lexer,t);
	  read_define_macro_obj(com,lexer,name,make_null());
	}
  }
  
  return;
}

static void read_define_macro_obj(compile_info_t *com,lexer_t *lexer,token_t *name,list_t *obj){

#ifdef __DEBUG__
  printf("read_define_macro_obj\n");
#endif
  
  token_t *t;
  token_t *pre_token;
  list_t *lst;
  list_t *val;
  list_t *src_toks;
  macro_t *macro;
  parser_t *parser;

  pre_token = NULL;
  lst = obj;
  while(TRUE){
    t = scan(lexer);
    if(IS_NEWLINE(t) && pre_token){
      pre_token = NULL;
      continue;
    } else if(IS_NEWLINE(t) && !pre_token){
      break;
    } else if(IS_BACKSLASH(t)){
      pre_token = t;
      continue;
    } else {
      ;
    }

    if(!pre_token){
      lst = concat(lst,cons(make_null(),t));
    } else if(pre_token){
      lst = concat(lst,cons(make_null(),pre_token));
      lst = concat(lst,cons(make_null(),t));
    }
  }

  macro = macro_create();
  MACRO_SET_TYPE(macro,MACRO_OBJECT);

  val = replace_macro(com,lst);
  MACRO_SET_OBJ(macro,val);

  map_put(COM_GET_MACROS(com),TOKEN_GET_STR(name),macro);

  return;
}

static void read_define_func(compile_info_t *com,lexer_t *lexer,token_t *name){

  macro_t *macro;
  list_t *body;
  list_t *parm;

#ifdef __DEBUG__
  printf("read_define_func\n");
#endif

  macro = macro_create();
  parm = read_like_func_parm(com,lexer);
  body = read_like_func_body(com,lexer);
  
  MACRO_SET_TYPE(macro,MACRO_LINE_FUNCTION);
  MACRO_SET_PARAM(macro,parm);
  MACRO_SET_BODY(macro,body);

  map_put(COM_GET_MACROS(com),TOKEN_GET_STR(name),macro);
  
  return;
}

static list_t *read_like_func_parm(compile_info_t *com,lexer_t *lexer){
  
  list_t *parm_lst;
  token_t *t;
  int pos;
#ifdef __DEBUG__
  printf("read_like_func_parm\n");
#endif
  
  pos = FALSE;
  parm_lst = make_null();
  while(TRUE){
    t = scan(lexer);
    if(IS_RPAREN(t)){
      break;
    }
    
    if(pos){
      pos = FALSE;
    } else {
      parm_lst = concat(parm_lst,cons(make_null(),t));
      pos = TRUE;
    }
  }

  return parm_lst;
}

static list_t *read_like_func_body(compile_info_t *com,lexer_t *lexer){

  list_t *body_lst;
  token_t *t;
  token_t *pre_token;
  
#ifdef __DEBUG__
  printf("read_like_func_body\n");
#endif

  pre_token = NULL;
  body_lst = make_null();
  while(TRUE){
    t = scan(lexer);
    if(IS_NEWLINE(t) && pre_token){
      pre_token = NULL;
      continue;
    } else if(IS_NEWLINE(t) && !pre_token){
      break;
    } else if(IS_BACKSLASH(t)){
      pre_token = t;
      continue;
    } else {
      ;
    }

    if(!pre_token){
      body_lst = concat(body_lst,cons(make_null(),t));
    } else if(pre_token){
      body_lst = concat(body_lst,cons(make_null(),pre_token));
      body_lst = concat(body_lst,cons(make_null(),t));
    }
  }

  return body_lst;
}

static void expand_macro(compile_info_t *com,lexer_t *lexer,macro_t *macro){

#ifdef __DEBUG__
  printf("expand_macro\n");
#endif
  
  if(IS_MACRO_OBJ(macro)){
    lexer->lst = concat(lexer->lst,copy_macro_lst(MACRO_GET_OBJ(macro)));
  } else {
    expand_like_func_macro(lexer,macro);
  }
  
  return;
}

static void expand_like_func_macro(lexer_t *lexer,macro_t *macro){
  
  list_t *param;
  list_t *body;
  list_t *vals;
  env_t *env;
  token_t *t;

#ifdef __DEBUG
  printf("expand_like_func_macro\n");
#endif
  env = make_env();
  param = MACRO_GET_PARAM(macro);
  body = MACRO_GET_BODY(macro);
  vals = get_body(lexer);
  bind(env,param,vals);
  apply_macro_expansion(lexer->lst,env,body);
  
  return;
}

static void read_ifndef(compile_info_t *com,lexer_t *lexer){

  token_t *t;
  token_t *letter;
  int flag;
  
#ifdef __DEBUG__
  printf("read_ifndef\n");
#endif
  
  flag = FALSE;
  t = scan(lexer);

  letter = t;
  t = scan(lexer);

  read_if_directive(com,lexer,letter,TRUE);
  
  return;
}

static void read_ifdef(compile_info_t *com,lexer_t *lexer){

  token_t *t;
  token_t *letter;
  int flag;
  
#ifdef __DEBUG__
  printf("read_ifdef\n");
#endif
  
  t = scan(lexer);
  
  letter = t;
  t = scan(lexer);
  
  read_if_directive(com,lexer,letter,FALSE);
  
  return;
}

static void read_undef(compile_info_t *com,lexer_t *lexer){

  token_t *t;

  #ifdef __DEBUG__
  printf("read_undef\n");
  #endif

  t = scan(lexer);

  string_t tk = TOKEN_GET_STR(t);
  map_remove(COM_GET_MACROS(com),TOKEN_GET_STR(t));
  
  t = scan(lexer);
  
  return;
}

static void read_elif(compile_info_t *com,lexer_t *lexer){

#ifdef __DEBUG__
  printf("read_elif\n");
#endif
  
  read_cond_directive(com,lexer,read_constant(com,lexer));
  
  return;
}

static void read_if(compile_info_t *com,lexer_t *lexer){

  token_t *t;
  bool_t flag;
  
#ifdef __DEBUG__
  printf("read_if\n");
#endif
  
  flag = read_constant(com,lexer);
  read_cond_directive(com,lexer,flag);
  
  return;
}

static void read_error(compile_info_t *com,lexer_t *lexer){

  token_t *t;

#ifdef __DEBUG__
  printf("read_error\n");
#endif
  
  while(TRUE){
    t = scan(lexer);
    if(IS_NEWLINE(t)){
      break;
    }
  }
  
  return;
}

static bool_t read_constant(compile_info_t *com,lexer_t *lexer){
  
  list_t *expr;
  list_t *val;
  bool_t ret;
  
#ifdef __DEBUG__
  printf("read_constant\n");
#endif

  expr = read_constant_newline(com,lexer);
  val = eval(expr,COM_GET_MACROS(com));
  if(*(int *)car(val)){
    ret = TRUE;
  } else {
    ret = FALSE;
  }
  
  return ret;
}

static list_t *read_constant_newline(compile_info_t *com,lexer_t *lexer){
  
  token_t *ct;
  list_t *token_seaquence;
  list_t *src_toks;
  list_t *expr;
  parser_t *parser;

#ifdef __DEBUG__
  printf("read_constant_newline\n");
#endif
  
  token_seaquence = create_token_seaquence(lexer);
  src_toks = LEXER_GET_LST(lexer);
  parser = parser_create();
  
  PARSER_SET_LEX(parser,lexer);
  LEXER_SET_LST(lexer,token_seaquence);
  
  expr = parser_constant_texts(parser);

  LEXER_SET_LST(lexer,src_toks);
  
  return expr;
}

static list_t *read_macro_list(compile_info_t *com,lexer_t *lexer){

  list_t *lst;
  token_t *t;
  token_t *pre_token;

#ifdef __DBEUG__
  printf("read_macro_list\n");
#endif
  
  pre_token = NULL;
  lst = make_null();
  while(TRUE){
    t = scan(lexer);
    if(IS_NEWLINE(t) && pre_token){
      pre_token = NULL;
      continue;
    } else if(IS_NEWLINE(t) && !pre_token){
      break;
    } else if(IS_BACKSLASH(t)){
      pre_token = t;
      continue;
    } else {
      ;
    }

    if(!pre_token){
      lst = concat(lst,cons(make_null(),t));
    } else if(pre_token){
      lst = concat(lst,cons(make_null(),pre_token));
      lst = concat(lst,cons(make_null(),t));
    }
  }
  
  return lst;
}

static token_t *read_defined_op(compile_info_t *com,lexer_t *lexer){

  token_t *t;
  
#ifdef __DEBUG__
  printf("read_defined_op\n");
#endif

  t = scan(lexer);
  if(IS_LPAREN(t)){
    t = scan(lexer);
    if(!IS_RPAREN(scan(lexer))){
      error(TOKEN_GET_LINE_NO(t),TOKEN_GET_NAME(t),"Expected : [%s] but got [%s]\n",";",TOKEN_GET_STR(t));
    }
  } else if(!IS_LETTER(t)){
    error(TOKEN_GET_LINE_NO(t),TOKEN_GET_NAME(t),"Expected : [%s] but got [%s]\n",";",TOKEN_GET_STR(t));
  }
  
  return t;
}

static string_t read_header_file(compile_info_t *com,lexer_t *lexer,bool_t *std){
  
  string_t file_name;
  char close;
  token_t *t;
  char *buf;
  int cnt;
  int i;
  int size;
  char c;

#ifdef __DEBUG__
  printf("read_header_file\n");
#endif
  
  c = read_ch(lexer);
  while(c == ' ' || c == '\t' || c == '\v'){
    c = read_ch(lexer);
  }
  
  if(c == '<'){
    close = '>';
    *std = TRUE;
  } else if(c == '"'){
    close = '"';
    *std = FALSE;
  }

  c = read_ch(lexer);
  
  i = 0;
  cnt = 1;
  buf = mem(BUF_SIZE);
  size = BUF_SIZE;
  while(close != c){
    if(c == '\n' || c  == EOF){
      exit(1);
    }
  
    if(i >= size){
      size = size * (cnt++);
      buf = remem(buf,size);
    }
    buf[i++] = c;
    c = read_ch(lexer);
  }
  buf[i] = NULL_CHAR;
  
  return buf;
}

static string_t read_header_path(compile_info_t *com,string_t path,string_t file_name){
  
  
  string_t new_file_name;
  string_t p;
  int len_of_path;
  int len_of_file_name;
  
#ifdef __DEBUG__
  printf("read_header_path\n");
#endif
  
  len_of_path = strlen(path);
  len_of_file_name = strlen(file_name);
  
  new_file_name = mem(len_of_path + len_of_file_name + NULL_LEN);
  p = new_file_name;
  memcpy(p,path,len_of_path);
  p += len_of_path;
  memcpy(p,file_name,len_of_file_name);
  
  return new_file_name;
}

static list_t *get_body(lexer_t *lexer){

  list_t *new_lst;
  list_t *token_lst;
  token_t *t;
  
#ifdef __DEBUG__
  printf("get_body\n");
#endif
  
  t = scan(lexer);
  
  new_lst = make_null();
  token_lst = make_null();
  while(TRUE){
    t = scan(lexer);
    if(IS_RPAREN(t)){
      new_lst = concat(new_lst,add_list(make_null(),token_lst));
      token_lst = make_null();
      break;
    } else if(IS_COMMA(t)){
      new_lst = concat(new_lst,add_list(make_null(),token_lst));
      token_lst = make_null();
      continue;
    } else if(IS_LPAREN(t)){
	  token_lst = concat(token_lst,cons(make_null(),t));
	  get_nest_body(lexer,token_lst);
	}
    
    token_lst = concat(token_lst,cons(make_null(),t));
  }

  return new_lst;
}

static list_t *get_nest_body(lexer_t *lexer,list_t *token_lst){

  list_t *new_lst;
  token_t *t;

#ifdef __DEBUG__
  printf("get_nest_body\n");
#endif

  new_lst = make_null();
  while(TRUE){
	t = scan(lexer);
	if(IS_LPAREN(t)){
	  return get_nest_body(lexer,token_lst);
	} else if(IS_RPAREN(t)){
	  token_lst = concat(token_lst,cons(make_null(),t));
	  break;
	} else if(IS_EOT(t)){
	  break;
	}
	token_lst = concat(token_lst,cons(make_null(),t));
  }

  return new_lst;
}

static void bind(env_t *env,list_t *args,list_t *vals){

  list_t *p;
  list_t *q;
  
#ifdef __DEBUG__
  printf("bind\n");
#endif
  
  p = args;
  q = vals;
  while(TRUE){
    if(IS_NULL_LIST(p) && IS_NULL_LIST(q)){
      break;
    }

    token_t *t = (token_t *)car(p);
    insert_obj(env,TOKEN_GET_STR(t),car(q));
    p = cdr(p);
    q = cdr(q);
  }

  return;
}

static void apply_macro_expansion(list_t *token_seqs,env_t *env,list_t *body){

  list_t *p;

#ifdef __DEBUG__
  printf("apply_macro_expansion\n");
#endif

  p = body;
  while(TRUE){
    if(IS_NULL_LIST(p)){
      break;
    }
    
    token_t *t = car(p);
    list_t *tks = lookup_obj(env,TOKEN_GET_STR(t));
    if(tks){
      token_seqs = concat(token_seqs,copy_macro_lst(tks));
    } else {
      token_seqs = concat(token_seqs,cons(make_null(),t));
    }
    p = cdr(p);
  }

  return;
}

static void read_if_directive(compile_info_t *com,lexer_t *lexer,token_t *t,int not_flag){
  
  int flag;
  #ifdef __DEBUG__
  printf("read_if_directive\n");
  #endif
  
  flag = FALSE;
  macro_t *macro = map_get(COM_GET_MACROS(com),TOKEN_GET_STR(t));
  if(macro){
    flag = TRUE;
  }

  if(not_flag){
    flag = !flag;
  }
  
  read_cond_directive(com,lexer,flag);
  
  return;
}

static void read_cond_directive(compile_info_t *com,lexer_t *lexer,int flag){

  token_t *t;
  
#ifdef __DEBUG__
  printf("read_cond_directive\n");
#endif

  while(TRUE){
    t = scan(lexer);
    if(IS_SHAPE(t)){
      put_token(lexer,t);
      cprereaddefs(com,lexer,TRUE,flag);
      break;
    } else if(IS_EOT(t)){
      return;
    }

    if(flag){
	  read_cond_directives(com,lexer,t);
	}
  }
  
  t = scan(lexer);
  if(IS_ENDIF(t)){
    return;
  } else if(IS_ELIF(t)){
    read_elif(com,lexer);
  } else if(IS_ENDIF(t) && !flag){
    read_cond_directive(com,lexer,flag);
  } else if(IS_ELSE(t)){
    read_cond_directive(com,lexer,!flag);
  }
  
  return;
}

static void read_cond_directives(compile_info_t *com,lexer_t *lexer,token_t *t){

#ifdef __DEBUG__
  printf("read_cond_directives\n");
#endif

  if(!IS_LETTER(t)){
	lexer->lst = concat(lexer->lst,cons(make_null(),t));
  } else {
	macro_t *macro = map_get(COM_GET_MACROS(com),TOKEN_GET_STR(t));
	if(macro){
	  expand_macro(com,lexer,macro);
	} else {
	  lexer->lst = concat(lexer->lst,cons(make_null(),t));
	}
  }

  return;
}

static list_t *copy_macro_lst(list_t *lst){

#ifdef __DEBUG__
  printf("copy_macro_lst\n");
#endif

  if(IS_NULL_LIST(lst)){
    return lst;
  } else {
    return concat(cons(make_null(),car(lst)),copy_macro_lst(cdr(lst)));
  }
}

static void dump_lst(list_t *lst){
  
  for(list_t *p = lst; IS_NOT_NULL_LIST(p); p = cdr(p)){
	
    if(IS_LIST(p)){
      printf("[ ");
      dump_lst(car(p));
      printf(" ]\n");
      continue;
    } else if(IS_OBJECT(p)){
      token_t *t = car(p);
      printf(" %s ",TOKEN_GET_STR(t));
    } else if(IS_INTEGER(p)){
      printf(" %d ",*(int *)car(p));
    }
  }
  
  return;
}

static void dump_macro(macro_t *macro){

#ifdef __DEBUG__
  printf("dump_macro\n");
#endif

  switch(MACRO_GET_TYPE(macro)){
  case MACRO_OBJECT:
    dump_macro_obj(macro);
    break;
  case MACRO_LINE_FUNCTION:
    dump_macro_func_like_obj(macro);
    break;
  default:
    break;
  }
  
  return;
}

static void dump_macro_obj(macro_t *macro){

#ifdef __DEBUG__
  printf("dump_macro_obj\n");
#endif

  dump_lst(MACRO_GET_BODY(macro));
  
  return;
}

static void dump_macro_func_like_obj(macro_t *macro){

#ifdef __DEBUG__
  printf("dump_macro_func_like_obj\n");
#endif

  printf("( ");
  dump_lst(MACRO_GET_PARAM(macro));
  printf(") ");
  printf("{ ");
  dump_lst(MACRO_GET_BODY(macro));
  printf(" }");

  return;
}

static void dump_token_sequence(list_t  *lst){

  list_t *l;
  token_t *t;
  
  for(l = lst; IS_NOT_NULL_LIST(l); l = cdr(l)){
    t = (token_t *)car(l);
	if(TOKEN_EOT == TOKEN_GET_TYPE(t)){
	  continue;
	}
	printf(" %s ",TOKEN_GET_STR(t));
#ifdef __CPP__DEBUG__
	printf("\n[%s]\n",TOKEN_GET_NAME(t));
#endif
  }
}


static list_t *create_token_seaquence(lexer_t *lexer){

  list_t *token_seaquence;
  token_t *t;
  token_t *pre_token;

#ifdef __DEBUG__
  printf("create_token_seaquence\n");
#endif
  
  token_seaquence = make_null();
  pre_token = NULL;
  while(TRUE){
    t = scan(lexer);
    if(IS_NEWLINE(t) && pre_token){
      pre_token = NULL;
      continue;
    } else if(IS_NEWLINE(t) && !pre_token){
      token_seaquence = concat(token_seaquence,cons(make_null(),create_token(TOKEN_EOT,NULL,0,LEXER_GET_NAME(lexer))));
      break;
    } else if(IS_BACKSLASH(t)){
      pre_token = t;
      continue;
    } else {
      ;
    }

    if(!pre_token){
      token_seaquence = concat(token_seaquence,cons(make_null(),t));
    } else if(pre_token){
      token_seaquence = concat(token_seaquence,cons(make_null(),pre_token));
      token_seaquence = concat(token_seaquence,cons(make_null(),t));
    }
  }
  
  return token_seaquence;
}

static bool_t is_terminate_symbol(token_t *t){
  return IS_ELSE(t) || IS_ENDIF(t) || IS_ELIF(t);
}

static void skip_until_newline(lexer_t *lexer){

  token_t *t;
  token_t *pre_token;
  bool_t flag;
  bool_t cpp_flag;
  
#ifdef __DEBUG__
  printf("skip_until_newline\n");
#endif
  
  flag = FALSE;
  cpp_flag = FALSE;
  pre_token = NULL;
  while(TRUE){
    t = scan(lexer);
    if(IS_IF(t) || IS_IFDEF(t) || IS_IFNDEF(t)){
      if(flag){
		put_token(lexer,t);
		skip_until_newline(lexer);
      } else {
		flag = TRUE;
      }
    }
    
	if(IS_NEWLINE(t) && pre_token){
      pre_token = NULL;
      continue;
    } else if(IS_NEWLINE(t) && !pre_token && !flag){
      break;
    } else if(IS_BACKSLASH(t)){
      pre_token = t;
      continue;
    } else if(IS_SHAPE(t)){
      cpp_flag = TRUE;
    } else if(IS_ENDIF(t)){
      flag = FALSE;
    } else if(IS_EOT(t)){
      break;
    } else {
      ;
    }
  }
  
  return;
}

static list_t *replace_macro(compile_info_t *com,list_t *lst){

  list_t *p;
  list_t *q;
  macro_t *macro;
  
#ifdef __DEBUG__
  printf("replace_macro\n");
#endif
  
  p = lst;
  q = make_null();
  while(TRUE){
    
    if(IS_NULL_LIST(p)){
      break;
    }
    token_t *t = (token_t*)car(p);
    macro = map_get(COM_GET_MACROS(com),TOKEN_GET_STR(t));
    if(macro){
      if(MACRO_OBJECT == MACRO_GET_TYPE(macro)){
		q = concat(q,copy_macro_lst(MACRO_GET_BODY(macro)));
      }
    } else {
      q = concat(q,cons(make_null(),car(p)));
    }
    p = cdr(p);
  }
  return q;
}

//
//
//  eval
//
//
static list_t *eval(list_t *expr,map_t *env){

  list_t *ret;
  
#ifdef __DEBUG__
  printf("eval\n");
#endif

  switch(LIST_GET_TYPE(expr)){
  case LIST:
    ret = eval((list_t *)car(expr),env);
    if(IS_NOT_NULL_LIST(cdr(expr))){
      ret = eval(cdr(expr),env);
    }
    break;
  case SYMBOL:
    ret = eval_symbol(expr,env);
    break;
  case INTEGER:
    ret = eval_number(expr,env);
    break;
  case NULL_LIST:
	break;
  }
 
  return ret;
}

static list_t *eval_operand(list_t *expr,map_t *env){

  list_t *ret;

#ifdef __DEBUG__
  printf("eval_operand\n");
#endif

  switch(LIST_GET_TYPE(expr)){
  case LIST:
    ret = eval_operand((list_t *)car(expr),env);
    break;
  case SYMBOL:
    ret = eval_symbol(expr,env);
    break;
  case INTEGER:
    ret = eval_number(expr,env);
    break;
  }
  
  return ret;
}

static list_t *eval_symbol(list_t *expr,map_t *env){
  
  list_t *ret;
  string_t symbol;
  
#ifdef __DEBUG__
  printf("eval_symbol\n");
#endif
  
  symbol = (string_t)car(expr);
  if(STRCMP(symbol,DEFINED)){
    ret = eval_defined(expr,env);
  } else if((STRCMP(symbol,LOGICAL_AND))
	    || (STRCMP(symbol,LOGICAL_OR))){
    ret = eval_logical_op(expr,env);
  } else if(STRCMP(symbol,NOT)){
    ret = eval_unary_op(expr,env);
  } else if(STRCMP(symbol,LESS)
	    || STRCMP(symbol,GREATER)
	    || STRCMP(symbol,LESS_EQAUL)
	    || STRCMP(symbol,GREATER_EQUAL)
	    || STRCMP(symbol,EQUAL)
	    || STRCMP(symbol,NOT_EQUAL)){
    ret = eval_op(expr,env);
  } else if(STRCMP(symbol,ADD)
	    || STRCMP(symbol,SUB)
	    || STRCMP(symbol,MUL)
	    || STRCMP(symbol,DIV)){
    ret = eval_bin_op(expr,env);
  } else if(STRCMP(symbol,FUNC_CALL)){
    ret = eval_func_call(expr,env);
  } else if(STRCMP(symbol,TERNARY)){
	ret = eval_ternary(expr,env);
  } else {
    ret = eval_macro_obj(env,symbol);
  }

  return ret;
}

static list_t *eval_number(list_t *expr,map_t *env){
  
#ifdef __DEBUG__
  printf("eval_number\n");
#endif
  
  return add_number(make_null(),*(int *)car(expr));
}

static list_t *eval_defined(list_t *expr,map_t *env){
  
#ifdef __DEBUG__
  printf("eval_defined\n");
#endif
  
  return eval_operand(cdr(expr),env);
}

static list_t *eval_logical_op(list_t *expr,map_t *env){
  
  list_t *ret;
  list_t *l;
  list_t *r;
  string_t op;
  int flag;

#ifdef __DEBUG__
  printf("eval_logical_op\n");
#endif

  op = (string_t)car(expr);
  l = eval_operand(cdr(expr),env);
  r = eval_operand(cdr(cdr(expr)),env);
  
  if(STRCMP(LOGICAL_AND,op)){
    flag = (*(long *)car(l)) && (*(long *)car(r));
  } else if(STRCMP(LOGICAL_OR,op)){
    flag = (*(long *)car(l)) || (*(long *)car(r));
  }
  
  return add_number(make_null(),flag);
}

static list_t *eval_unary_op(list_t *expr,map_t *env){

  string_t op;
  list_t *l;
  int flag;
  
#ifdef __DEBUG__
  printf("eval_unary_op\n");
#endif

  op = (string_t)car(expr);
  l = eval_operand(cdr(expr),env);
  
  if(STRCMP(NOT,op)){
    flag = (*(int *)car(l));
    flag = !flag;
  }
  
  return add_number(make_null(),flag);
}

static list_t *eval_macro_obj(map_t *env,string_t symbol){

  macro_t *macro;
  list_t *ret;

#ifdef __DEBUG__
  printf("eval_macro_obj\n");
#endif

  ret = make_null();
  macro = map_get(env,symbol);
  if(macro){
    if(MACRO_GET_TYPE(macro) == MACRO_OBJECT){
      ret = eval_macro(macro);
    }
  } else {
    ret = add_number(ret,FALSE);
  }
  
  return ret;
}

static list_t *eval_macro(macro_t *macro){
  
  list_t *lst;
  list_t *val;
  token_t *t;
  
#ifdef __DEBUG__
  printf("eval_macro\n");
#endif

  val = make_null();
  lst = MACRO_GET_BODY(macro);
  if(IS_NULL_LIST(lst)){
    return add_number(val,TRUE);
  }

  t = car(lst);
  if(TOKEN_NUMBER == TOKEN_GET_TYPE(t)){
    val = add_number(val,atoi(TOKEN_GET_STR(t)));
  }

  return val;
}

static list_t *eval_op(list_t *expr,map_t *env){

  list_t *val;
  list_t *r;
  list_t *l;
  string_t op;
  int ret;
  
#ifdef __DEBUG__
  printf("eval_op\n");
#endif

  op = (string_t)car(expr);
  l = eval_operand(cdr(expr),env);
  r = eval_operand(cdr(cdr(expr)),env);
  
  val = make_null();
  if(STRCMP(op,LESS)){
    ret = *(long*)car(l) < *(long*)car(r);
  } else if(STRCMP(op,GREATER)){
    ret = *(long*)car(l) > *(long*)car(r);
  } else if(STRCMP(op,LESS_EQAUL)){
    ret = *(long*)car(l) <= *(long*)car(r);
  } else if(STRCMP(op,GREATER_EQUAL)){
    ret = *(long*)car(l) >= *(long*)car(r);
  } else if(STRCMP(op,EQUAL)){
    ret = *(long*)car(l) == *(long*)car(r);
  } else if(STRCMP(op,NOT_EQUAL)){
    ret = *(long*)car(l) != *(long*)car(r);
  } else {
    exit(1);
  }
  
  val = add_number(make_null(),ret);

  return val;
}

static list_t *eval_bin_op(list_t *expr,map_t *env){

  list_t *l;
  list_t *r;
  list_t *val;
  string_t op;
  int ret;

  #ifdef __DEBUG__
  printf("eval_bin_op\n");
  #endif

  val = make_null();
  op = (string_t)car(expr);
  l = eval_operand(cdr(expr),env);
  r = eval_operand(cdr(cdr(expr)),env);

  if(STRCMP(op,ADD)){
    ret = *(long*)car(l) + *(long*)car(r);
  } else if(STRCMP(op,SUB)){
    ret = *(long*)car(l) - *(long*)car(r);
  } else if(STRCMP(op,MUL)){
    ret = *(long*)car(l) * *(long*)car(r);
  } else if(STRCMP(op,DIV)){
    ret = *(long *)car(l) / *(long *)car(r);
  } else {
    exit(1);
  }

  val = add_number(val,ret);

  return val;
}

static list_t *eval_func_call(list_t *expr,map_t *env){

  list_t *val;

#ifdef __DEBUG__
  printf("eval_func_call\n");
#endif

  val = make_null();
  val = add_number(val,FALSE);
  
  return val;
}

static list_t *eval_ternary(list_t *expr,map_t *env){

  list_t *val;
  list_t *cond;
  int ret;

#ifdef __DEBUG__
  printf("eval_ternary\n");
#endif

  cond = eval_operand(car(cdr(expr)),env);
  ret = *(int *)car(cond);

  if(ret){
	val = eval_operand(car(cdr(cdr(expr))),env);
  } else {
	val = eval_operand(car(cdr(cdr(cdr(expr)))),env);
  }

  return val;
}
