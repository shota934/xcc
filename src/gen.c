// 
//
//  Copyright 2019 Shota Hisai. Released under the MIT license.
//
//
#include <stdio.h>
#include <stdarg.h>
#include "gen.h"
#include "mem.h"
#include "inst.inc"

#define EMIT_NO_INDENT(ei,...) emitf(ei,0x00,__VA_ARGS__)
#define EMIT(ei,...)        emitf(ei,__LINE__, "\t" __VA_ARGS__)
#define GET_OUTPUT_FILE(ei)        FILE_GET_FP(ei->output_file)

#define IS_FUNC_DEF(n) STRCMP(FUNC_DEF,n)

static list_t *get_args(list_t *lst);
static list_t *get_local_vars(list_t *lst);
static bool_t has_args(list_t *lst);

static void emitf(gen_info_t *ei,int line, char *fmt, ...);

static list_t *gen_funcdef(gen_info_t *gi,env_t *env,list_t *lst);
static list_t *gen_function(gen_info_t *gi,env_t *env,list_t *lst);
static list_t *gen_body(gen_info_t *gi,env_t *env,list_t *lst);
static list_t *gen_integer(gen_info_t *gi,env_t *env,list_t *lst);
static list_t *gen_ret(gen_info_t *gi);
static list_t *gen_args(gen_info_t *gi,env_t *env,list_t *lst);
static list_t *gen_local_vars(gen_info_t *gi,env_t *env,list_t *lst);
static list_t *gen_symbol(gen_info_t *gi,env_t *env,list_t *lst);
static list_t *gen_return(gen_info_t *gi,env_t *env,list_t *lst);

gen_info_t *create_gen_info(){

  gen_info_t *gi;
#ifdef __DEBUG__
  printf("create_gen_info\n");
#endif

  gi = mem(sizeof(gen_info_t));
  gi->stack_pos = 0;
  gi->offset = 0;

  return gi;
}

void init_gen_info(gen_info_t *gi){

#ifdef __DEBUG__
  printf("init_gen_info\n");
#endif
  gi->stack_pos = 0;
  gi->offset = 0;

  return;
}

list_t *gen(gen_info_t *gi,env_t *env,list_t *lst){

  list_t *val;
  string_t type;

#ifdef __DEBUG__
  printf("gen\n");
#endif
  if(IS_NULL_LIST(lst)){
	return make_null();
  }

  init_gen_info(gi);
  DUMP_AST(lst);
  type = car(car(lst));
  if(IS_FUNC_DEF(type)){
	val = gen_funcdef(gi,env,car(lst));
  }

  gen(gi,env,cdr(lst));

  return val;
}

void delete_gen_info(gen_info_t *gi){

  fre(gi);

  return;
}

static list_t *get_args(list_t *lst){
  return car(cdr(cdr(lst)));
}

static list_t *get_local_vars(list_t *lst){
  return car(cdr(cdr(cdr(cdr(lst)))));
}

static bool_t has_args(list_t *lst){

  if(IS_NULL_LIST(lst)){
	return FALSE;
  } else {
	return TRUE;
  }
}

static void emitf(gen_info_t *ei,int line, char *fmt, ...){

  char buf[256];
  int i = 0;
  for(char *p = fmt; *p; p++){
    if(*p == '#'){
      buf[i++] = '%';
      buf[i++] = '%';
    } else {
      buf[i++] = *p;
    }
  }
  buf[i] = '\0';

  va_list args;
  va_start(args,fmt);
  vfprintf(GET_OUTPUT_FILE(ei),buf,args);
  va_end(args);

  fprintf(GET_OUTPUT_FILE(ei),"\n");

  return;
}

static list_t *gen_funcdef(gen_info_t *gi,env_t *env,list_t *lst){

  list_t *val;
  env_t *new_env;
#ifdef __DEBUG__
  printf("gen_funcdef\n");
#endif

  new_env = extend_env(env);
  val = make_null();
  val = concat(val,gen_function(gi,env,cdr(lst)));
  val = concat(val,gen_body(gi,env,car(cdr(cdr(cdr(cdr(lst)))))));
  val = concat(val,gen_ret(gi));

  return val;
}

static list_t *gen_function(gen_info_t *gi,env_t *env,list_t *lst){

  list_t *val;
  list_t *name;
  list_t *args;

#ifdef __DEBUG__
  printf("gen_function\n");
#endif

  val = make_null();
  DUMP_AST(lst);

  name = cdr(lst);
  EMIT(gi,".text");
#ifdef __LINUX__
  EMIT(gi,".global %s",(string_t)car(name));
  EMIT_NO_INDENT(gi,"%s:",(string_t)car(name));
#else
  EMIT(gi,".global _%s",(string_t)car(name));
  EMIT_NO_INDENT(gi,"_%s:",(string_t)car(name));
#endif

  EMIT(gi,"pushq #rbp");
  EMIT(gi,"movq #rsp, #rbp");

  gen_args(gi,env,get_args(lst));
  gen_local_vars(gi,env,get_local_vars(lst));

  return val;
}

static list_t *gen_body(gen_info_t *gi,env_t *env,list_t *lst){

  list_t *val;
  list_type_t type;

#ifdef __DEBUG__
  printf("gen_body\n");
#endif
  val = make_null();
  DUMP_AST(lst);
  type = LIST_GET_TYPE(lst);
  switch(type){
  case LIST:
	val = gen_body(gi,env,car(lst));
	if(IS_NOT_NULL_LIST(cdr(lst))){
	  val = concat(val,gen_body(gi,env,cdr(lst)));
	}
	break;
  case INTEGER:
	val = gen_integer(gi,env,lst);
	break;
  case SYMBOL:
	val = gen_symbol(gi,env,lst);
	break;
  case NULL_LIST:
	break;
  default:
	break;
  }

  return val;
}

static list_t *gen_integer(gen_info_t *gi,env_t *env,list_t *lst){

  list_t *val;
  integer_t num;
#ifdef __DEBUG__
  printf("gen_integer\n");
#endif

  num = *(integer_t *)car(lst);
  EMIT(gi,"movq $%d,#rax",num);

  return add_number(make_null(),num);
}

static list_t *gen_ret(gen_info_t *gi){

#ifdef __DEBUG__
  printf("gen_ret\n");
#endif

  EMIT(gi,"movq #rbp ,#rsp");
  EMIT(gi,"popq #rbp");
  EMIT(gi,"ret");

  return make_null();
}

static list_t *gen_args(gen_info_t *gi,env_t *env,list_t *lst){

  list_t *val;
#ifdef __DEBUG__
  printf("gen_args\n");
#endif

  val = make_null();
  if(has_args(lst)){

  }

  return val;
}

static list_t *gen_local_vars(gen_info_t *gi,env_t *env,list_t *lst){

  list_t *val;
  int localarea;
#ifdef __DEBUG__
  printf("gen_local_vars\n");
#endif

  val = make_null();

  return val;
}

static list_t *gen_symbol(gen_info_t *gi,env_t *env,list_t *lst){

  string_t symbol;
  list_t *val;
#ifdef __DEBUG__
  printf("gen_symbol\n");
#endif

  val = make_null();

  symbol = (string_t)car(lst);
  if(STRCMP(RETURN,symbol)){
	val = gen_return(gi,env,cdr(lst));
  } else {

  }

  return val;
}

static list_t *gen_return(gen_info_t *gi,env_t *env,list_t *lst){

  list_t *val;
#ifdef __DEBUG__
  printf("gen_return\n");
#endif

  DUMP_AST(lst);
  val = gen_body(gi,env,car(lst));
  gen_ret(gi);

  return val;
}
