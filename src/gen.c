// 
//
//  Copyright 2019 Shota Hisai. Released under the MIT license.
//
//
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include "gen.h"
#include "mem.h"
#include "inst.inc"
#include "func.h"
#include "compound_type.h"
#include "tych.h"
#include "type.h"

char *REGS_64[] = {"rdi","rsi","rdx","rcx","r8","r9"};
char *REGS_32[] = {"edi","esi","edx","ecx","r8d","r9d"};
char *FLOAT_REGS[] = {"xmm0","xmm1","xmm2","xmm3","xmm4","xmm5","xmm6","xmm7"};

#define AL  "al"
#define AX  "ax"
#define EAX "eax"
#define RAX "rax"

#define CL  "cl"
#define CX  "cx"
#define ECX "ecx"
#define RCX "rcx"

#define MOVB "movb"
#define MOVW "movw"
#define MOVL "movl"
#define MOVQ "movq"

#define FUNC        "FUNC"
#define SWITCH_NEXT "next"
#define SWITCH_TEST "test"

#define LOOP_CONTINUE "next_continue"
#define NO_LINE -1

#define SIZE sizeof(void *)
#define EMIT_NO_INDENT(gi,...) emitf(gi,0x00,__VA_ARGS__)
#define EMIT(gi,...)        emitf(gi,__LINE__, "\t" __VA_ARGS__)
#define GET_OUTPUT_FILE(gi)        FILE_GET_FP(gi->output_file)

#define IS_FUNC_DEF(n)   STRCMP(FUNC_DEF,n)
#define IS_UNION_DEF(n)   STRCMP(UNION_DEF,n)
#define IS_FUNC_DECL(n)  STRCMP(FUNC_DECL,n)
#define IS_STRUCT_DEF(n) STRCMP(STRUCT_DEF,n)
#define IS_STRUCT_DECL(n) STRCMP(STRUCT_DECL,n)
#define IS_TYPEDEF(n)    STRCMP(TYPEDEF,n)
#define IS_ENUM(n)       STRCMP(ENUM,n)
#define DECL_TYPE_GET_TYPE(l) tail(lst)
#define IS_ASSIGN(g) g->flag_of_assign
#define ASSIGN_ON(g)  g->flag_of_assign = TRUE
#define ASSIGN_OFF(g) g->flag_of_assign = FALSE

#define INIT_ENUM_VALUE(gi)  gi->enum_value = 0
#define SET_ENUM_VALUE(gi,v) gi->enum_value = v
#define GET_ENUM_VALUE(gi)   gi->enum_value
#define INCREMENT_ENUM_VALUE(gi) gi->enum_value++

#define ENUM_NAME(l) cdr(l)
#define ENUM_LIST(l) cdr(cdr(l))

#define INFO_SET_MAP(gi,m)               gi->map = map
#define INFO_GET_MAP(gi)                 gi->map
#define GET_LABEL(gi)   gi->label

static list_t *get_args(list_t *lst);
static list_t *get_ret(list_t *lst);
static list_t *get_body(list_t *lst);
static list_t *get_local_vars(list_t *lst);
static bool_t has_args(list_t *lst);
static int calc_arg_offset(gen_info_t *gi, int size);
static int calc_offset(gen_info_t *gi, int size);

static void emitf(gen_info_t *gi,int line, char *fmt, ...);

static void init_pos(gen_info_t *gi);
static void gen_info_add_pos(gen_info_t *gi,int pos);
static int gen_info_get_pos(gen_info_t *gi);

static void gen_info_add_stack_pos(gen_info_t *gi,int pos);
static int gen_info_get_stack_pos(gen_info_t *gi);
static void gen_info_add_no_align_localarea(gen_info_t *gi,int localarea);
static int gen_info_get_no_align_localarea(gen_info_t *gi);
static void gen_info_set_lhs_type(gen_info_t *gi, list_t *lhs_type);
static list_t *gen_info_get_lhs_type(gen_info_t *gi);

static void gen_info_add_localarea(gen_info_t *gi,int localarea);
static int gen_info_get_localarea(gen_info_t *gi);

static list_t *gen_funcdef(gen_info_t *gi,env_t *env,list_t *lst);
static list_t *gen_func_name(gen_info_t *gi,list_t *lst);
static list_t *gen_global_var(gen_info_t *gi,env_t *env,list_t *lst);
static list_t *gen_function(gen_info_t *gi,env_t *env,list_t *lst,env_t *penv);
static list_t *gen_body(gen_info_t *gi,env_t *env,list_t *lst);
static list_t *gen_oprand(gen_info_t *gi,env_t *env,list_t *lst);
static list_t *gen_integer(gen_info_t *gi,env_t *env,list_t *lst);
static list_t *gen_char(gen_info_t *gi,env_t *env,list_t *lst);
static list_t *gen_string(gen_info_t *gi,env_t *env,list_t *lst);
static list_t *gen_floating_point(gen_info_t *gi,env_t *env,list_t *lst);
static list_t *gen_ret(gen_info_t *gi);
static list_t *gen_args(gen_info_t *gi,env_t *env,list_t *lst);
static type_t get_type(list_t *lst);
static list_t *gen_funcdecl(gen_info_t *gi,env_t *env,list_t *lst);

static void gen_local_vars(gen_info_t *gi,env_t *env,list_t *lst);
static int gen_loacl_ints(gen_info_t *gi,env_t *env,list_t *lst);
static int gen_loacl_int(gen_info_t *gi,env_t *env,list_t *lst);

static symbol_t *factory_symbol(gen_info_t *gi,env_t *env,list_t *lst,scope_t scope);
static symbol_t *factory_member(gen_info_t *gi,env_t *env,list_t *lst,integer_t offset);

static list_t *gen_symbol(gen_info_t *gi,env_t *env,list_t *lst);
static list_t *gen_decl_var(gen_info_t *gi,env_t *env,list_t *lst);
static list_t *gen_return(gen_info_t *gi,env_t *env,list_t *lst);
static list_t *gen_assign(gen_info_t *gi,env_t *env,list_t *lst);
static list_t *gen_assign_inst(gen_info_t *gi,list_t *lst);
static list_t *gen_complex_symbol(gen_info_t *gi,env_t *env,list_t *lst,symbol_t *sym);
static list_t *gen_symbol_var(gen_info_t *gi,env_t *env,symbol_t *sym,list_t *lst);
static list_t *gen_symbol_ptr(gen_info_t *gi,env_t *env,symbol_t *sym,list_t *lst);
static list_t *gen_call(gen_info_t *gi,env_t *env,list_t *lst);
static list_t *gen_call_func(gen_info_t *gi,env_t *env,func_t *func,list_t *lst);
static list_t *gen_call_func_ptr(gen_info_t *gi,env_t *env,symbol_t *sym);
static list_t *gen_call_args(gen_info_t *gi,env_t *env,list_t *lst);
static void pop_int(gen_info_t *gi,int len);
static void pop_float(gen_info_t *gi,int len);
static list_t *gen_bit_op(gen_info_t *gi,env_t *env,list_t *lst);
static list_t *gen_bit_shift_op(gen_info_t *gi,env_t *env,list_t *lst);
static list_t *gen_logical_not(gen_info_t *gi,env_t *env,list_t *lst);
static list_t *gen_not(gen_info_t *gi,env_t *env,list_t *lst);
static list_t *gen_add(gen_info_t *gi,env_t *env,list_t *lst);
static list_t *gen_op(gen_info_t *gi,env_t *env,list_t *lst,char op);
static list_t *gen_bin_op(gen_info_t *gi,env_t *env,list_t *lst,char op);
static list_t *gen_fp_op(gen_info_t *gi,env_t *env,list_t *lst,type_t type,char op);
static list_t *gen_sub(gen_info_t *gi,env_t *env,list_t *lst);
static list_t *gen_mul(gen_info_t *gi,env_t *env,list_t *lst);
static list_t *gen_div(gen_info_t *gi,env_t *env,list_t *lst);
static list_t *gen_div_int(gen_info_t *gi,env_t *env,list_t *lst);
static list_t *gen_div_floating_value(gen_info_t *gi,env_t *env,list_t *lst,type_t type);
static list_t *gen_array(gen_info_t *gi,env_t *env,list_t *lst,symbol_t *sym,bool_t recursive,list_t *type_lst);
static list_t *gen_array_index(gen_info_t *gi,env_t *env,list_t *lst,symbol_t *sym,list_t *type_lst);
static list_t *gen_pointer_op(gen_info_t *gi,env_t *env,list_t *lst,symbol_t *sym);

static list_t *gen_bin_cmp_op(gen_info_t *gi,env_t *env,list_t *lst);

static list_t *gen_sizeof(gen_info_t *gi,env_t *env,list_t *lst);
static list_t *gen_sizeof_sym(gen_info_t *gi,env_t *env,list_t *lst);
static list_t *gen_sizeof_expr(gen_info_t *gi,env_t *env,list_t *lst);
static list_t *gen_type(gen_info_t *gi,env_t *env,list_t *lst);

static list_t *categorize(env_t *env,list_t *lst);
static list_t *categorize_type(env_t *env,list_t *lst);

static void gen_save_regs(gen_info_t *gi,list_t *lst);
static void gen_restore_regs(gen_info_t *gi,list_t *lst);
static list_t *lookup_symbol(gen_info_t *gi,env_t *env,list_t *lst);
static list_t *gen_load(gen_info_t *gi,env_t *env,list_t *lst);
static list_t *gen_local_load(gen_info_t *gi,env_t *env,list_t *lst,symbol_t *sym);
static list_t *gen_load_inst(gen_info_t *gi,list_t *lst,bool_t flg);
static list_t *gen_global_load(gen_info_t *gi,env_t *env,list_t *lst,object_t *obj);
static list_t *gen_enum_load(gen_info_t *gi,enumdef_t *enumdef);

static list_t *gen_func_parms(gen_info_t *gi,env_t *env,list_t *lst);
static int gen_func_parms_size(gen_info_t *gi,env_t *env,list_t *lst);
static int gen_func_parm(gen_info_t *gi,env_t *env,list_t *lst,int index);
static void load_parm(gen_info_t *gi,symbol_t *sym,int index);
static char *gen_cmp_inst(list_t *lst);
static void gen_cmp(gen_info_t *gi);
static void gen_je(gen_info_t *gi,char *label);
static void gen_jmp(gen_info_t *gi,char *label);

static list_t *gen_llabel(gen_info_t *gi,list_t *lst);
static void gen_label(gen_info_t *gi,char *label);
static list_t *gen_goto(gen_info_t *gi,env_t *env,list_t *lst);
static list_t *gen_if(gen_info_t *gi,env_t *env,list_t *lst);
static list_t *gen_do_while(gen_info_t *gi,env_t *env,list_t *lst);
static list_t *gen_while(gen_info_t *gi,env_t *env,list_t *lst);
static list_t *gen_for(gen_info_t *gi,env_t *env,list_t *lst);
static list_t *gen_switch(gen_info_t *gi,env_t *env,list_t *lst);
static list_t *gen_switch_cases(gen_info_t *gi,env_t *env,list_t *lst);
static list_t *gen_switch_case(gen_info_t *gi,env_t *env,list_t *lst);
static list_t *gen_case(gen_info_t *gi,env_t *env,list_t *lst);
static list_t *gen_default(gen_info_t *gi,env_t *env,list_t *lst);
static list_t *gen_cases_stmt(gen_info_t *gi,env_t *env,list_t *lst);
static list_t *gen_break(gen_info_t *gi,env_t *env,list_t *lst);
static list_t *gen_struct_assign(gen_info_t *gi,env_t *env,list_t *lst);
static list_t *gen_continue(gen_info_t *gi,env_t *env,list_t *lst);
static list_t *gen_increment(gen_info_t *gi,env_t *env,list_t *lst);
static list_t *gen_increment_assign(gen_info_t *gi,env_t *env,list_t *lst);
static list_t *gen_cast(gen_info_t *gi,env_t *env,list_t *lst);
static list_t *gen_ternary(gen_info_t *gi,env_t *env,list_t *lst);
static list_t *gen_array_list(gen_info_t *gi,env_t *env,list_t *lst);

static list_t *gen_block(gen_info_t *gi,env_t *env,list_t *lst);
static int gen_array_values(gen_info_t *gi,env_t *env,list_t *lst,int pos,int cnt,int size);
static int gen_array_value(gen_info_t *gi,env_t *env,list_t *lst,int pos,int cnt,int size);
static void gen_cast_char(gen_info_t *gi,type_t src_type);
static void gen_cast_int(gen_info_t *gi,type_t src_type);
static void gen_cast_long(gen_info_t *gi,type_t src_type);
static char *choose_increment_op(list_t *lst);

static void gen_enum_elements(gen_info_t *gi,env_t *env,list_t *lst,string_t enum_class);
static void gen_enum_element(gen_info_t *gi,env_t *env,list_t *lst,string_t enum_class);
static list_t *gen_enum_value(gen_info_t *gi,env_t *env,list_t *lst);
static list_t *gen_struct_ref(gen_info_t *gi,env_t *env,list_t *lst);
static list_t *gen_struct_decl(gen_info_t *gi,env_t *env,list_t *lst);
static list_t *gen_struct_def(gen_info_t *gi,env_t *env,list_t *lst);
static list_t *gen_enum(gen_info_t *gi,env_t *env,list_t *lst);
static list_t *gen_union(gen_info_t *gi,env_t *env,list_t *lst);
static list_t *gen_asm(gen_info_t *gi,env_t *env,list_t *lst);
static integer_t gen_members(gen_info_t *gi,env_t *env,list_t *lst, int offset);
static integer_t get_operand_size(list_t *lst);
static integer_t calc_mem_offset(list_t *lst);
static integer_t calc_mem_size(list_t *lst);
static int calc_struct_size(gen_info_t *gi,env_t *env,list_t *lst);
static void push(gen_info_t *gi,string_t reg);
static void pop(gen_info_t *gi,string_t reg);
static void pop_only_inst(gen_info_t *gi,string_t reg);
static void push_xmm(gen_info_t *gi,string_t reg);
static void pop_xmm(gen_info_t *gi,string_t reg);

static list_t *eval_type(gen_info_t *gi,env_t  *env,list_t *lst);
static integer_t select_size(gen_info_t *gi,env_t  *env,list_t *lst,bool_t flg);
static list_t *select_array_size(gen_info_t *gi,env_t *env,list_t *lst,int size);

static integer_t select_compound_type(env_t *env,list_t *lst);
static string_t select_reg(integer_t size);
static string_t select_reg_c(integer_t size);
static string_t select_inst(integer_t size);
static string_t select_inst_fp(integer_t size);
static string_t select_op(char op);
static string_t select_op_fp(char op);
static string_t select_movs_inst(integer_t size);
static bool_t is_need_padding(gen_info_t *gi);
static bool_t is_unary(list_t *lst);
static func_t *make_func(list_t *lst,scope_t scope);
static list_t *gen_typedef(gen_info_t *gi,env_t *env,list_t *lst);
static integer_t get_offset(list_t *lst);
static char* make_label(gen_info_t *gi);
static int get_size_of_array(gen_info_t *gi,list_t *lst,env_t *env);

static int calc(gen_info_t *gi,list_t *lst,env_t *env);
static int calc_symbol(gen_info_t *gi,list_t *lst,env_t *env);
static int calc_integer(gen_info_t *gi,list_t *lst,env_t *env);
static int calc_add(gen_info_t *gi,list_t *lst,env_t *env);
static int calc_sub(gen_info_t *gi,list_t *lst,env_t *env);
static int calc_mul(gen_info_t *gi,list_t *lst,env_t *env);
static int calc_div(gen_info_t *gi,list_t *lst,env_t *env);
static int calc_mod(gen_info_t *gi,list_t *lst,env_t *env);
static int calc_sizeof(gen_info_t *gi,list_t *lst,env_t *env);
static int calc_type(gen_info_t *gi,list_t *lst,env_t *env);
static int calc_ternary(gen_info_t *gi,list_t *lst,env_t *env);
static int calc_bit_shift(gen_info_t *gi,list_t *lst,env_t *env);
static int calc_bin_cmp_op(gen_info_t *gi,list_t *lst,env_t *env);
static int calc_not(gen_info_t *gi,list_t *lst,env_t *env);
static int calc_load(gen_info_t *gi,env_t *env,string_t name);

static int align(int localarea);
static int calc_mem_alignment(int offset,int size);
static bool_t is_float_sym(list_t *lst);
static void conv_ftoi(gen_info_t *gi,list_t *lst);
static bool_t is_array_type(list_t *lst);
static int calc_sizeof_dimension(list_t *lst);

gen_info_t *create_gen_info(){

  gen_info_t *gi;
  map_t *map;

#ifdef __DEBUG__
  printf("create_gen_info\n");
#endif

  gi = mem(sizeof(gen_info_t));
  gi->src = NULL;
  gi->output_file = NULL;
  gi->stack_pos = 0;
  gi->pos = 0;
  gi->localarea = 0;
  gi->no_align_localarea = 0;
  gi->flag_of_assign = FALSE;
  gi->call_flag = FALSE;
  gi->label = 0;
  gi->clabel = -1;
  gi->eval_type = FALSE;
  gi->lhs_type = make_null();

  map = map_create();
  INFO_SET_MAP(gi,map);

  return gi;
}

void init_gen_info(gen_info_t *gi){

#ifdef __DEBUG__
  printf("init_gen_info\n");
#endif
  gi->stack_pos = 8;
  gi->pos = 0;
  gi->localarea = 0;
  gi->no_align_localarea = 0;
  gi->flag_of_assign = FALSE;
  gi->call_flag = FALSE;
  gi->clabel = -1;
  gi->lhs_type = make_null();

  return;
}

list_t *gen(gen_info_t *gi,env_t *env,list_t *lst){

  list_t *val;
  list_t *v;
  list_t *l;
  string_t type;

#ifdef __DEBUG__
  printf("gen\n");
#endif

  l = lst;
  val = make_null();
  
  while(TRUE){

	if(IS_NULL_LIST(l)){
	  break;
	}

	init_gen_info(gi);
	type = car(car(l));
	if(IS_FUNC_DEF(type)){
	  v = gen_funcdef(gi,env,car(l));
	} else if(IS_FUNC_DECL(type)){
	  v = gen_funcdecl(gi,env,car(l));
	} else if(IS_ENUM(type)){
	  v = gen_enum(gi,env,cdr(car(l)));
	} else if(IS_STRUCT_DEF(type)){
	  v = gen_struct_def(gi,env,cdr(car(l)));
	} else if(IS_STRUCT_DECL(type)){
	  v = gen_struct_decl(gi,env,cdr(car(l)));
	} else if(IS_UNION_DEF(type)){
	  v = gen_union(gi,env,cdr(car(l)));
	} else if(IS_TYPEDEF(type)){
	  v = gen_typedef(gi,env,cdr(car(l)));
	} else if(STRCMP(DECL_VAR,type)){
	  v = gen_global_var(gi,env,cdr(l));
	} else {
	  printf("<--------------\n");
	  DUMP_AST(l);
	  printf("<--------------\n");
	  exit(1);
	}

	l = cdr(l);
	val = concat(val,v);
  }

  return val;
}

void delete_gen_info(gen_info_t *gi){

  fre(gi);

  return;
}

static list_t *get_args(list_t *lst){
  list_t *l;
  DUMP_AST(lst);
  l = car(cdr(lst));
  DUMP_AST(l);
  return l;
}

static list_t *get_ret(list_t *lst){
  list_t *l;
  DUMP_AST(lst);
  l = car(cdr(cdr(lst)));
  DUMP_AST(l);
  return l;
}

static list_t *get_body(list_t *lst){
  return car(cdr(cdr(cdr(get_func_name(cdr(lst))))));
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

static int calc_arg_offset(gen_info_t *gi, int size){

  int offset;
  int alignment;

  alignment = calc_mem_alignment(gi->pos,size);
  if(0 < alignment){
	gi->pos += alignment;
  }

  gen_info_add_pos(gi,size);
  offset = -gi->pos;

  return offset;
}

static int calc_offset(gen_info_t *gi, int size){

  int offset;
  int alignment;
#ifdef __DEBUG__
  printf("calc_offset\n");
#endif

  alignment = calc_mem_alignment(gi->pos,size);
  if(0 < alignment){
	gi->pos += alignment;
  }

  offset = gi->pos - gi->no_align_localarea;
  gen_info_add_pos(gi,size);

  return offset;
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

static void init_pos(gen_info_t *gi){

  gi->pos = 0;

  return;
}

static void gen_info_add_pos(gen_info_t *gi,int pos){

  gi->pos += pos;

  return;
}

static int gen_info_get_pos(gen_info_t *gi){
  return gi->pos;
}

static void gen_info_add_stack_pos(gen_info_t *gi,int pos){

  gi->stack_pos += pos;

  return;
}

static int gen_info_get_stack_pos(gen_info_t *gi){
  return gi->stack_pos;
}

static void gen_info_add_localarea(gen_info_t *gi,int localarea){

  gi->localarea += localarea;

  return;
}

static int  gen_info_get_localarea(gen_info_t *gi){
  return gi->localarea;
}

static void gen_info_add_no_align_localarea(gen_info_t *gi,int localarea){

  gi->no_align_localarea += localarea;

  return;
}
static int gen_info_get_no_align_localarea(gen_info_t *gi){
  return gi->no_align_localarea;
}

static void gen_info_set_lhs_type(gen_info_t *gi, list_t *lhs_type){
  gi->lhs_type = lhs_type;
  return;
}

static list_t *gen_info_get_lhs_type(gen_info_t *gi){
  return gi->lhs_type;
}

static list_t *gen_funcdecl(gen_info_t *gi,env_t *env,list_t *lst){

  list_t *val;
  list_t *p;
  func_t *func;
  string_t name;
  object_t *obj;

#ifdef __DEBUG__
  printf("gen_funcdecl\n");
#endif

  val = make_null();
  p = get_func_name(cdr(lst));
  name = car(p);

  obj = lookup_obj(env,name);
  if(obj){
	exit(1);
  }

  func = create_func(get_args(p),get_ret(p),UNDEFINED);
  insert_obj(env,name,func);

  val = add_symbol(val,name);

  return val;
}

static list_t *gen_funcdef(gen_info_t *gi,env_t *env,list_t *lst){

  env_t *new_env;
  list_t *val;
  list_t *func;
  string_t name;

#ifdef __DEBUG__
  printf("gen_funcdef\n");
#endif

  val = make_null();
  func = get_func_name(cdr(lst));
  name = (string_t)car(func);
  if(!set_find_obj(GEN_INFO_GET_SET(gi),name)){
	return val;
  }

  new_env = extend_env(env);
  val = concat(val,gen_function(gi,new_env,get_func_name(cdr(lst)),env));
  val = concat(val,gen_body(gi,new_env,get_body(lst)));
  val = concat(val,gen_ret(gi));

  if(gen_info_get_localarea(gi)){
	gen_info_add_stack_pos(gi,-gi->localarea);
  }

  gen_info_add_stack_pos(gi,-8);

  return val;
}

static list_t *gen_global_var(gen_info_t *gi,env_t *env,list_t *lst){

  list_t *val;
#ifdef __DEBUG__
  printf("gen_global_var\n");
#endif

  val = make_null();

  return val;
}

static list_t *gen_func_name(gen_info_t *gi,list_t *lst){

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

static list_t *gen_function(gen_info_t *gi,env_t *env,list_t *lst,env_t *penv){

  func_t *func;
  list_t *val;
  list_t *args;
  string_t name;
  int size;

#ifdef __DEBUG__
  printf("gen_function\n");
#endif

  val = make_null();

  name = car(lst);
  gen_func_name(gi,lst);

  push(gi,"rbp");
  EMIT(gi,"movq #rsp, #rbp");

  gen_args(gi,env,get_args(lst));
  gen_local_vars(gi,env,get_local_vars(lst));

  if(gen_info_get_no_align_localarea(gi)){
	gi->localarea = align(gen_info_get_no_align_localarea(gi));
	EMIT(gi,"subq $%d, %rsp",gi->localarea);
	gen_info_add_stack_pos(gi,gi->localarea);
  } else {
	gi->localarea = gen_info_get_no_align_localarea(gi);
	gen_info_add_stack_pos(gi,gi->localarea);
  }

  if(!lookup_obj(env,name)){
	func = make_func(lst,GLOBAL);
	insert_obj(penv,name,func);
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
  case DECIMAL:
	val = gen_floating_point(gi,env,lst);
	break;
  case STRING:
	val = gen_string(gi,env,lst);
	break;
  case CHARACTER:
	val = gen_char(gi,env,lst);
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
  case DECIMAL:
	val = gen_floating_point(gi,env,lst);
	break;
  case NULL_LIST:
	break;
  case STRING:
	val = gen_string(gi,env,lst);
	break;
  case CHARACTER:
	val = gen_char(gi,env,lst);
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

  return add_number(make_null(),sizeof(int));
}

static list_t *gen_char(gen_info_t *gi,env_t *env,list_t *lst){

  list_t *val;
  char ch;
#ifdef __DEBUG__
  printf("gen_char\n");
#endif

  val = make_null();
  ch = *(char *)car(lst);
  EMIT(gi,"movb $%d,#al",ch);

  return add_number(make_null(),sizeof(char));
}

static list_t *gen_string(gen_info_t *gi,env_t *env,list_t *lst){

  string_t l;
  list_t *val;

#ifdef __DEBUG__
  printf("gen_string\n");
#endif

  val = make_null();
  EMIT(gi,".data");
  l = make_label(gi);
  gen_label(gi,l);
  EMIT(gi,".string %s",(string_t)car(lst));
  EMIT(gi,".text");
  EMIT(gi,"lea %s(#rip), #rax",l);

  val = add_number(val,TYPE_STRING);
  val = add_number(val,strlen(car(lst)));
  val = add_number(val,SIZE);

  return val;
}

static list_t *gen_floating_point(gen_info_t *gi,env_t *env,list_t *lst){

  list_t *val;
  string_t l;

#ifdef __DEBUG__
  printf("gen_floating_point\n");
#endif

  val = make_null();
  l = make_label(gi);
  EMIT(gi,".data");
  gen_label(gi,l);
  float fval= strtof(car(lst),NULL);
  EMIT(gi,".quad %lu",*(unsigned int *)&fval);
  EMIT(gi,".text");
  EMIT(gi,"movsd %s(#rip), #xmm0", l);

  return val;
}

static list_t *gen_ret(gen_info_t *gi){

#ifdef __DEBUG__
  printf("gen_ret\n");
#endif

  if(gi->localarea){
	EMIT(gi,"addq $%d, #rsp",gi->localarea);
  }

  EMIT(gi,"movq #rbp ,#rsp");
  pop_only_inst(gi,"rbp");
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

static type_t get_type(list_t *lst){

  if(length_of_list(lst) != 3){
	return TYPE_INT;
  }

  return *(type_t *)car(cdr(cdr(lst)));
}

static void gen_local_vars(gen_info_t *gi,env_t *env,list_t *lst){

  list_t *val;
  list_t *type_lst;
  int localarea;

#ifdef __DEBUG__
  printf("gen_local_vars\n");
#endif

  localarea = 0;
  init_pos(gi);
  val = make_null();
  localarea += gen_loacl_ints(gi,env,lst);

  gen_info_add_no_align_localarea(gi,localarea);

  return;
}

static int gen_loacl_ints(gen_info_t *gi,env_t *env,list_t *lst){

  list_t *val;
  int localarea;
  int size;
  list_t *p;
#ifdef __DEBUG__
  printf("gen_loacl_ints\n");
#endif

  p = lst;
  localarea = 0;
  while(IS_NOT_NULL_LIST(p)){
	size = gen_loacl_int(gi,env,cdr(cdr(car(p))));
	localarea += calc_mem_alignment(localarea,size);
	localarea += size;
	p = cdr(p);
  }

  return localarea;
}

static int gen_loacl_int(gen_info_t *gi,env_t *env,list_t *lst){

  int size;

#ifdef __DEBUG__
  printf("gen_loacl_int\n");
#endif

  return select_size(gi,env,lst,FALSE);
}

static symbol_t *factory_symbol(gen_info_t *gi,env_t *env,list_t *lst,scope_t scope){

  symbol_t *sym;
  int size;
  int offset;
  int alignment;

#ifdef __DEBUG__
  printf("factory_symbol\n");
#endif

  sym = create_symbol(eval_type(gi,env,lst));
  size = select_size(gi,env,lst,FALSE);

  switch(scope){
  case ARGMENT:
	offset = calc_arg_offset(gi,size);
	break;
  default:
	offset = calc_offset(gi,size);
	break;
  }

  SYMBOL_SET_SCOPE(sym,scope);
  SYMBOL_SET_SIZE(sym,size);
  SYMBOL_SET_OFFSET(sym,offset);

  return sym;
}

static symbol_t *factory_member(gen_info_t *gi,env_t *env,list_t *lst,integer_t offset){

  symbol_t *sym;
  int size;
  int alignment;
#ifdef __DEBUG__
  printf("factory_member\n");
#endif

  sym = create_symbol(eval_type(gi,env,lst));
  size = select_size(gi,env,lst,FALSE);
  alignment = calc_mem_alignment(-offset,size);
  if(0 < alignment){
	offset += alignment;
  }

  SYMBOL_SET_SCOPE(sym,MEMBER);
  SYMBOL_SET_SIZE(sym,size);
  SYMBOL_SET_OFFSET(sym,offset);

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
	val = gen_decl_var(gi,env,cdr(lst));
  } else if(STRCMP(ASSIGN,symbol)){
	val = gen_assign(gi,env,cdr(lst));
  } else if(STRCMP(ADD,symbol)){
	val = gen_add(gi,env,lst);
  } else if(STRCMP(SUB,symbol)){
	val = gen_sub(gi,env,lst);
  } else if(STRCMP(MUL,symbol)){
	val = gen_mul(gi,env,lst);
  } else if(STRCMP(DIV,symbol)
			|| STRCMP(MOD,symbol)){
	val = gen_div(gi,env,lst);
  } else if(STRCMP(FUNC_CALL,symbol)){
	val = gen_call(gi,env,cdr(lst));
  } else if(STRCMP(FOR,symbol)){
	val = gen_for(gi,env,cdr(lst));
  } else if(STRCMP(DO,symbol)){
	val = gen_do_while(gi,env,cdr(lst));
  } else if(STRCMP(WHILE,symbol)){
	val = gen_while(gi,env,cdr(lst));
  } else if(STRCMP(IF,symbol)){
	val = gen_if(gi,env,cdr(lst));
  } else if(STRCMP(SWITCH,symbol)){
	val = gen_switch(gi,env,cdr(lst));
  } else if(STRCMP(BREAK,symbol)){
	val = gen_break(gi,env,lst);
  } else if(STRCMP(DOT,symbol)){
	val = gen_struct_assign(gi,env,cdr(lst));
  }else if(STRCMP(REF_MEMBER_ACCESS,symbol)){
	val = gen_struct_ref(gi,env,cdr(lst));
  } else if(STRCMP(SIZEOF,symbol)){
	val = gen_sizeof(gi,env,cdr(lst));
  } else if(STRCMP(TYPE,symbol)){
	val = gen_type(gi,env,cdr(lst));
  } else if((STRCMP(symbol,INCREMENT))
			|| (STRCMP(symbol,DECREMENT))){
	val = gen_increment(gi,env,lst);
  } else if((STRCMP(symbol,INCREMNT_ASSING))
			|| (STRCMP(symbol,DECREMNT_ASSING))){
	val = gen_increment_assign(gi,env,lst);
  } else if((STRCMP(symbol,LESS))
			|| (STRCMP(symbol,GREATER))
			|| (STRCMP(symbol,LESS_EQAUL))
			|| (STRCMP(symbol,GREATER_EQUAL))
			|| (STRCMP(symbol,EQUAL))
			|| (STRCMP(symbol,NOT_EQUAL))){
	val = gen_bin_cmp_op(gi,env,lst);
  } else if(STRCMP(symbol,NOT)){
	val = gen_logical_not(gi,env,lst);
  } else if(STRCMP(car(lst),BIT_REVERSAL)){
    val = gen_not(gi,env,lst);
  } else if((STRCMP(symbol,AND))
			|| (STRCMP(symbol,OR))
			|| (STRCMP(symbol,XOR))){
    val = gen_bit_op(gi,env,lst);
  } else if(STRCMP(symbol,CONTINUTE)){
    gen_continue(gi,env,cdr(lst));
  } else if(STRCMP(symbol,BIT_LEFT_SHIFT)
			|| STRCMP(symbol,BIT_RIGHT_SHIFT)){
	val = gen_bit_shift_op(gi,env,lst);
  } else if(STRCMP(symbol,TYPEDEF)){
	val = gen_typedef(gi,env,cdr(lst));
  } else if(STRCMP(symbol,CAST)){
	val = gen_cast(gi,env,cdr(lst));
  } else if(STRCMP(symbol,TERNARY)){
	val = gen_ternary(gi,env,cdr(lst));
  } else if(STRCMP(symbol,LABEL)){
	val = gen_llabel(gi,cdr(lst));
  } else if(STRCMP(symbol,GOTO)){
	val = gen_goto(gi,env,cdr(lst));
  } else if(STRCMP(symbol,ASM)){
	val = gen_asm(gi,env,cdr(lst));
  } else if(STRCMP(symbol,ARRAY_LIST)){
	val = gen_array_list(gi,env,cdr(lst));
  } else if(STRCMP(symbol,BLOCK)){
	val = gen_block(gi,env,cdr(lst));
  } else {
	if(IS_ASSIGN(gi)){
	  val = lookup_symbol(gi,env,lst);
	} else {
	  val = gen_load(gi,env,lst);
	}
  }

  return val;
}

static list_t *gen_decl_var(gen_info_t *gi,env_t *env,list_t *lst){

  list_t *val;
  symbol_t *sym;
  string_t name;

#ifdef __DEBUG__
  printf("gen_decl_var\n");
#endif

  name = car(lst);
  sym = factory_symbol(gi,env,cdr(lst),LOCAL);
  insert_obj(env,name,sym);
  val = add_symbol(make_null(),name);

  return val;
}

static list_t *gen_assign(gen_info_t *gi,env_t *env,list_t *lst){

  list_t *r;
  list_t *l;
  list_t *val;
  symbol_t *sym;
  string_t op;
  string_t reg;

#ifdef __DEBUG__
  printf("gen_assign\n");
#endif

  ASSIGN_ON(gi);
  l = gen_operand(gi,env,lst);
  gen_info_set_lhs_type(gi,l);
  if(is_name(l)){
	l = gen_operand(gi,env,l);
  }
  ASSIGN_OFF(gi);
  r = gen_operand(gi,env,cdr(lst));
  gen_info_set_lhs_type(gi,make_null());
  if(IS_NULL_LIST(r)){
	val = r;
  } else {
	val = gen_assign_inst(gi,l);
  }

  return val;
}

static list_t *gen_assign_inst(gen_info_t *gi,list_t *lst){

  list_t *val;
  string_t op;
  string_t reg;
  int size;

#ifdef __DEBUG__
  printf("gen_assign_inst\n");
#endif

  val = make_null();
  if(is_array(lst)){
	size = *(integer_t *)car(cdr(lst));
	op = select_inst(size);
	reg = select_reg(size);
	EMIT(gi,"%s #%s,(%rcx)",op,reg);
  } else if(is_pointer(lst)){
	EMIT(gi,"movq #rax,(#rcx)");
  } else if(is_struct_ref(lst)){
	EMIT(gi,"movq #rax,%d(#rcx)",*(integer_t *)car(cdr(car(cdr(lst)))));
  } else if(is_value(lst)){
	if(is_float_sym(car(lst))){
	  op = select_inst_fp(*(integer_t *)car(car(lst)));
	  EMIT(gi,"%s #xmm0,%d(#rbp)",op,*(integer_t *)car(cdr(car(lst))));
	} else {
	  op = select_inst(*(integer_t *)car(car(lst)));
	  reg = select_reg(*(integer_t *)car(car(lst)));
	  EMIT(gi,"%s #%s,%d(#rbp)",op,reg,*(integer_t *)car(cdr(car(lst))));
	}
  } else if(IS_NULL_LIST(lst)){
	return val;
  } else {
	exit(1);
  }

  return val;
}

static list_t *gen_complex_symbol(gen_info_t *gi,env_t *env,list_t *lst,symbol_t *sym){

  list_t *val;
  list_t *type_lst;
  string_t name;
  int size;
  int offset;

#ifdef __DEBUG__
  printf("gen_complex_symbol\n");
#endif

  val = make_null();
  if(!IS_SYMBOL(lst)){
	val = gen_symbol_var(gi,env,sym,lst);
  } else {
	name = car(lst);
	if(STRCMP(ARRAY,name)){
	  val = gen_array(gi,env,lst,sym,FALSE,make_null());
	} else if(STRCMP(ADDRESS,name)){
	  offset = SYMBOL_GET_OFFSET(sym);
	  val = add_number(val,offset);
	  val = add_symbol(val,ADDRESS);
	} else if(STRCMP(POINTER,name)){
	  EMIT(gi,"movq %d(#rbp),#rcx",SYMBOL_GET_OFFSET(sym));
	  val = add_symbol(val,POINTER);
	  val = concat(val,gen_symbol_ptr(gi,env,sym,cdr(lst)));
	} else {
	  type_lst = SYMBOL_GET_TYPE_LST(sym);
	  size = SYMBOL_GET_SIZE(sym);
	  offset = SYMBOL_GET_OFFSET(sym);
	  val = add_number(val,conv_type(env,type_lst,lst));
	  val = add_number(val,offset);
	  val = add_number(val,size);
	  val = add_list(make_null(),val);
	}
  }

  return val;
}

static list_t *gen_symbol_var(gen_info_t *gi,env_t *env,symbol_t *sym,list_t *lst){

  list_t *val;
  list_t *type_lst;
  string_t name;
  int size;
  int offset;

  val = make_null();
#ifdef __DEBUG__
  printf("gen_symbol_var\n");
#endif

  type_lst = SYMBOL_GET_TYPE_LST(sym);
  name = car(type_lst);
  size = SYMBOL_GET_SIZE(sym);
  offset = SYMBOL_GET_OFFSET(sym);

  val = add_number(val,conv_type(env,type_lst,lst));
  val = add_number(val,offset);
  val = add_number(val,size);

  if(STRCMP(ARRAY,name)){
	val = add_symbol(val,ARRAY);
  } else {
	val = add_list(make_null(),val);
  }

  return val;
}

static list_t *gen_symbol_ptr(gen_info_t *gi,env_t *env,symbol_t *sym,list_t *lst){

  list_t *val;
  string_t name;
  int size;
  int offset;

#ifdef __DEBUG__
  printf("gen_symbol_ptr\n");
#endif

  val = make_null();
  if(IS_NULL_LIST(lst)){
	return val;
  } else if(IS_SYMBOL(lst)){
	name = car(lst);
	if(STRCMP(POINTER,name)){
	  val = add_symbol(val,POINTER);
	  val = concat(val,gen_symbol_ptr(gi,env,sym,cdr(lst)));
	} else if(STRCMP(RESTRICT,name)){
	  val = concat(val,gen_symbol_ptr(gi,env,sym,cdr(lst)));
	}
  } else {
	exit(1);
  }

  return val;
}

static list_t *gen_call(gen_info_t *gi,env_t *env,list_t *lst){

  list_t *val;
  list_t *args;
  object_t *obj;
  int len_args;
  bool_t fop;
  list_t *cl;

#ifdef __DEBUG__
  printf("gen_call\n");
#endif

  fop = FALSE;
  args = car(cdr(lst));

  cl = categorize(env,args);

  // Save registers
  gen_save_regs(gi,cl);

  // Parameter Passing
  gen_call_args(gi,env,cl);

  if(is_need_padding(gi)){
	EMIT(gi,"subq $8,%rsp");
	gen_info_add_stack_pos(gi,8);
	fop = TRUE;
  }

  obj = lookup_obj(env,car(lst));

  if(!obj){
	warn(LIST_GET_SYMBOL_LINE_NO(lst),LIST_GET_SYMBOL_SRC(lst),
		 "implicit declaration of function '%s'",(string_t)car(lst));
	return make_null();
  }

  if(is_func(obj)){
	val = gen_call_func(gi,env,(func_t *)obj,lst);
  } else if(is_symbol(obj)){
	val = gen_call_func_ptr(gi,env,(symbol_t *)obj);
  }

  if(fop){
	EMIT(gi,"addq $8,%rsp");
	gen_info_add_stack_pos(gi,-8);
  }

  // Restore registers
  gen_restore_regs(gi,cl);

  return val;
}

static list_t *gen_call_func(gen_info_t *gi,env_t *env,func_t *func,list_t *lst){

  list_t *val;
  int size;

#ifdef __DEBGU__
  printf("gen_call_func\n");
#endif

  val = make_null();
#ifdef __LINUX__
  EMIT(gi,"callq %s",car(lst));
#else
  EMIT(gi,"callq _%s",car(lst));
#endif

  size = select_size(gi,env,FUNC_GET_RET_TYPE(func),FALSE);
  val = add_number(val,size);

  return val;
}

static list_t *gen_call_func_ptr(gen_info_t *gi,env_t *env,symbol_t *sym){

  list_t *val;
  int size;

#ifdef __DEBUG__
  printf("gen_call_func_ptr\n");
#endif

#ifdef __LINUX__
  EMIT(gi,"movq %d(#rbp), #rax",SYMBOL_GET_OFFSET(sym));
  EMIT(gi,"call *#rax");
#endif

  val = make_null();
  size = select_size(gi,env,SYMBOL_GET_RET_LST(sym),FALSE);

  val = add_number(val,size);

  return val;
}

static list_t *gen_call_args(gen_info_t *gi,env_t *env,list_t *lst){

  list_t *val;
  list_t *v;
  list_t *p;
  list_t *arg;
  int len;
  int size;

#ifdef __DEBUG__
  printf("gen_call_args\n");
#endif

  val = make_null();

  gi->call_flag = TRUE;
  // rest
  for(p = car(cdr(cdr(lst))); IS_NOT_NULL_LIST(p); p = cdr(p)){
	arg = car(p);
  }

  // float
  len = 0;
  for(p = car(cdr(lst)); IS_NOT_NULL_LIST(p); p = cdr(p)){
	arg = car(p);
	v = gen_operand(gi,env,arg);
	val = concat(val,v);
	push_xmm(gi,"xmm0");
	len++;
  }
  pop_float(gi,len);

  // integer and pointer
  len = 0;
  for(p = car(lst); IS_NOT_NULL_LIST(p); p = cdr(p)){
	arg = car(p);
	v = gen_operand(gi,env,arg);
	size = *(integer_t *)car(v);
	if(size < SIZE){
	  EMIT(gi,"%s #%s,#%s",select_movs_inst(size),select_reg(size),"rax");
	}
	val = concat(val,v);
	push(gi,"rax");
	len++;
  }

  pop_int(gi,len);

  gi->call_flag = FALSE;

  return val;
}

static void pop_int(gen_info_t *gi,int len){

  int i;
#ifdef __DEBUG__
  printf("pop_float\n");
#endif

  for(i = len - 1; i >= 0; i--){
	pop(gi,REGS_64[i]);
  }

  return;
}

static void pop_float(gen_info_t *gi,int len){

  int i;
#ifdef __DEBUG__
  printf("pop_float\n");
#endif

  for(i = len - 1; i >= 0; i--){
	pop_xmm(gi,FLOAT_REGS[i]);
  }

  return;
}

static void gen_save_regs(gen_info_t *gi,list_t *lst){

  int i;
  list_t *p;
  int len;

#ifdef __DEBUG__
  printf("gen_save_regs\n");
#endif

  p = car(lst);
  len = length_of_list(p);
  for(i = 0; i < len; i++){
	push(gi,REGS_64[i]);
  }

  p = car(cdr(lst));
  len = length_of_list(p);
  for(i = 0; i < len; i++){
	push_xmm(gi,FLOAT_REGS[i]);
  }

  p = car(cdr(cdr(lst)));
  len = length_of_list(p);
  for(i = 0; i < len; i++){

  }

  return;
}

static void gen_restore_regs(gen_info_t *gi,list_t *lst){

  list_t *p;
  int i;
  int len;
#ifdef __DEBUG__
  printf("gen_restore_regs\n");
#endif

  p = car(lst);
  len = length_of_list(p);
  for(i = len - 1; i >= 0; i--){
	pop(gi,REGS_64[i]);
  }

  p = car(cdr(lst));
  len = length_of_list(p);
  for(i = len - 1; i >= 0; i--){
	pop_xmm(gi,FLOAT_REGS[i]);
  }

  p = car(cdr(cdr(lst)));
  len = length_of_list(p);
  for(i = len - 1; i >= 0; i--){

  }

  return;
}

static list_t *gen_bit_op(gen_info_t *gi,env_t *env,list_t *lst){

  string_t op;
  list_t *val;
  list_t *l;
  list_t *r;
  int szl;
  int szr;

#ifdef __DBEUG__
  printf("gen_bit_op\n");
#endif

  if(STRCMP((string_t)car(lst),AND)){
	op = "and";
  } else if(STRCMP((string_t)car(lst),OR)){
	op = "or";
  } else if(STRCMP((string_t)car(lst),XOR)){
	op = "xor";
  } else {
    exit(1);
  }

  l = gen_operand(gi,env,cdr(lst));
  szl = get_operand_size(l);
  if(szl < SIZE){
	EMIT(gi,"%s #%s,#%s",select_movs_inst(szl),select_reg(szl),"rax");
  }
  push(gi,"rax");
  r = gen_operand(gi,env,cdr(cdr(lst)));
  szr = get_operand_size(r);
  if(szr < SIZE){
	EMIT(gi,"%s #%s,#%s",select_movs_inst(szr),select_reg(szr),"rax");
  }
  pop(gi,"rcx");
  EMIT(gi,"%s #rcx, #rax",op);

  val = make_null();
  val = add_number(val,SIZE);

  return val;
}

static list_t *gen_bit_shift_op(gen_info_t *gi,env_t *env,list_t *lst){

  string_t op;
  list_t *val;
  list_t *l;
  list_t *r;
  int szl;
  int szr;

#ifdef __DEBUG__
  printf("gen_bit_shift_op\n");
#endif

  if(STRCMP((string_t)car(lst),BIT_LEFT_SHIFT)){
    op = "shl";
  } else if(STRCMP((string_t)car(lst),BIT_RIGHT_SHIFT)){
    op = "shr";
  } else {
    exit(1);
  }

  l = gen_operand(gi,env,cdr(lst));
  szl = get_operand_size(l);
  if(szl < SIZE){
	EMIT(gi,"%s #%s,#%s",select_movs_inst(szl),select_reg(szl),"rax");
  }
  push(gi,"rax");
  r = gen_operand(gi,env,cdr(cdr(lst)));
  szr = get_operand_size(r);
  if(szr < SIZE){
	EMIT(gi,"%s #%s,#%s",select_movs_inst(szr),select_reg(szr),"rax");
  }
  pop(gi,"rcx");
  EMIT(gi,"%s #rcx, #rax",op);

  val = make_null();
  val = add_number(val,SIZE);

  return val;
}

static list_t *gen_add(gen_info_t *gi,env_t *env,list_t *lst){

  list_t *val;
  type_t type;

#ifdef __DEBUG__
  printf("gen_add\n");
#endif

  val = gen_op(gi,env,lst,'+');

  return val;
}

static list_t *gen_op(gen_info_t *gi,env_t *env,list_t *lst,char op){

  list_t *val;
  type_t type;

#ifdef __DEBUG__
  printf("gen_op\n");
#endif

  type = check_type(env,lst);
  switch(type){
  case TYPE_CHAR:
  case TYPE_SHORT:
  case TYPE_LONG:
  case TYPE_INT:
  case TYPE_UNSIGNED:
  case TYPE_SIGNED:
  case TYPE_POINTER:
	val = gen_bin_op(gi,env,cdr(lst),op);
	break;
  case TYPE_FLOAT:
	val = gen_fp_op(gi,env,cdr(lst),type,op);
	break;
  default:
	exit(1);
  }

  return val;
}

static list_t *gen_bin_op(gen_info_t *gi,env_t *env,list_t *lst,char op){

  list_t *val;
  list_t *r;
  list_t *l;
  int szl;
  int szr;

#ifdef __DEBUG__
  printf("gen_bin_op\n");
#endif

  val = make_null();
  l = gen_operand(gi,env,lst);
  szl = get_operand_size(l);
  if(szl < SIZE){
	EMIT(gi,"%s #%s,#%s",select_movs_inst(szl),select_reg(szl),"rax");
  }

  push(gi,"rax");

  r = gen_operand(gi,env,cdr(lst));
  pop(gi,"rcx");
  szr = get_operand_size(r);
  if(szr < SIZE){
	EMIT(gi,"%s #%s,#%s",select_movs_inst(szr),select_reg(szr),"rax");
  }
  EMIT(gi,"%s #rax,#rcx",select_op(op));
  EMIT(gi,"movq #rcx,#rax");

  val = add_number(val,SIZE);

  return val;
}

static list_t *gen_fp_op(gen_info_t *gi,env_t *env,list_t *lst,type_t type,char op){

  list_t *val;
  list_t *l;
  list_t *r;

#ifdef __DEBUG__
  printf("gen_fp_op\n");
#endif

  val = make_null();
  l = gen_operand(gi,env,lst);
  conv_ftoi(gi,cdr(cdr(l)));
  push_xmm(gi,"xmm0");
  r = gen_operand(gi,env,cdr(lst));
  conv_ftoi(gi,cdr(cdr(r)));

  pop_xmm(gi,"xmm1");
  EMIT(gi,"%s #xmm0,#xmm1",select_op_fp(op));
  EMIT(gi,"movsd #xmm1,#xmm0");

  if(type == TYPE_FLOAT){
	EMIT(gi,"cvtsd2ss #xmm0,#xmm0");
	val = add_number(val,sizeof(float));
  } else {
	val = add_number(val,sizeof(double));
  }

  return val;
}

static list_t *gen_sub(gen_info_t *gi,env_t *env,list_t *lst){

  list_t *val;

#ifdef __DEBUG__
  printf("gen_sub\n");
#endif

  val = gen_op(gi,env,lst,'-');

  return val;
}

static list_t *gen_mul(gen_info_t *gi,env_t *env,list_t *lst){

  list_t *val;
  type_t type;
#ifdef __DEBUG__
  printf("gen_sub\n");
#endif

  val = gen_op(gi,env,lst,'*');

  return val;
}

static list_t *gen_mul_int(gen_info_t *gi,env_t *env,list_t *lst){

  list_t *val;
  list_t *r;
  list_t *l;
  int szl;
  int szr;

#ifdef __DEBUG__
  printf("gen_mul\n");
#endif

  val = make_null();
  l = gen_operand(gi,env,lst);
  szl = *(integer_t *)car(l);
  if(szl < SIZE){
	EMIT(gi,"%s #%s,#%s",select_movs_inst(szl),select_reg(szl),"rax");
  }
  push(gi,"rax");

  r = gen_operand(gi,env,cdr(lst));
  pop(gi,"rcx");
  szr = *(integer_t *)car(r);
  if(szr < SIZE){
	EMIT(gi,"%s #%s,#%s",select_movs_inst(szr),select_reg(szr),"rax");
  }

  EMIT(gi,"imulq #rcx,#rax");

  val = add_number(val,SIZE);

  return val;
}

static list_t *gen_div(gen_info_t *gi,env_t *env,list_t *lst){

  list_t *val;
  type_t type;
#ifdef __DEBUG__
  printf("gen_div\n");
#endif

  type = check_type(env,lst);
  switch(type){
  case TYPE_INT:
	val = gen_div_int(gi,env,lst);
	break;
  case TYPE_POINTER:
	exit(1);
	break;
  case TYPE_FLOAT:
	val = gen_fp_op(gi,env,cdr(lst),type,'/');
	break;
  default:
	exit(1);
  }

  return val;
}

static list_t *gen_div_int(gen_info_t *gi,env_t *env,list_t *lst){

  list_t *val;
  list_t *r;
  list_t *l;
  int szl;
  int szr;
  string_t op;

#ifdef __DEBUG__
  printf("gen_div_int\n");
#endif

  val = make_null();
  l = gen_operand(gi,env,cdr(lst));
  szl = *(integer_t *)car(l);
  if(szl < SIZE){
	EMIT(gi,"%s #%s,#%s",select_movs_inst(szl),select_reg(szl),"rax");
  }
  push(gi,"rax");

  r = gen_operand(gi,env,cdr(cdr(lst)));
  szr = *(integer_t *)car(r);
  if(szr < SIZE){
	EMIT(gi,"%s #%s,#%s",select_movs_inst(szr),select_reg(szr),"rax");
  }

  EMIT(gi,"movq #rax,#rcx");
  pop(gi,"rax");

  EMIT(gi,"movq $0,#rdx");
  EMIT(gi,"cltd");
  EMIT(gi,"idivq #rcx");

  op = car(lst);
  if(*op == '%'){
	EMIT(gi,"movq #rdx,#rax");
  }
  val = add_number(val,SIZE);

  return val;
}

static list_t *gen_div_floating_value(gen_info_t *gi,env_t *env,list_t *lst,type_t type){

     list_t *val;

#ifdef __DEBUG__
  printf("gen_mul_floating_value\n");
#endif

  val = make_null();
  gen_operand(gi,env,lst);
  push_xmm(gi,"xmm0");
  gen_operand(gi,env,cdr(lst));
  pop_xmm(gi,"xmm1");
  EMIT(gi,"divsd #xmm0,#xmm1");
  EMIT(gi,"movsd #xmm1,#xmm0");

  if(type == TYPE_FLOAT){
	EMIT(gi,"cvtsd2ss #xmm0,#xmm0");
	val = add_number(val,sizeof(float));
  } else {
	val = add_number(val,sizeof(double));
  }

  return val;
}

static list_t *gen_array(gen_info_t *gi,env_t *env,list_t *lst,symbol_t *sym,bool_t recursive,list_t *type_lst){

  list_t *val;
  string_t name;

#ifdef __DEBUG__
  printf("gen_array\n");
#endif

  val = make_null();
  if(IS_NULL_LIST(lst)){
	pop(gi,"rcx");
	return val;
  } else {
	if(recursive){
	  pop(gi,"rcx");
	  EMIT(gi,"movq #rcx,#rax");
	} else {
	  switch(SYMBOL_GET_SCOPE(sym)){
	  case LOCAL:
		EMIT(gi,"leaq %d(#rbp),#rax",SYMBOL_GET_OFFSET(sym));
		break;
	  case ARGMENT:
		EMIT(gi,"movq %d(#rbp),#rax",SYMBOL_GET_OFFSET(sym));
		break;
	  default:
		break;
	  }
	}

	if(!recursive){
	  type_lst = SYMBOL_GET_TYPE_LST(sym);
	}
	push(gi,"rax");
	val = gen_array_index(gi,env,lst,sym,cdr(cdr(type_lst)));
	pop(gi,"rax");
	EMIT(gi,"addq #rax,#rcx");
	push(gi,"rcx");
	val = concat(val,gen_array(gi,env,cdr(cdr(lst)),sym,TRUE,cdr(cdr(type_lst))));

	return val;
  }
}

static char *gen_cmp_inst(list_t *lst){

  string_t inst = NULL;
#ifdef __DEBUG__
  printf("gen_cmp_inst\n");
#endif
  if(STRCMP(car(lst),LESS)){
    inst = "setl";
  } else if(STRCMP(car(lst),LESS_EQAUL)){
    inst = "setle";
  } else if(STRCMP(car(lst),GREATER)){
    inst = "setg";
  } else if(STRCMP(car(lst),GREATER_EQUAL)){
    inst = "setge";
  } else if(STRCMP(car(lst),EQUAL)){
    inst = "sete";
  } else if(STRCMP(car(lst),NOT_EQUAL)){
    inst = "setne";
  } else {
    error_no_info("Undefined code for [%s]",car(lst));
    exit(1);
  }

  return inst;
}

static list_t *gen_bin_cmp_op(gen_info_t *gi,env_t *env,list_t *lst){

  list_t *val;
  char *op;
  list_t *l;
  list_t *r;
  char *inst;

#ifdef __DEBUG__
  printf("gen_bin_cmp_op\n");
#endif

  inst = gen_cmp_inst(lst);
  l = cdr(lst);
  gen_body(gi,env,l);
  push(gi,"rax");
  r = cdr(cdr(lst));
  gen_body(gi,env,r);
  pop(gi,"rcx");
  EMIT(gi,"cmp #eax, #ecx");

  EMIT(gi,"%s #al", inst);
  EMIT(gi,"movzb #al, #eax");

  val = make_null();
  val = add_symbol(val,car(lst));

  return val;
}

static list_t *gen_logical_not(gen_info_t *gi,env_t *env,list_t *lst){

  list_t *val;
#ifdef __DEBUG__
  printf("gen_logical_not\n");
#endif
  gen_operand(gi,env,cdr(lst));
  push(gi,"rax");
  EMIT(gi,"mov $0, #rax");
  pop(gi,"rcx");
  EMIT(gi,"cmp #rax, #rcx");
  EMIT(gi,"sete #al");
  EMIT(gi,"movzbq #al, #rax");

  val = add_symbol(val,NOT);

  return val;
}

static list_t *gen_not(gen_info_t *gi,env_t *env,list_t *lst){

  list_t *val;
#ifdef __DEBUG__
  printf("gen_not\n");
#endif
  gen_operand(gi,env,cdr(lst));
  EMIT(gi,"not #rax");

  val = add_symbol(val,BIT_REVERSAL);

  return val;
}

static list_t *gen_pointer_op(gen_info_t *gi,env_t *env,list_t *lst,symbol_t *sym){

  list_t *val;

#ifdef __DEBUG__
  printf("gen_pointer_op\n");
#endif

  val = make_null();

  return val;
}

static list_t *gen_array_index(gen_info_t *gi,env_t *env,list_t *lst,symbol_t *sym,list_t *type_lst){

  list_t *val;
  list_t *l;
  string_t name;
  int offset;
  int size;
  int v;

#ifdef __DEBUG__
  printf("gen_array_index\n");
#endif

  val = make_null();
  val = gen_operand(gi,env,car(cdr(lst)));
  v = calc_sizeof_dimension(type_lst);
  size = select_size(gi,env,SYMBOL_GET_TYPE_LST(sym),TRUE);
  push(gi,"rax");
  EMIT(gi,"movq $%d,#rax",size * v);
  pop(gi,"rcx");
  EMIT(gi,"imulq #rcx,#rax");
  EMIT(gi,"movq #rax,#rcx");

  val = add_number(make_null(),size);
  return add_symbol(val,ARRAY);
}

static list_t *gen_sizeof(gen_info_t *gi,env_t *env,list_t *lst){

  list_t *val;
  list_type_t type;

#ifdef __DEBUG__
  printf("gen_sizeof\n");
#endif

  val = make_null();
  type = LIST_GET_TYPE(lst);
  switch(type){
  case INTEGER:
	val = add_number(val,sizeof(int));
	val = gen_operand(gi,env,val);
	break;
  case STRING:
	val = add_number(val,strlen((string_t)car(lst)));
	val = gen_operand(gi,env,val);
	break;
  case CHARACTER:
	val = add_number(val,sizeof(char));
	val = gen_operand(gi,env,val);
	break;
  case DECIMAL:
	val = add_number(val,sizeof(float));
	val = gen_operand(gi,env,val);
	break;
  case SYMBOL:
	val = gen_sizeof_sym(gi,env,lst);
	break;
  case LIST:
	val = gen_sizeof_expr(gi,env,lst);
	break;
  default:
	printf("error\n");
	break;
  }

  return val;
}

static list_t *gen_sizeof_sym(gen_info_t *gi,env_t *env,list_t *lst){

  list_t *val;
  symbol_t *sym;
  string_t name;
  int size;

#ifdef __DEBUG__
  printf("gen_sizeof_sym\n");
#endif

  name = (string_t)car(lst);
  if(STRCMP(name,TYPE)){
	val = gen_operand(gi,env,lst);
  } else {
	val = make_null();
	size = calc(gi,lst,env);
	EMIT(gi,"movq $%d,#rax",size);
	val = add_number(val,SYMBOL_GET_SIZE(sym));
  }

  return val;
}

static list_t *gen_sizeof_expr(gen_info_t *gi,env_t *env,list_t *lst){


  list_t *val;
  type_t type;

#ifdef __DEBUG__
  printf("gen_sizeof_expr\n");
#endif

  val = make_null();
  type = check_type(env,lst);
  switch(type){
  case TYPE_INT:
	val = add_number(val,sizeof(int));
	val = gen_operand(gi,env,val);
	break;
  case TYPE_POINTER:
	val = add_number(val,SIZE);
	val = gen_operand(gi,env,val);
	break;
  default:
	exit(1);
  }

  return val;
}

static list_t *gen_type(gen_info_t *gi,env_t *env,list_t *lst){

  list_t *val;
  string_t inst;
  string_t reg;
  int size;

#ifdef __DEBUG__
  printf("gen_type\n");
#endif

  val = make_null();
  size = select_size(gi,env,lst,FALSE);
  val = add_number(val,size);

  EMIT(gi,"movq $%d,#rax",size);

  return val;
}

static list_t *categorize(env_t *env,list_t *lst){

  list_t *l;
  list_t *int_lst;
  list_t *float_lst;
  list_t *other_lst;
  list_t *pp;
  list_t *p;
  type_t type;

#ifdef __DEBUG__
  printf("categorize\n");
#endif

  l = make_null();
  int_lst = make_null();
  float_lst = make_null();
  other_lst = make_null();
  for(p = lst; IS_NOT_NULL_LIST(p); p = cdr(p)){
	type = check_type(env,p);
	switch(type){
	case TYPE_CHAR:
	case TYPE_SHORT:
	case TYPE_INT:
	case TYPE_LONG:
	case TYPE_POINTER:
	case TYPE_STRING:
	case TYPE_ARRAY:
	case TYPE_ENUM:
	  int_lst = concat(int_lst,add_list(make_null(),car(p)));
	  break;
	case TYPE_FLOAT:
	case TYPE_DOUBLE:
	  float_lst = concat(float_lst,add_list(make_null(),car(p)));
	  break;
	default:
	  other_lst = concat(other_lst,add_list(make_null(),car(p)));
	  break;
	}
  }
  l = add_list(make_null(),other_lst);
  l = add_list(l,float_lst);
  l = add_list(l,int_lst);

  return l;
}

static list_t *categorize_type(env_t *env,list_t *lst){

  list_t *l;
  list_t *int_lst;
  list_t *float_lst;
  list_t *other_lst;
  list_t *pp;
  list_t *p;

#ifdef __DEBUG__
  printf("categorize_type\n");
#endif

  int_lst = make_null();
  float_lst = make_null();
  other_lst = make_null();
  for(p = lst; IS_NOT_NULL_LIST(p); p = cdr(p)){
	pp = cdr(car(p));
	if(is_inttype(pp) || is_pointer(pp) || is_array(pp)){
	  int_lst = concat(int_lst,add_list(make_null(),car(p)));
	} else if(is_float(pp)){
	  float_lst = concat(float_lst,add_list(make_null(),car(p)));
	} else {
	  other_lst = concat(other_lst,add_list(make_null(),car(p)));
	}
  }

  l = add_list(make_null(),other_lst);
  l = add_list(l,float_lst);
  l = add_list(l,int_lst);

  return l;
}

static list_t *lookup_symbol(gen_info_t *gi,env_t *env,list_t *lst){

  symbol_t *sym;
  list_t *val;

#ifdef __DEBUG__
  printf("lookup_symbol\n");
#endif

  sym = lookup_obj(env,car(lst));
  if(!sym){
	error(LIST_GET_SYMBOL_LINE_NO(lst),LIST_GET_SYMBOL_SRC(lst),"undefined variable %s",(string_t)car(lst));
	return make_null();
  }

  val = gen_complex_symbol(gi,env,cdr(lst),sym);

  return val;
}

static list_t *gen_load(gen_info_t *gi,env_t *env,list_t *lst){

  object_t *obj;
  list_t *val;

#ifdef __DEBUG__
  printf("gen_load\n");
#endif

  obj = (object_t *)lookup_obj(env,car(lst));
  if(!obj){
	error(LIST_GET_SYMBOL_LINE_NO(lst),LIST_GET_SYMBOL_SRC(lst),"undefined variable %s",(string_t)car(lst));
	return make_null();
  }

  switch(OBJ_GET_SCOPE(obj)){
  case LOCAL:
  case ARGMENT:
	val = gen_local_load(gi,env,lst,(symbol_t *)obj);
	break;
  case GLOBAL:
	val = gen_global_load(gi,env,lst,obj);
	break;
  case ENUMLATE:
	val = gen_enum_load(gi,(enumdef_t *)obj);
	break;
  default:
	val = make_null();
	break;
  }

  return val;
}

static list_t *gen_local_load(gen_info_t *gi,env_t *env,list_t *lst,symbol_t *sym){

  list_t *val;
  string_t inst;
  string_t reg;
  int offset;

#ifdef __DEBUG__
  printf("gen_local_load\n");
#endif

  val = gen_complex_symbol(gi,env,cdr(lst),sym);
  val = gen_load_inst(gi,val,FALSE);

  return val;
}

static list_t *gen_load_inst(gen_info_t *gi,list_t *lst,bool_t flg){

  list_t *val;
  string_t inst;
  string_t reg;
  int offset;
  int size;

#ifdef __DEBUG__
  printf("gen_load_inst\n");
#endif

  val = make_null();
  if(IS_NULL_LIST(lst)){
	return val;
  }

  val = make_null();
  if(gi->call_flag
	 && is_array_type(cdr(cdr(cdr(lst))))){
	offset = *(integer_t *)car(cdr(cdr(lst)));
	EMIT(gi,"leaq %d(#rbp),#rax",offset);
	val = add_number(val,*(integer_t *)car(cdr(lst)));
	val = add_number(val,SIZE);
  } else if(is_array(lst)){
	size = *(integer_t *)car(cdr(lst));
	inst = select_inst(size);
	reg = select_reg(size);
	EMIT(gi,"%s (#rcx),#%s",inst,reg);
	val = lst;
  } else if(is_pointer(lst)){
	if(flg){
	  EMIT(gi,"movq #rax,#rcx");
	}
	EMIT(gi,"movq (#rcx),#rax");
	val = add_number(val,SIZE);
	val = concat(val,gen_load_inst(gi,cdr(lst),TRUE));
  } else if(is_address(lst)){
	offset = *(integer_t *)car(cdr(lst));
	EMIT(gi,"leaq %d(#rbp),#rax",offset);
	val = add_number(val,TYPE_POINTER);
	val = add_number(val,*(integer_t *)car(cdr(lst)));
	val = add_number(val,SIZE);
  } else if(is_struct_ref(lst)){
	;
  } else if(is_value(lst)){
	size = *(integer_t *)car(car(lst));
	offset = *(integer_t *)car(cdr(car(lst)));
	if(is_float_sym(car(lst))){
	  inst = select_inst_fp(size);
	  EMIT(gi,"%s %d(#rbp),#xmm0",inst,offset);
	  if(size == 4){
		EMIT(gi,"cvtss2sd #xmm0, #xmm0");
	  }
	  val = add_number(make_null(),get_type(car(lst)));
	} else {
	  inst = select_inst(size);
	  reg = select_reg(size);
	  EMIT(gi,"%s %d(#rbp),#%s",inst,offset,reg);
	  val = add_number(make_null(),get_type(car(lst)));
	}
	val = add_number(val,offset);
	val = add_number(val,size);
  } else {
	DUMP_AST(lst);
	exit(1);
  }

  return val;
}

static list_t *gen_global_load(gen_info_t *gi,env_t *env,list_t *lst,object_t *obj){

  list_t *val;

#ifdef __DEBUG__
  printf("gen_global_load\n");
#endif

  val = make_null();
  if(is_func(obj) && is_address(cdr(lst))){
	EMIT(gi,"leaq %s(#rip),#rax",(string_t)car(lst));
	val = add_symbol(val,FUNC);
  }

  return val;
}

static list_t *gen_enum_load(gen_info_t *gi,enumdef_t *enumdef){

  integer_t num;

#ifdef __DEBUG__
  printf("gen_enum_load\n");
#endif

  num = ENUMDEF_GET_VAL(enumdef);
  EMIT(gi,"movq $%d,#rax",num);

  return add_number(make_null(),sizeof(int));
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
  list_t *l;
  list_t *int_lst;
  list_t *float_lst;
  list_t *other_lst;
  int localarea;
  int i;

#ifdef __DEBUG__
  printf("gen_func_parms\n");
#endif

  l = categorize_type(env,lst);
  val = make_null();
  localarea = 0;
  i = 0;

  int_lst = car(l);
  float_lst = car(cdr(l));
  other_lst = car(cdr(cdr(l)));

  localarea = gen_func_parms_size(gi,env,l);
  gen_info_add_no_align_localarea(gi,localarea);
  for(p = int_lst; IS_NOT_NULL_LIST(p); p = cdr(p)){
	gen_func_parm(gi,env,car(p),i);
	i++;
  }

  i = 0;
  for(p = float_lst; IS_NOT_NULL_LIST(p); p = cdr(p)){
	gen_func_parm(gi,env,car(p),i);
	i++;
  }

  i = 0;
  for(p = other_lst; IS_NOT_NULL_LIST(p); p = cdr(p)){
	gen_func_parm(gi,env,car(p),i);
	i++;
  }

  return val;
}

static int gen_func_parms_size(gen_info_t *gi,env_t *env,list_t *lst){

  list_t *p;
  list_t *l;
  list_t *int_lst;
  list_t *float_lst;
  list_t *other_lst;
  int localarea;
  int i;

#ifdef __DEBUG__
  printf("gen_func_parms_size\n");
#endif

  localarea = 0;
  int_lst = car(lst);
  float_lst = car(cdr(lst));
  other_lst = car(cdr(cdr(lst)));

  for(p = int_lst; IS_NOT_NULL_LIST(p); p = cdr(p)){
	localarea += select_size(gi,env,cdr(car(p)),FALSE);
  }

  for(p = float_lst; IS_NOT_NULL_LIST(p); p = cdr(p)){
	localarea += select_size(gi,env,cdr(car(p)),FALSE);
  }

  for(p = other_lst; IS_NOT_NULL_LIST(p); p = cdr(p)){
	localarea += select_size(gi,env,cdr(car(p)),FALSE);
  }

  return localarea;
}

static int gen_func_parm(gen_info_t *gi,env_t *env,list_t *lst,int index){

  symbol_t *sym;
  string_t name;
#ifdef __DEBUG__
  printf("gen_func_parm\n");
#endif

  name = car(lst);
  sym = factory_symbol(gi,env,cdr(lst),ARGMENT);
  insert_obj(env,name,sym);
  load_parm(gi,sym,index);

  return SYMBOL_GET_SIZE(sym);
}

static void load_parm(gen_info_t *gi,symbol_t *sym,int index){

  int size;
#ifdef __DEBUG__
  printf("load_parm\n");
#endif

  size = SYMBOL_GET_SIZE(sym);
  switch(size){
  case 1:
  case 2:
	EMIT(gi,"movq #%s,%d(#rbp)",REGS_64[index],SYMBOL_GET_OFFSET(sym));
	break;
  case 4:
	EMIT(gi,"movl #%s,%d(#rbp)",REGS_32[index],SYMBOL_GET_OFFSET(sym));
	break;
  case 8:
	EMIT(gi,"movq #%s,%d(#rbp)",REGS_64[index],SYMBOL_GET_OFFSET(sym));
	break;
  default:
	EMIT(gi,"movq #%s,%d(#rbp)",REGS_64[index],SYMBOL_GET_OFFSET(sym));
	break;
  }

  return;
}

static void gen_cmp(gen_info_t *gi){

#ifdef __DEBUG__
  printf("gen_cmp\n");
#endif

  EMIT(gi,"test #rax, #rax");

  return;
}

static void gen_je(gen_info_t *gi,char *label){

#ifdef __DEBUG__
  printf("gen_je\n");
#endif
  EMIT(gi,"je %s", label);

  return;
}

static void gen_jmp(gen_info_t *gi,char *label){

#ifdef __DEBUG__
  printf("gen_jmp\n");
#endif
  EMIT(gi,"jmp %s",label);

  return;
}

static list_t *gen_llabel(gen_info_t *gi,list_t *lst){

  list_t *new_lst;
  string_t label;

#ifdef __DEBUG__
  printf("gen_llabel\n");
#endif

  new_lst = make_null();
  label = (string_t)car(lst);
  gen_label(gi,label);

  return new_lst;
}

static void gen_label(gen_info_t *gi,char *label){

#ifdef __DEBUG__
  printf("gen_label\n");
#endif

  EMIT_NO_INDENT(gi,"%s:",label);

  return;
}

static list_t *gen_goto(gen_info_t *gi,env_t *env,list_t *lst){

  list_t *val;
  string_t l;

#ifdef __DEBUG__
  printf("gen_goto\n");
#endif

  val = make_null();
  l = (string_t)car(lst);
  gen_jmp(gi,l);

  return val;
}

static list_t *gen_if(gen_info_t *gi,env_t *env,list_t *lst){

  list_t *val;
  list_t *cond;
  list_t *conseq;
  list_t *altern;
  char *l0;

#ifdef __DEBUG__
  printf("gen_if\n");
#endif

  val = make_null();
  conseq = car(cdr(lst));
  altern = car(cdr(cdr(lst)));
  cond = car(lst);

  gen_body(gi,env,cond);
  gen_cmp(gi);
  l0 = make_label(gi);
  gen_je(gi,l0);

  gen_body(gi,env,conseq);
  if(IS_NOT_NULL_LIST(altern)){
    char *l1 = make_label(gi);
    gen_jmp(gi,l1);
    gen_label(gi,l0);
    gen_body(gi,env,altern);
    gen_label(gi,l1);
  } else {
    gen_label(gi,l0);
  }

  return val;
}

static list_t *gen_do_while(gen_info_t *gi,env_t *env,list_t *lst){

  list_t *val;
  list_t *cond;
  list_t *stmt;
  string_t l0;
  string_t l1;

#ifdef __DEBUG__
  printf("gen_do_while\n");
#endif

  stmt = car(lst);
  cond = car(cdr(lst));

  val = make_null();
  l0 = make_label(gi);
  l1 = make_label(gi);
  gen_label(gi,l0);
  gen_body(gi,env,stmt);

  gen_body(gi,env,cond);
  gen_cmp(gi);
  gen_je(gi,l1);
  gen_jmp(gi,l0);
  gen_label(gi,l1);

  return val;
}

static list_t *gen_while(gen_info_t *gi,env_t *env,list_t *lst){

  list_t *val;
  char *l0;
  char *l1;

#ifdef __DEBUG__
  printf("gen_while\n");
#endif

  val = make_null();
  l0 = make_label(gi);
  l1 = make_label(gi);
  gen_label(gi,l0);
  gen_body(gi,env,car(lst));
  gen_cmp(gi);
  gen_je(gi,l1);
  gen_body(gi,env,cdr(lst));
  gen_jmp(gi,l0);
  gen_label(gi,l1);

  return val;
}

static list_t *gen_struct_assign(gen_info_t *gi,env_t *env,list_t *lst){

  symbol_t *sym;
  symbol_t *sym_mem;
  compound_def_t *com;
  list_t *val;
  string_t mname;
  string_t name;
  int offset;
  int m_offset;
  int size;

#ifdef __DEBUG__
  printf("gen_struct_assign\n");
#endif

  val = make_null();
  DUMP_AST(lst);
  sym = lookup_obj(env,car(lst));
  if(!sym){
	exit(1);
  }

  com = lookup_obj(env,car(tail(SYMBOL_GET_TYPE_LST(sym))));
  if(!com){
	exit(1);
  }

  if(IS_LIST(cdr(lst))){
	val = gen_operand(gi,COMPOUND_TYPE_GET_ENV(com),car(cdr(lst)));
	m_offset = calc_mem_offset(val);
	size = calc_mem_size(val);
	val = make_null();
  } else {
	sym_mem = lookup_member(COMPOUND_TYPE_GET_ENV(com),car(cdr(lst)));
	m_offset = SYMBOL_GET_OFFSET(sym_mem);
	size = SYMBOL_GET_SIZE(sym_mem);
  }

  offset = SYMBOL_GET_OFFSET(sym) + m_offset;

  if(!IS_ASSIGN(gi)){
	EMIT(gi,"movq %d(#rbp),#rax",offset);
  }

  val = add_number(val,offset);
  val = add_number(val,size);
  val = add_list(make_null(),val);

  return val;
}
static list_t *gen_struct_ref(gen_info_t *gi,env_t *env,list_t *lst){

  list_t *val;
  symbol_t *sym;
  symbol_t *sym_mem;
  compound_def_t *com;
  int m_offset;
  int size;
  int base;

#ifdef __DEBUG__
  printf("gen_struct_ref\n");
#endif

  val = make_null();
  DUMP_AST(lst);
  sym = lookup_obj(env,car(lst));
  if(!sym){
	exit(1);
  }

  com = lookup_obj(env,car(tail(SYMBOL_GET_TYPE_LST(sym))));
  if(!com){
	exit(1);
  }

  if(IS_LIST(cdr(lst))){
	val = gen_operand(gi,COMPOUND_TYPE_GET_ENV(com),car(cdr(lst)));
	m_offset = calc_mem_offset(val);
	size = calc_mem_size(val);
	val = make_null();
  } else {
	sym_mem = lookup_member(COMPOUND_TYPE_GET_ENV(com),car(cdr(lst)));
	m_offset = SYMBOL_GET_OFFSET(sym_mem);
	size = SYMBOL_GET_SIZE(sym_mem);
  }

  base = SYMBOL_GET_OFFSET(sym);

  EMIT(gi,"movq %d(#rbp),#rax",base);
  EMIT(gi,"movq #rax,#rcx");
  if(!IS_ASSIGN(gi)){
	EMIT(gi,"movq %d(#rcx),#rax",m_offset);
  }

  val = add_number(val,m_offset);
  val = add_number(val,size);
  val = add_list(make_null(),val);
  val = add_symbol(val,REF_MEMBER_ACCESS);

  return val;
}

static list_t *gen_for(gen_info_t *gi,env_t *env,list_t *lst){

  list_t *val;
  char *l0;
  char *l1;
  char *l2;
  map_t *map;

#ifdef __DEBUG__
  printf("gen_for\n");
#endif

  map = INFO_GET_MAP(gi);
  val = make_null();
  l0 = make_label(gi);

  l1 = make_label(gi);
  gen_body(gi,env,car(lst));

  gen_label(gi,l0);
  gen_body(gi,env,car(cdr(lst)));
  gen_cmp(gi);
  gen_je(gi,l1);

  l2 = make_label(gi);
  map_put(map,LOOP_CONTINUE,l2);
  gen_body(gi,env,car(cdr(cdr(cdr(lst)))));

  gen_label(gi,l2);
  gen_body(gi,env,car(cdr(cdr(lst))));
  gen_jmp(gi,l0);

  gen_label(gi,l1);

  map_remove(map,LOOP_CONTINUE);

  return val;
}

static list_t *gen_switch(gen_info_t *gi,env_t *env,list_t *lst){

  string_t test;
  string_t next;
  map_t *map;
  list_t *label_lst;

#ifdef __DEBUG__
  printf("gen_switch\n");
#endif

  map = INFO_GET_MAP(gi);

  test = make_label(gi);
  next = make_label(gi);

  map_put(map,SWITCH_NEXT,next);
  map_put(map,SWITCH_TEST,test);

  gen_jmp(gi,test);
  gen_label(gi,test);

  gen_operand(gi,env,lst);
  push(gi,"rax");
  pop(gi,"rcx");

  label_lst = gen_switch_cases(gi,env,car(cdr(lst)));
  gen_cases_stmt(gi,env,label_lst);

  gen_label(gi,next);

  return make_null();
}

static list_t *gen_switch_cases(gen_info_t *gi,env_t *env,list_t *lst){

  list_t *val;
  map_t *map;
  string_t next;

#ifdef __DEBUG__
  printf("gen_switch_cases\n");
#endif

  if(IS_LIST(lst)){
    val = gen_switch_case(gi,env,car(lst));
    val = concat(val,gen_switch_cases(gi,env,cdr(lst)));
  } else {
    map = INFO_GET_MAP(gi);
    next = map_get(map,SWITCH_NEXT);
    if(!next){
      error_no_info("Undefied next label for switch statement.");
      exit(1);
    }
    gen_jmp(gi,next);
    val = make_null();
  }

  return val;
}

static list_t *gen_switch_case(gen_info_t *gi,env_t *env,list_t *lst){

  string_t label;
  list_t *val;

#ifdef __DEBUG__
  printf("gen_switch_case\n");
#endif

  label = car(lst);
  if(STRCMP(CASE,label)){
    val = gen_case(gi,env,car(cdr(lst)));
  } else if(STRCMP(DEFAULT,label)){
    val = gen_default(gi,env,car(cdr(lst)));
  } else {
    val = make_null();
  }

  return val;
}

static list_t *gen_case(gen_info_t *gi,env_t *env,list_t *lst){

  string_t l;
  list_t *val;
  map_t *map;

#ifdef __DEBUG__
  printf("gen_case\n");
#endif

  map = INFO_GET_MAP(gi);
  val = make_null();
  gen_operand(gi,env,lst);
  EMIT(gi,"cmp #eax, #ecx");

  l = make_label(gi);
  map_put(map,l,cdr(lst));
  val = add_symbol(val,l);
  gen_je(gi,l);

  return val;
}

static list_t *gen_default(gen_info_t *gi,env_t *env,list_t *lst){

  string_t l;
  list_t *val;
  map_t *map;

#ifdef __DEBUG__
  printf("gen_default\n");
#endif

  map = INFO_GET_MAP(gi);
  val = make_null();

  l = make_label(gi);
  map_put(map,l,lst);
  val = add_symbol(val,l);
  gen_jmp(gi,l);

  return val;
}

static list_t *gen_cases_stmt(gen_info_t *gi,env_t *env,list_t *lst){

  list_t *p;
  list_t *case_stmts;
  map_t *map;
  string_t l;
  string_t label_type;

#ifdef __DEBUG__
  printf("gen_case_stmt\n");
#endif

  map = INFO_GET_MAP(gi);
  for(p = lst; IS_NOT_NULL_LIST(p); p = cdr(p)){
    l = car(p);
    case_stmts = map_get(map,l);
    if(!case_stmts){
      error_no_info("Unknown label : [%s]\n",l);
      exit(1);
    }
    gen_label(gi,l);
    gen_body(gi,env,case_stmts);
  }

  return make_null();
}

static list_t *gen_break(gen_info_t *gi,env_t *env,list_t *lst){

  map_t *map;
  string_t next;

#ifdef __DEBUG__
  printf("gen_break\n");
#endif

  map = INFO_GET_MAP(gi);
  next = map_get(map,SWITCH_NEXT);

  gen_jmp(gi,next);

  return make_null();
}

static list_t *gen_continue(gen_info_t *gi,env_t *env,list_t *lst){

  string_t con;
  map_t *map;
  list_t *val;

#ifdef __DEBUG__
  printf("gen_continue\n");
#endif

  val = make_null();
  map = INFO_GET_MAP(gi);
  con = map_get(map,LOOP_CONTINUE);
  gen_jmp(gi,con);

  return val;
}

static list_t *gen_increment(gen_info_t *gi,env_t *env,list_t *lst){

  string_t reg;
  string_t op;
  list_t *l;
  int szl;
  int offset;
  symbol_t *sym;

#ifdef __DEBUG__
  printf("gen_increment\n");
#endif

  op = choose_increment_op(lst);
  l = gen_operand(gi,env,cdr(lst));
  szl = get_operand_size(l);
  if(szl < SIZE){
	EMIT(gi,"movslq #%s,#%s",select_reg(szl),"rax");
  }
  push(gi,"rax");
  EMIT(gi,"movq $%d, #rax",1);
  pop(gi,"rcx");
  if((STRCMP(car(lst),DECREMENT))
     || (STRCMP(car(lst),DECREMNT_ASSING))){
    EMIT(gi,"%s #rax, #rcx",op);
    EMIT(gi,"movq #rcx, #rax");
  } else {
    EMIT(gi,"%s #rcx, #rax",op);
  }

  sym = lookup_obj(env,car(cdr(lst)));

  offset = SYMBOL_GET_OFFSET(sym);
  op = select_inst(SYMBOL_GET_SIZE(sym));
  reg = select_reg(SYMBOL_GET_SIZE(sym));

  EMIT(gi,"%s #%s, %d(#rbp)",op,reg,offset);

  return make_null();
}

static list_t *gen_increment_assign(gen_info_t *gi,env_t *env,list_t *lst){

  char *op;
  string_t reg;
  list_t *rval;
  list_t *lval;
  symbol_t *sym;
  int offset;
  int szr;
  int szl;

#ifdef __DEBUG__
  printf("gen_increment_assign\n");
#endif

  rval = gen_operand(gi,env,cdr(lst));
  szr = get_operand_size(rval);
  if(szr < SIZE){
	EMIT(gi,"movslq #%s,#%s",select_reg(szr),"rax");
  }

  offset = get_offset(rval);
  push(gi,"rax");
  lval = gen_operand(gi,env,cdr(cdr(lst)));
  pop(gi,"rcx");

  szl = get_operand_size(lval);
  if(szl < SIZE){
	EMIT(gi,"movslq #%s,#%s",select_reg(szl),"rax");
  }

  op = choose_increment_op(lst);
  if(STRCMP(car(lst),DECREMNT_ASSING)){
    EMIT(gi,"%s #rax, #rcx",op);
    EMIT(gi,"mov #rcx, #rax");
  } else {
    EMIT(gi,"%s #rcx, #rax",op);
  }

  op = select_inst(szr);
  reg = select_reg(szr);
  EMIT(gi,"%s #%s, %d(#rbp)",op,reg,offset);

  return make_null();
}

static list_t *gen_cast(gen_info_t *gi,env_t *env,list_t *lst){

  list_t *val;
  list_t *name;
  symbol_t *symbol;
  type_t src_type;
  type_t cast_type;
  list_type_t type;

#ifdef __DEBUG__
  printf("gen_cast\n");
#endif

  val = make_null();
  cast_type = conv_type(env,car(lst),make_null());
  name = gen_operand(gi,env,cdr(lst));

  type = LIST_GET_TYPE(name);
  switch(type){
  case INTEGER:
	src_type = TYPE_INT;
	break;
  case DECIMAL:
	src_type = TYPE_FLOAT;
	break;
  case SYMBOL:
	symbol = lookup_obj(env,car(name));
	if(!symbol){
	  exit(1);
	}
	src_type = SYMBOL_GET_TYPE(symbol);
	break;
  default:
	exit(1);
	break;
  }

  switch(cast_type){
  case TYPE_INT:
    gen_cast_int(gi,src_type);
    break;
  case TYPE_LONG:
    gen_cast_long(gi,src_type);
    break;
  case TYPE_CHAR:
	gen_cast_char(gi,src_type);
	break;
  case TYPE_POINTER:
	break;
  default:
    break;
  }

  return val;
}

static list_t *gen_ternary(gen_info_t *gi,env_t *env,list_t *lst){

  list_t *cond;
  list_t *conseq;
  list_t *altern;
  list_t *val;
  string_t l0;
  string_t l1;

#ifdef __DEBUG__
  printf("gen_ternary\n");
#endif

  val = make_null();

  cond = car(lst);
  conseq = car(cdr(lst));
  altern = car(cdr(cdr(lst)));

  gen_body(gi,env,cond);
  gen_cmp(gi);
  l0 = make_label(gi);
  gen_je(gi,l0);

  gen_body(gi,env,conseq);
  l1 = make_label(gi);
  gen_jmp(gi,l1);
  gen_label(gi,l0);
  gen_body(gi,env,altern);
  gen_label(gi,l1);

  return val;
}

static list_t *gen_array_list(gen_info_t *gi,env_t *env,list_t *lst){

  list_t *val;
  list_t *lhs_type;
  symbol_t *sym;
  string_t name;
  int size;
  int cnt;

#ifdef __DEBUG__
  printf("gen_array_list\n");
#endif

  lhs_type = gen_info_get_lhs_type(gi);
  sym = lookup_obj(env,car(lhs_type));
  if(!sym){
	exit(1);
  }

  size = SYMBOL_GET_SIZE(sym);
  cnt = select_size(gi,env,SYMBOL_GET_TYPE_LST(sym),TRUE);
  gen_array_values(gi,env,car(lst),0,cnt,size);
  val =  make_null();

  return val;
}

static list_t *gen_block(gen_info_t *gi,env_t *env,list_t *lst){

  list_t *val;
#ifdef __DEBUG__
  printf("gen_block");
#endif

  val = gen_body(gi,extend_env(env),car(lst));

  return val;
}

static int gen_array_values(gen_info_t *gi,env_t *env,list_t *lst,int pos,int cnt,int size){

  int val;
  list_type_t type;
  string_t name;

#ifdef __DEBUG__
  printf("gen_array_values\n");
#endif

  type = LIST_GET_TYPE(lst);
  switch(type){
  case NULL_LIST:
	val = (pos >= 0) ? -cnt : pos;
	break;
  case LIST:
	val = gen_array_values(gi,env,cdr(lst),pos,cnt,size);
	val = gen_array_values(gi,env,car(lst),val,cnt,size);
	break;
  default:
	val = gen_array_values(gi,env,cdr(lst),pos,cnt,size);
	val = gen_array_value(gi,env,lst,val,cnt,size);
	break;
  }

  return val;
}

static int gen_array_value(gen_info_t *gi,env_t *env,list_t *lst,int pos,int cnt,int size){

  int offset;
  int v;

#ifdef __DEBUG__
  printf("gen_array_value\n");
#endif

  v = *(integer_t *)car(lst);
  offset = pos;
  EMIT(gi,"%s $%d, %d(#rbp)",select_inst(cnt),v,offset);

  return offset - cnt;
}

static void gen_cast_char(gen_info_t *gi,type_t src_type){

#ifdef __DEBUG__
  printf("gen_cast_char\n");
#endif

  switch(src_type){
  case TYPE_INT:
	EMIT(gi,"movswq #ax,#rax");
	break;
  case TYPE_SHORT:
	break;
  default:
	break;
  }

  return;
}

static void gen_cast_int(gen_info_t *gi,type_t src_type){

#ifdef __DEBUG__
  printf("gen_cast_int\n");
#endif

  switch(src_type){
  case TYPE_SHORT:
    EMIT(gi,"movswq #ax,#rax");
    break;
  case TYPE_LONG:
	break;
  default:
	break;
  }

  return;
}

static void gen_cast_long(gen_info_t *gi,type_t src_type){

#ifdef __DEBUG__
  printf("gen_cast_long\n");
#endif

  switch(src_type){
  case TYPE_SHORT:
    EMIT(gi,"movswq #ax,#rax");
    break;
  case TYPE_INT:
    EMIT(gi,"movslq #eax,#rax");
    break;
  case TYPE_LONG:
    break;
  default:
	break;
  }

  return;
}

static char *choose_increment_op(list_t *lst){

  char *op;
#ifdef __DEBUG__
  printf("choose_increment_op\n");
#endif

  op = NULL;
  if(STRCMP(car(lst),INCREMENT)){
    op = "addq";
  } else if(STRCMP(car(lst),DECREMENT)){
    op = "subq";
  } else if(STRCMP(car(lst),INCREMNT_ASSING)){
    op = "addq";
  } else if(STRCMP(car(lst),DECREMNT_ASSING)){
    op = "subq";
  } else {
	exit(1);
  }

  return op;
}

static void gen_enum_elements(gen_info_t *gi,env_t *env,list_t *lst,string_t enum_class){

#ifdef __DEBUG__
  printf("gen_enum_elements\n");
#endif

  if(IS_NOT_NULL_LIST(lst)){
    gen_enum_element(gi,env,lst,enum_class);
    gen_enum_elements(gi,env,cdr(lst),enum_class);
  }

  return;
}

static void gen_enum_element(gen_info_t *gi,env_t *env,list_t *lst,string_t enum_class){

  enumdef_t *enumdef;
  string_t name;
  int val;

#ifdef __DEBUG__
  printf("gen_enum_element\n");
#endif

  if(IS_LIST(lst)){
	  name = (string_t )car(car(lst));
	  val = calc(gi,cdr(car(lst)),env);
	  SET_ENUM_VALUE(gi,val);
  } else if(IS_SYMBOL(lst)){
	  name = (string_t )car(lst);
	  val = GET_ENUM_VALUE(gi);
  }

  enumdef = enumdef_create(val,enum_class);
  insert_obj(env,name,enumdef);
  INCREMENT_ENUM_VALUE(gi);

  return;
}

static list_t *gen_enum_value(gen_info_t *gi,env_t *env,list_t *lst){

  list_t *val;

  val = lookup(env,lst);
  if(IS_NULL_LIST(val)){
	return make_null();
  }

  EMIT(gi,"mov $%u, #rax",*(int *)car(val));

  return add_symbol(val,ENUM);
}

static list_t *gen_struct_decl(gen_info_t *gi,env_t *env,list_t *lst){

  string_t name;
  list_t *val;
  compound_def_t *com;

#ifdef __DEBUG__
  printf("gen_struct_decl\n");
#endif

  name = car(lst);
  com = create_compound_type_def(TYPE_COMPOUND,GLOBAL);
  COMPOUND_TYPE_SET_TYPE(com,STRUCT_COMPOUND_TYPE);
  insert_obj(env,name,com);
  val = add_symbol(val,name);

  return val;
}

static list_t *gen_struct_def(gen_info_t *gi,env_t *env,list_t *lst){

  list_t *val;
  string_t name;
  compound_def_t *com;
  env_t *stru_env;
  int size;

#ifdef __DEBUG__
  printf("gen_struct_def\n");
#endif

  val = make_null();
  stru_env = extend_env(env);
  name = car(lst);
  size = calc_struct_size(gi,env,car(cdr(lst)));
  com = create_compound_type_def(TYPE_COMPOUND,GLOBAL);
  COMPOUND_TYPE_SET_TYPE(com,STRUCT_COMPOUND_TYPE);
  COMPOUND_TYPE_SET_ENV(com,stru_env);
  size = gen_members(gi,stru_env,car(cdr(lst)),0);
  COMPOUND_TYPE_SET_SIZE(com,size);
  insert_obj(env,name,com);
  val = add_symbol(val,name);

  return val;
}

static list_t *gen_enum(gen_info_t *gi,env_t *env,list_t *lst){

  list_t *enum_lst;
  list_t *new_enum_lst;
  string_t name;
  symbol_t *sym;

#ifdef __DEBUG__
  printf("gen_enum\n");
#endif

  sym = create_symbol(make_null());
  SYMBOL_SET_TYPE(sym,TYPE_ENUM);
  name = car(lst);
  INIT_ENUM_VALUE(gi);
  gi->eval_type = TRUE;
  gen_enum_elements(gi,env,car(cdr(lst)),name);
  gi->eval_type = FALSE;
  insert_obj(env,name,sym);
  INIT_ENUM_VALUE(gi);

  return make_null();
}

static list_t *gen_union(gen_info_t *gi,env_t *env,list_t *lst){

#ifdef __DEBUG__
  printf("gen_union\n");
#endif

  list_t *val;
  string_t name;
  compound_def_t *com;
  list_t *body;
  env_t *union_env;

  val = make_null();
  body = car(cdr(lst));

  name = car(lst);
  union_env = extend_env(env);
  com = create_compound_type_def(TYPE_COMPOUND,UNDEFINED);
  COMPOUND_TYPE_SET_TYPE(com,STRUCT_COMPOUND_TYPE);
  COMPOUND_TYPE_SET_ENV(com,union_env);
  COMPOUND_TYPE_SET_SIZE(com,gen_members(gi,union_env,body,0));

  insert_obj(env,name,com);

  val = add_symbol(val,name);

  return val;
}

static list_t *gen_asm(gen_info_t *gi,env_t *env,list_t *lst){

  string_t code;
  string_t p;
  int len;

#ifdef __DEBUG__
  printf("gen_asm\n");
#endif

  code = (string_t)car(lst);
  p = code;
  p++;

  len = STRLEN(p);
  p += len - 1;
  *p = NULL_CHAR;
  p = code;
  p++;
  EMIT(gi,"%s",p);

  return make_null();
}

static integer_t gen_members(gen_info_t *gi,env_t *env,list_t *lst, int offset){

  string_t name;
  int size;
  symbol_t *sym;

#ifdef __DEBUG__
  printf("gen_members\n");
#endif

  if(IS_NULL_LIST(lst)){
	return offset;
  } else {
	sym = factory_member(gi,env,cdr(car(lst)),offset);
	name = car(car(lst));
	insert_obj(env,name,sym);
	return gen_members(gi,env,cdr(lst),SYMBOL_GET_OFFSET(sym) + SYMBOL_GET_SIZE(sym));
  }
}

static integer_t get_operand_size(list_t *lst){

#ifdef __DEBUG__
  printf("get_operand_size\n");
#endif

  if(IS_INTEGER(lst)){
	return *(integer_t *)car(lst);
  }

  if(is_struct_ref(lst)){
	return *(integer_t *)car(car(cdr(lst)));
  }

  if(is_value(lst)){
	return *(integer_t *)car(car(lst));
  }

  if(is_array(lst)){
	return *(integer_t *)car(cdr(lst));
  }

  printf("error\n");
  DUMP_AST(lst);
  exit(1);
}

static integer_t calc_mem_offset(list_t *lst){

  list_t *p;
  integer_t offset;

  p = lst;
  while(!IS_LIST(p)){
	p = cdr(p);
  }

  offset = *(integer_t *)car(cdr(car(p)));

  return offset;
}

static integer_t calc_mem_size(list_t *lst){

  list_t *p;
  integer_t offset;

  p = lst;
  while(!IS_LIST(p)){
	p = cdr(p);
  }

  offset = *(integer_t *)car(car(p));

  return offset;
}

static int calc_struct_size(gen_info_t *gi,env_t *env,list_t *lst){

  list_t *p;
  int size;

#ifdef __DEBUG__
  printf("calc_struct_size\n");
#endif

  p = lst;
  size = 0;
  while(IS_NOT_NULL_LIST(p)){
	size += select_size(gi,env,cdr(car(lst)),FALSE);
	p = cdr(p);
  }

  return size;
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

static void pop_only_inst(gen_info_t *gi,string_t reg){

  EMIT(gi,"popq #%s",reg);

  return;
}

static void push_xmm(gen_info_t *gi,string_t reg){

  EMIT(gi,"sub $8, #rsp");
  EMIT(gi,"movsd #%s, (#rsp)",reg);
  gi->stack_pos += SIZE;

  return;
}

static void pop_xmm(gen_info_t *gi,string_t reg){

  EMIT(gi,"movsd (#rsp), #%s",reg);
  EMIT(gi,"add $8, #rsp");
  gi->stack_pos -= SIZE;

  return;
}

static list_t *eval_type(gen_info_t *gi,env_t  *env,list_t *lst){

  list_t *l;
  symbol_t *sym;
  string_t name;

#ifdef __DEBUG__
  printf("eval_type\n");
#endif

  DUMP_AST(lst);
  if(IS_LIST(lst)){
	return eval_type(gi,env,car(lst));
  } else {
	name = car(lst);
	if(STRCMP(name,"*")){
	  return lst;
	} else if(STRCMP(name,INT)){
	  return lst;
	} else if(STRCMP(name,CHAR)){
	  return lst;
	} else if(STRCMP(name,SHORT)){
	  return lst;
	} else if(STRCMP(name,LONG)){
	  return lst;
	} else if(STRCMP(name,FLOAT)){
	  return lst;
	} else if(STRCMP(name,DOUBLE)){
	  return lst;
	} else if(STRCMP(name,ARRAY)){
	  return lst;
	} else if(STRCMP(car(lst),STRUCT_ALLOC)){
	  return lst;
	} else if(STRCMP(car(lst),UNION_ALLOC)){
	  return lst;
	} else if(STRCMP(car(lst),STRUCT)){
	  return cdr(lst);
	} else if(STRCMP(car(lst),ENUM)){
	  return cdr(lst);
	} else if(STRCMP(car(lst),STRUCT_DEF)){
	  return gen_struct_def(gi,env,cdr(lst));
	} else if(STRCMP(car(lst),UNION_DEF)){
	  return gen_union(gi,env,cdr(lst));
	} else if(STRCMP(car(lst),UNION)){
	  return cdr(lst);
	} else if(STRCMP(car(lst),UNSIGNED)){
	  return lst;
	} else if(STRCMP(car(lst),SIGNED)){
	  return lst;
	} else if(STRCMP(car(lst),VOID)){
	  return lst;
	} else if(STRCMP(car(lst),INT)){
	  return lst;
	} else if(STRCMP(car(lst),DOUBLE)){
	  return lst;
	} else if(STRCMP(car(lst),SHORT)){
	  return lst;
	} else if(STRCMP(car(lst),LONG)){
	  return lst;
	} else if(STRCMP(car(lst),CHAR)){
	  return lst;
	} else {
	  sym = lookup_obj(env,car(lst));
	  if(!sym){
		error(LIST_GET_SYMBOL_LINE_NO(lst),LIST_GET_SYMBOL_SRC(lst),"undefined variable %s",(string_t)car(lst));
		return make_null();
	  }

	  if(sym->obj.type == TYPE_COMPOUND){
		return lst;
	  } else {
		return eval_type(gi,env,SYMBOL_GET_TYPE_LST(sym));
	  }
	}
  }
}

static integer_t select_size(gen_info_t *gi,env_t  *env,list_t *lst,bool_t flg){

  integer_t size;
  symbol_t *sym;
  compound_def_t *com;
  list_type_t type;

#ifdef __DEBUG__
  printf("select_size\n");
#endif

  type = LIST_GET_TYPE(lst);
  switch(type){
  case NULL_LIST:
	return 1;
	break;
  case LIST:
	return select_size(gi,env,car(lst),flg) * select_size(gi,env,cdr(lst),flg);
	break;
  default:
	if(STRCMP(car(lst),"*")){
	  return SIZE;
	} else if(STRCMP(car(lst),INT)){
	  return sizeof(int);
	} else if(STRCMP(car(lst),CHAR)){
	  return sizeof(char);
	} else if(STRCMP(car(lst),SHORT)){
	  return sizeof(short);
	} else if(STRCMP(car(lst),LONG)){
	  return sizeof(long);
	} else if(STRCMP(car(lst),FLOAT)){
	  return sizeof(float);
	} else if(STRCMP(car(lst),DOUBLE)){
	  return sizeof(double);
	} else if(STRCMP(car(lst),VOID)){
	  return 1;
	} else if(STRCMP(car(lst),SIGNED)){
	  if(IS_NOT_NULL_LIST(lst)){
		return select_size(gi,env,cdr(lst),FALSE);
	  } else {
		return sizeof(signed);
	  }
	} else if(STRCMP(car(lst),UNSIGNED)){
	  if(IS_NOT_NULL_LIST(lst)){
		return select_size(gi,env,cdr(lst),FALSE);
	  } else {
		return sizeof(unsigned);
	  }
	} else if(STRCMP(car(lst),ARRAY)){
	  if(flg){
		return select_size(gi,env,cdr(cdr(lst)),flg);
	  } else {
		size = get_size_of_array(gi,cdr(lst),env);
		return  size * select_size(gi,env,cdr(cdr(lst)),flg);
	  }
	} else if(STRCMP(car(lst),STRUCT_ALLOC)){
	  return select_compound_type(env,cdr(lst));
	} else if(STRCMP(car(lst),UNION_ALLOC)){
	  return select_compound_type(env,cdr(lst));
	} else if(STRCMP(car(lst),STRUCT)){
	  return select_compound_type(env,cdr(lst));
	} else if(STRCMP(car(lst),STRUCT_DEF)){
	  return select_compound_type(env,cdr(lst));
	} else if(STRCMP(car(lst),UNION)){
	  return select_compound_type(env,cdr(lst));
	} else if(STRCMP(car(lst),UNION_DEF)){
	  return select_compound_type(env,cdr(lst));
	} else if(STRCMP(car(lst),ENUM)){
	  return sizeof(int);
	} else {
	  sym = lookup_obj(env,car(lst));
	  if(!sym){
		error(LIST_GET_SYMBOL_LINE_NO(lst),LIST_GET_SYMBOL_SRC(lst),"undefined  type %s ",(string_t)car(lst));
		exit(1);
	  }

	  if(sym->obj.type == TYPE_COMPOUND){
		com = (compound_def_t *)sym;
		return COMPOUND_TYPE_GET_SIZE(com);
	  } else {
		return select_size(gi,env,SYMBOL_GET_TYPE_LST(sym),FALSE);
	  }
	}
	exit(1);
  }
}

static integer_t select_compound_type(env_t *env,list_t *lst){

  string_t name;
  compound_def_t *com;

#ifdef __DEBUG__
  printf("select_compound_type\n");
#endif

  name = car(lst);
  com = lookup_obj(env,name);
  if(!com){
	exit(1);
  }

  return COMPOUND_TYPE_GET_SIZE(com);
}

static string_t select_reg(integer_t size){

  string_t op;
#ifdef __DEBUG__
  printf("select_reg\n");
#endif

  switch(size){
  case 1:
	op = AL;
	break;
  case 2:
	op = AX;
  case 4:
	op = EAX;
	break;
  case 8:
	op = RAX;
	break;
  }

  return op;
}

static string_t select_reg_c(integer_t size){

  string_t op;
#ifdef __DEBUG__
  printf("select_reg\n");
#endif

  switch(size){
  case 1:
	op = CL;
	break;
  case 2:
	op = CX;
  case 4:
	op = ECX;
	break;
  case 8:
	op = RCX;
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

 static string_t select_inst_fp(integer_t size){

   string_t op;
#ifdef __DEBUG__
   printf("select_inst_fp\n");
#endif

   switch(size){
	 case 4:
	   op = "movss";
	 break;
   case 8:
	 op = "movsd";
	 break;
   default:
	 exit(1);
	 break;
   }

   return op;
 }

static string_t select_op(char op){

  string_t inst;
#ifdef __DEBUG__
  printf("select_op\n");
#endif

  switch(op){
  case '+':
	inst = "addq";
	break;
  case '-':
	inst = "subq";
	break;
  case '*':
	inst = "imulq";
	break;
  default:
	printf("op : %c\n",op);
	exit(1);
	break;
  }

  return inst;
}

static string_t select_op_fp(char op){

  string_t inst;

#ifdef __DEBUG__
  printf("select_op_fp\n");
#endif

  switch(op){
  case '+':
	inst = "addsd";
	break;
  case '-':
	inst = "subsd";
	break;
  case '*':
	inst = "mulsd";
	break;
  case '/':
	inst = "divsd";
	break;
  default:
	exit(1);
	break;
  }

  return inst;
}

static string_t select_movs_inst(integer_t size){

  string_t inst;

#ifdef __DEBUG__
  printf("select_movs_inst\n");
#endif

  switch(size){
  case 1:
	inst = "movsbq";
	break;
  case 2:
	inst = "movswq";
	break;
  case 4:
	inst = "movslq";
	break;
  default:
	printf("size : %d\n",size);
	exit(1);
  }

  return inst;
}

static bool_t is_need_padding(gen_info_t *gi){

  if(gen_info_get_stack_pos(gi) % 16 == 0){
	return FALSE;
  }

  return TRUE;
}

static bool_t is_unary(list_t *lst){
  return IS_NULL_LIST(lst);
}

static func_t *make_func(list_t *lst,scope_t scope){

  func_t *func;
#ifdef __DEBUG__
  printf("make_func\n");
#endif

  func = create_func(get_args(lst),get_ret(lst),scope);
  return func;
}

static list_t *gen_typedef(gen_info_t *gi,env_t *env,list_t *lst){

  symbol_t *sym;
  list_t *type_lst;
  list_t *val;
  string_t name;

#ifdef __DEBUG__
  printf("gen_typedef\n");
#endif

  val = make_null();
  name = car(lst);
  sym = create_symbol(eval_type(gi,env,cdr(lst)));
  SYMBOL_SET_TYPE(sym,TYPE_TYPE);

  insert_obj(env,name,sym);

  val = add_symbol(val,name);

  return val;
}

static integer_t get_offset(list_t *lst){

  if(IS_LIST(lst)){
	return *(integer_t *)car(cdr(car(lst)));
  } else {
	return *(integer_t *)car(cdr(lst));

  }
}

static string_t make_label(gen_info_t *gi){

#ifdef __DEBUG__
  printf("make_label\n");
#endif

  char buf[256];
  string_t str;
  sprintf(buf,".L%d",gi->label);
  gi->label++;
  str = mem(strlen(buf) + 1);
  strcpy(str,buf);

  return str;
}

static int get_size_of_array(gen_info_t *gi,list_t *lst,env_t *env){

  int size;
#ifdef __DEBUG__
  printf("get_size_of_array\n");
#endif

  size = calc(gi,lst,env);

  return size;
}

static int calc(gen_info_t *gi,list_t *lst,env_t *env){

  list_type_t type;

#ifdef __DEBUG__
  printf("calc\n");
#endif

  type = LIST_GET_TYPE(lst);
  switch(type){
  case LIST:
	return calc(gi,car(lst),env);
	break;
  case SYMBOL:
	return calc_symbol(gi,lst,env);
	break;
  case INTEGER:
	return calc_integer(gi,lst,env);
	break;
  }

  exit(1);
}

static int calc_symbol(gen_info_t *gi,list_t *lst,env_t *env){

  string_t name;
#ifdef __DEBUG__
  printf("calc_symbol\n");
#endif

  name = (string_t)car(lst);
  if(STRCMP(name,ADD)){
	return calc_add(gi,cdr(lst),env);
  } else if(STRCMP(name,SUB)){
	return calc_sub(gi,cdr(lst),env);
  } else if(STRCMP(name,MUL)){
	return calc_mul(gi,cdr(lst),env);
  } else if(STRCMP(name,DIV)){
	return calc_div(gi,cdr(lst),env);
  } else if(STRCMP(name,MOD)){
	return calc_mod(gi,cdr(lst),env);
  } else if(STRCMP(name,TYPE)){
	return calc_type(gi,cdr(lst),env);
  } else if(STRCMP(name,CAST)){
	return calc(gi,cdr(cdr(lst)),env);
  } else if(STRCMP(name,SIZEOF)){
	return calc_sizeof(gi,cdr(lst),env);
  } else if((STRCMP(name,LESS))
			|| (STRCMP(name,GREATER))
			|| (STRCMP(name,LESS_EQAUL))
			|| (STRCMP(name,GREATER_EQUAL))
			|| (STRCMP(name,EQUAL))
			|| (STRCMP(name,NOT_EQUAL))){
	return calc_bin_cmp_op(gi,lst,env);
  } else if(STRCMP(name,NOT)){
	return calc_not(gi,cdr(lst),env);
  } else if(STRCMP(name,BIT_LEFT_SHIFT)
			|| STRCMP(name,BIT_RIGHT_SHIFT)){
	return calc_bit_shift(gi,lst,env);
  } else if(STRCMP(name,TERNARY)){
	return calc_ternary(gi,cdr(lst),env);
  } else {
	if(gi->eval_type){
	  return calc_load(gi,env,name);
	} else {
	  return select_size(gi,env,lst,FALSE);
	}
  }

  exit(1);
}

static int calc_integer(gen_info_t *gi,list_t *lst,env_t *env){

  int val;
#ifdef __DEBUG__
  printf("calc_integer\n");
#endif

  val = *(int *)car(lst);

  return val;
}

static int calc_add(gen_info_t *gi,list_t *lst,env_t *env){

  int val;
  int left;
  int right;

#ifdef __DEBUG__
  printf("calc_add\n");
#endif

  left = calc(gi,lst,env);
  right = calc(gi,cdr(lst),env);
  val = left + right;

  return val;
}

static int calc_sub(gen_info_t *gi,list_t *lst,env_t *env){

  int val;
  int left;
  int right;

#ifdef __DEBUG__
  printf("calc_sub\n");
#endif

  left = calc(gi,lst,env);
  right = calc(gi,cdr(lst),env);
  val = left - right;

  return val;
}

static int calc_mul(gen_info_t *gi,list_t *lst,env_t *env){

  int val;
  int left;
  int right;

#ifdef __DEBUG__
  printf("calc_mul\n");
#endif

  left = calc(gi,lst,env);
  right = calc(gi,cdr(lst),env);
  val = left * right;

  return val;
}

static int calc_div(gen_info_t *gi,list_t *lst,env_t *env){

  int val;
  int left;
  int right;

#ifdef __DEBUG__
  printf("calc_div\n");
#endif

  left = calc(gi,lst,env);
  right = calc(gi,cdr(lst),env);
  val = left / right;

  return val;
}

static int calc_mod(gen_info_t *gi,list_t *lst,env_t *env){

  int val;
  int left;
  int right;

#ifdef __DEBUG__
  printf("calc_mod\n");
#endif

  left = calc(gi,lst,env);
  right = calc(gi,cdr(lst),env);
  val = left % right;

  return val;
}

static int calc_sizeof(gen_info_t *gi,list_t *lst,env_t *env){

  int val;
#ifdef __DEBUG__
  printf("calc_sizeof\n");
#endif

  val = calc(gi,lst,env);

  return val;
}

static int calc_type(gen_info_t *gi,list_t *lst,env_t *env){

#ifdef __DEBUG__
  printf("calc_type\n");
#endif

  return select_size(gi,env,lst,FALSE);
}

static int calc_ternary(gen_info_t *gi,list_t *lst,env_t *env){

  int cond;
  int left;
  int right;
  int val;

#ifdef __DEBUG__
  printf("calc_ternary\n");
#endif

  cond = calc(gi,car(lst),env);
  left = calc(gi,car(cdr(lst)),env);
  right = calc(gi,car(cdr(cdr(lst))),env);

  val = cond ? left : right;

  return val;
}

static int calc_bit_shift(gen_info_t *gi,list_t *lst,env_t *env){

  string_t op;
  int val;
  int left;
  int right;

#ifdef __DEBUG__
  printf("calc_bit_shift\n");
#endif

  left = calc(gi,cdr(lst),env);
  right = calc(gi,cdr(cdr(lst)),env);
  op = (string_t)car(lst);
  if(STRCMP(op,BIT_LEFT_SHIFT)){
	val = left << right;
  } else if(STRCMP(op,BIT_RIGHT_SHIFT)){
	val = left >> right;
  }

  return val;
}

static int calc_bin_cmp_op(gen_info_t *gi,list_t *lst,env_t *env){

  string_t op;
  int val;
  int left;
  int right;

#ifdef __DEBUG__
  printf("calc_bin_cmp_op\n");
#endif

  left = calc(gi,cdr(lst),env);
  right = calc(gi,cdr(cdr(lst)),env);

  op = (string_t)car(lst);
  if(STRCMP(op,LESS)){
	val = left < right;
  } else if(STRCMP(op,GREATER)){
	val = left > right;
  } else if(STRCMP(op,LESS_EQAUL)){
	val = left <= right;
  } else if(STRCMP(op,GREATER_EQUAL)){
	val = left >= right;
  } else if(STRCMP(op,EQUAL)){
	val = left == right;
  } else if(STRCMP(op,NOT_EQUAL)){
	val = left != right;
  }

  return val;
}

static int calc_not(gen_info_t *gi,list_t *lst,env_t *env){

  int val;
#ifdef __DEBUG__
  printf("calc_not\n");
#endif

  val = calc(gi,lst,env);

  return !val;
}

static int calc_load(gen_info_t *gi,env_t *env,string_t name){

  int val;
  object_t *obj;
  enumdef_t *enumdef;

#ifdef __DEBUG__
  printf("calc_load\n");
#endif

  obj = lookup_obj(env,name);
  printf("name : %s",name);
  if(!obj){
	exit(1);
  }

  switch(OBJ_GET_SCOPE(obj)){
  case ENUMLATE:
	enumdef = (enumdef_t *)obj;
	val = ENUMDEF_GET_VAL(enumdef);
	break;
  default:
	printf("name : %s\n",name);
	exit(1);
	break;
  }

  return val;
}

static int align(int localarea){

  int size;
#ifdef __DEBUG__
  printf("align\n");
#endif

  if(localarea % 16 == 0){
	size = localarea;
  } else {
	size = (localarea / 16) * 16 + 16;
  }

  return size;
}

static int calc_mem_alignment(int offset,int size){

  int r;

#ifdef __DEBUG__
  printf("calc_mem_alignment\n");
#endif

  r = offset % size;
  if(r == 0){
	return 0;
  }

  return size - r;
}

static bool_t is_float_sym(list_t *lst){

  if(length_of_list(lst) != 3){
	return FALSE;
  }

  if(TYPE_FLOAT == get_type(lst)){
	return TRUE;
  }

  return FALSE;
}

static void conv_ftoi(gen_info_t *gi,list_t *lst){

  type_t type;
#ifdef __DEBUG__
  printf("conv_ftoi\n");
#endif

  type = *(type_t *)car(lst);
  if(type == TYPE_INT){
	EMIT(gi,"cvtsi2sd #rax,#xmm0");
  }

  return;
}

static bool_t is_array_type(list_t *lst){

  bool_t flg;
  type_t type;

  if(!IS_INTEGER(lst)){
	return FALSE;
  }

  type = *(integer_t *)car(lst);
  if(type != TYPE_ARRAY){
	return FALSE;
  }

  return TRUE;
}

static int calc_sizeof_dimension(list_t *lst){

  string_t name;

  if(IS_NULL_LIST(lst)){
	return 1;
  } else if(IS_SYMBOL(lst)){
	name = car(lst);
	if(!STRCMP(name,ARRAY)){
	  return 1;
	} else {
	  return *(int *)car(car(cdr(lst))) * calc_sizeof_dimension(cdr(cdr(lst)));
	}
  }
  exit(1);
}
