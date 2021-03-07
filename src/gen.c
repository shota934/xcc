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

char const *REGS[] = {"rdi","rsi","rdx","rcx","r8","r9"};

#define AL  "al"
#define AX  "ax"
#define EAX "eax"
#define RAX "rax"

#define MOVB "movb"
#define MOVW "movw"
#define MOVL "movl"
#define MOVQ "movq"

#define SIZE sizeof(void *)
#define EMIT_NO_INDENT(ei,...) emitf(ei,0x00,__VA_ARGS__)
#define EMIT(ei,...)        emitf(ei,__LINE__, "\t" __VA_ARGS__)
#define GET_OUTPUT_FILE(ei)        FILE_GET_FP(ei->output_file)

#define IS_FUNC_DEF(n) STRCMP(FUNC_DEF,n)
#define DECL_TYPE_GET_TYPE(l) tail(car(car(cdr(lst))))
#define DECL_TYPE_GET_NAME(l) tail(car(car(cdr(lst))))

static list_t *get_args(list_t *lst);
static list_t *get_local_vars(list_t *lst);
static bool_t has_args(list_t *lst);

static void emitf(gen_info_t *ei,int line, char *fmt, ...);

static void gen_info_add_offset(gen_info_t *gi,int offset);
static int gen_info_get_offset(gen_info_t *gi);
static void gen_info_add_localarea(gen_info_t *gi,int localarea);
static list_t *gen_funcdef(gen_info_t *gi,env_t *env,list_t *lst);
static list_t *gen_func_name(gen_info_t *gi,env_t *env,list_t *lst);
static list_t *gen_function(gen_info_t *gi,env_t *env,list_t *lst);
static list_t *gen_body(gen_info_t *gi,env_t *env,list_t *lst);
static list_t *gen_oprand(gen_info_t *gi,env_t *env,list_t *lst);
static list_t *gen_integer(gen_info_t *gi,env_t *env,list_t *lst);
static list_t *gen_ret(gen_info_t *gi);
static list_t *gen_args(gen_info_t *gi,env_t *env,list_t *lst);
static void gen_local_vars(gen_info_t *gi,env_t *env,list_t *lst);
static int gen_loacl_ints(gen_info_t *gi,env_t *env,list_t *lst);
static int gen_loacl_int(gen_info_t *gi,env_t *env,list_t *lst);

static symbol_t *factory_symbol(gen_info_t *gi,list_t *lst);

static list_t *gen_symbol(gen_info_t *gi,env_t *env,list_t *lst);
static list_t *gen_decl_var(gen_info_t *gi,env_t *env,list_t *lst);
static list_t *gen_return(gen_info_t *gi,env_t *env,list_t *lst);
static list_t *gen_assign(gen_info_t *gi,env_t *env,list_t *lst);
static list_t *lookup_symbol(env_t *env,string_t name);
static list_t *gen_func_parms(gen_info_t *gi,env_t *env,list_t *lst);
static int gen_func_parm(gen_info_t *gi,env_t *env,list_t *lst);
static symbol_t *gen_func_parm_primitive(gen_info_t *gi,env_t *env,list_t *lst);
static void push(gen_info_t *gi,string_t reg);
static void pop(gen_info_t *gi,string_t reg);

static integer_t select_size(list_t *lst);
static string_t select_reg(integer_t size);
static string_t select_inst(integer_t size);

gen_info_t *create_gen_info(){

  gen_info_t *gi;
#ifdef __DEBUG__
  printf("create_gen_info\n");
#endif

  gi = mem(sizeof(gen_info_t));
  gi->stack_pos = 0;
  gi->offset = 0;
  gi->localarea = 0;

  return gi;
}

void init_gen_info(gen_info_t *gi){

#ifdef __DEBUG__
  printf("init_gen_info\n");
#endif
  gi->stack_pos = 0;
  gi->offset = 0;
  gi->localarea = 0;

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

static void emitf(gen_info_t *gi,int line, char *fmt, ...){

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
  vfprintf(GET_OUTPUT_FILE(gi),buf,args);
  va_end(args);

  fprintf(GET_OUTPUT_FILE(gi),"\n");

  return;
}

static void gen_info_add_offset(gen_info_t *gi,int offset){

  gi->offset += offset;

  return;
}

static int gen_info_get_offset(gen_info_t *gi){
  return gi->offset;
}

static void gen_info_add_localarea(gen_info_t *gi,int localarea){

  gi->localarea += localarea;

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

static list_t *gen_func_name(gen_info_t *gi,env_t *env,list_t *lst){

  list_t *val;
  string_t name;

#ifdef __DEBUG__
  printf("gen_func_name\n");
#endif

  val = make_null();
  name = car(lst);
  EMIT(gi,".text");
#ifdef __LINUX__
  EMIT(gi,".global %s",name);
  EMIT_NO_INDENT(gi,"%s:",name);
#else
  EMIT(gi,".global _%s",name);
  EMIT_NO_INDENT(gi,"_%s:",name);
#endif

  return add_symbol(make_null(),name);
}

static list_t *gen_function(gen_info_t *gi,env_t *env,list_t *lst){

  list_t *val;
  list_t *args;

#ifdef __DEBUG__
  printf("gen_function\n");
#endif

  val = make_null();
  DUMP_AST(lst);

  gen_func_name(gi,env,cdr(lst));

  push(gi,"rbp");
  EMIT(gi,"movq #rsp, #rbp");

  gen_args(gi,env,get_args(lst));
  gen_local_vars(gi,env,get_local_vars(lst));

  if(gi->localarea){
	EMIT(gi,"subq $-%d, %rsp",gi->localarea);
  }

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

static list_t *gen_operand(gen_info_t *gi,env_t *env,list_t *lst){

  list_t *val;
  list_type_t type;

#ifdef __DEBUG__
  printf("gen_operand\n");
#endif

  val = make_null();
  DUMP_AST(lst);
  type = LIST_GET_TYPE(lst);
  switch(type){
  case LIST:
	val = gen_operand(gi,env,car(lst));
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

  if(gi->localarea){
	EMIT(gi,"addq $%d, #rsp",gi->localarea);
  }

  EMIT(gi,"movq #rbp ,#rsp");
  pop(gi,"rbp");
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
	gen_func_parms(gi,env,lst);
  }

  return val;
}

static void gen_local_vars(gen_info_t *gi,env_t *env,list_t *lst){

  list_t *val;
  list_t *type_lst;
  int localarea;

#ifdef __DEBUG__
  printf("gen_local_vars\n");
#endif

  localarea = 0;
  val = make_null();
  dump_ast(type_lst);
  localarea += gen_loacl_ints(gi,env,lst);

  gen_info_add_localarea(gi,localarea);

  return;
}

static int gen_loacl_ints(gen_info_t *gi,env_t *env,list_t *lst){

  list_t *val;
  int localarea;
  list_t *p;
#ifdef __DEBUG__
  printf("gen_loacl_ints\n");
#endif

  p = lst;
  localarea = 0;
  while(IS_NOT_NULL_LIST(p)){
	localarea += gen_loacl_int(gi,env,car(p));
	p = cdr(p);
  }

  return localarea;
}

static int gen_loacl_int(gen_info_t *gi,env_t *env,list_t *lst){

  int size;

#ifdef __DEBUG__
  printf("gen_loacl_int\n");
#endif

  return select_size(DECL_TYPE_GET_TYPE(lst));
}

static symbol_t *factory_symbol(gen_info_t *gi,list_t *lst){

  symbol_t *sym;
  int size;
  int offset;

#ifdef __DEBUG__
  printf("factory_symbol\n");
#endif

  dump_ast(lst);
  sym = create_symbol();
  size = select_size(lst);
  offset = gen_info_get_offset(gi);
  offset -= size;
  gen_info_add_offset(gi,size);
  SYMBOL_SET_SIZE(sym,size);
  SYMBOL_SET_OFFSET(sym,offset);
  SYMBOL_SET_TYPE_LST(sym,lst);

  return sym;
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
  } else if(STRCMP(DECL_VAR,symbol)){
	val = gen_decl_var(gi,env,car(cdr(lst)));
  } else if(STRCMP(ASSIGN,symbol)){
	val = gen_assign(gi,env,cdr(lst));
  } else {
	val = lookup_symbol(env,symbol);
  }

  return val;
}

static list_t *gen_decl_var(gen_info_t *gi,env_t *env,list_t *lst){

  list_t *val;
  symbol_t *symbol;
  string_t name;

#ifdef __DEBUG__
  printf("gen_decl_var\n");
#endif

  dump_ast(lst);

  name = car(cdr(lst));
  symbol = factory_symbol(gi,car(lst));
  insert_obj(env,name,symbol);

  return val;
}

static list_t *gen_assign(gen_info_t *gi,env_t *env,list_t *lst){

  list_t *r;
  list_t *l;
  symbol_t *sym;
  string_t op;
  string_t reg;

#ifdef __DEBUG__
  printf("gen_assign\n");
#endif

  // LHS
  l = gen_operand(gi,env,lst);
  sym = (symbol_t *)car(l);

  // RHS
  r = gen_operand(gi,env,cdr(lst));

  op = select_inst(SYMBOL_GET_SIZE(sym));
  reg = select_reg(SYMBOL_GET_SIZE(sym));

  EMIT(gi,"%s #%s,%d(#rbp)",op,reg,SYMBOL_GET_OFFSET(sym));

  return make_null();
}

static list_t *lookup_symbol(env_t *env,string_t name){

  symbol_t *sym;

#ifdef __DEBUG__
  printf("lookup_symbol\n");
#endif

  sym = lookup_obj(env,name);
  if(!sym){
	exit(1);
  }

  return cons(make_null(),sym);
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

static list_t *gen_func_parms(gen_info_t *gi,env_t *env,list_t *lst){

  list_t *val;
  list_t *p;
  int localarea;

#ifdef __DEBUG__
  printf("gen_func_parms\n");
#endif

  val = make_null();
  localarea = 0;
  for(p = lst; IS_NOT_NULL_LIST(p); p = cdr(p)){
	localarea += gen_func_parm(gi,env,car(p));
  }

  gen_info_add_localarea(gi,localarea);

  return val;
}

static int gen_func_parm(gen_info_t *gi,env_t *env,list_t *lst){

#ifdef __DEBUG__
  printf("gen_func_parm\n");
#endif

  return select_size(tail(car(lst)));
}

static symbol_t *gen_func_parm_primitive(gen_info_t *gi,env_t *env,list_t *lst){

  symbol_t *sym;

#ifdef __DEBUG__
  printf("gen_func_parm_primitive\n");
#endif

  sym = create_symbol();

  return sym;
}

static void push(gen_info_t *gi,string_t reg){

  EMIT(gi,"pushq #%s",reg);
  gi->stack_pos += SIZE;

  return;
}

static void pop(gen_info_t *gi,string_t reg){

  EMIT(gi,"popq #%s",reg);
  gi->stack_pos -= SIZE;

  return;
}

static integer_t select_size(list_t *lst){

  list_t *type;
  integer_t size;

#ifdef __DEBUG__
  printf("select_size");
#endif

  if(STRCMP(car(lst),"*")){
	return SIZE;
  }

  type = tail(lst);
  if(STRCMP(car(type),INT)){
	size = sizeof(int);
  } else if(STRCMP(car(type),CHAR)){
	size = sizeof(char);
  } else if(STRCMP(car(type),SHORT)){
	size = sizeof(short);
  } else if(STRCMP(car(type),LONG)){
	size = sizeof(long);
  } else if(STRCMP(car(type),FLOAT)){
	size = sizeof(float);
  } else if(STRCMP(car(type),DOUBLE)){
	size = sizeof(double);
  } else {

  }

  return size;
}

static string_t select_reg(integer_t size){

  string_t op;
#ifdef __DEBUG__
  printf("select_reg\n");
#endif

  switch(size){
  case 1:
	op = "al";
	break;
  case 2:
	op = "ax";
  case 4:
	op = "eax";
	break;
  case 8:
	op = "rax";
	break;
  }

  return op;
}

static string_t select_inst(integer_t size){

  string_t op;
#ifdef __DEBUG__
  printf("select_inst\n");
#endif

  switch(size){
  case 1:
	op = "movb";
	break;
  case 2:
	op = "movw";
  case 4:
	op = "movl";
	break;
  case 8:
	op = "movq";
	break;
  }

  return op;
}
