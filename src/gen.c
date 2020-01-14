// 
//
//  Copyright 2019 Shota Hisai. Released under the MIT license.
//
//
#include <stdarg.h>
#include "common.h"
#include "gen.h"
#include "mem.h"
#include "util.h"
#include "inst.inc"
#include "util.h"
#include "symbol.h"
#include "compound_type.h"

#ifdef __DEBUG__
#include "dump.h"
#endif

#define SAVED_REGISTER_SIZE 176

static int TAB = 8;

char *regs[] = {"rdi","rsi","rdx","rcx","r4","r9"};
#define SIZE sizeof(void *)
#define EMIT_NO_INDENT(ei,...) emitf(ei,0x00,__VA_ARGS__)
#define EMIT(ei,...)        emitf(ei,__LINE__, "\t" __VA_ARGS__)
#define IS_TYPE(l)       is_type(l)
#define CALC_OFF(i) -((i + 1 )  * SIZE )
#define GET_OUTPUT_FILE(ei)        FILE_GET_FP(ei->output_file)
#define ENABLE_ASSIGN_FLAG(ei)     ei->assing_flag = TRUE
#define DISENABLE_ASSIGN_FLAG(ei)  ei->assing_flag = FALSE
#define IS_ASSIGN_FLAG(ei)         ei->assing_flag
#define ENABLE_ARRAY_FLAG(ei)      ei->array_assing = TRUE
#define DISENABLE_ARRAY_FLAG(ei)   ei->array_assing = FALSE
#define IS_TO_ENABLE_ASSIGN_FLAG(ei) ei->assing_flag
#define ARRAY_FLAG(ei)             ei->array_assing
#define SET_STACK_POS(ei,s)       ei->stack_pos = s
#define GET_STACK_POS(ei,s)       ei->stack_pos
#define INCREMENT_STACK(ei,s)       ei->stack_pos += s
#define DECREMENT_STACK(ei,s)       ei->stack_pos -= s
#define CALC_PASSING_OF_STACK_POS(ei,d) ei->stack_pos % d

#define INIT_ENUM_VALUE(ei)  ei->enum_value = 0
#define SET_ENUM_VALUE(ei,v) ei->enum_value = v
#define GET_ENUM_VALUE(ei)   ei->enum_value
#define INCREMENT_ENUM_VALUE(ei) ei->enum_value++

#define ENUM_NAME(l) cdr(l)
#define ENUM_LIST(l) cdr(cdr(l))

static list_t *get_enum_obj(list_t *lst);
static list_t *get_args(list_t *lst);
static list_t *get_local_vars(list_t *lst);

static void gen_info_set_offset(gen_info_t *ei,int size);
static int  gen_info_get_offset(gen_info_t *ei);

static list_t *gen_struct(gen_info_t *ei,env_t *env,list_t *lst);
static void gen_union(gen_info_t *ei,env_t *env,list_t *lst);
static list_t *gen_members_of_struct(gen_info_t *ei,env_t *env,list_t *lst);
static list_t *gen_member_of_struct(gen_info_t *ei,env_t *env,list_t *lst);
static void gen_function(gen_info_t *ei,env_t *env,list_t *function);
static void gen_func_decl(gen_info_t *ei,env_t *env,list_t *lst);
static void gen_func_parms(gen_info_t *ei,env_t *env,list_t *lst);
static list_t *gen_body(gen_info_t *ei,env_t *env,list_t *lst);
static void gen_operand(gen_info_t *ei,env_t *env,list_t *lst);
static void  gen_string(gen_info_t *ei,env_t *env,list_t *lst);
static list_t *gen_symbol(gen_info_t *ei,env_t *env,list_t *lst);
static void gen_return(gen_info_t *ei,env_t *env,list_t *lst);
static void gen_func_call(gen_info_t *ei,env_t *env,list_t *lst);
static void gen_func_pointer_call(gen_info_t *ei,env_t *env,list_t *lst,symbol_t *sym);
static void gen_enum(gen_info_t *ei,env_t *env,list_t *lst);
static int  gen_func_call_args(gen_info_t *ei,env_t *env,list_t *lst);
static void gen_index_of_array(gen_info_t *ei,env_t *env,list_t *lst);
static void gen_assign(gen_info_t *ei,env_t *env,list_t *lst);
static list_t *gen_increment_assign(gen_info_t *ei,env_t *env,list_t *lst);
static list_t *gen_increment(gen_info_t *ei,env_t *env,list_t *lst);
static list_t *gen_bin_cmp_op(gen_info_t *ei,env_t *env,list_t *lst);
static list_t *gen_bin_op(gen_info_t *ei,env_t *env,list_t *lst);
static void gen_bin_op_int(gen_info_t *ei,env_t *env,list_t *lst);
static void gen_bin_op_float(gen_info_t *ei,env_t *env,list_t *lst);
static list_t *gen_bit_shift_op(gen_info_t *ei,env_t *env,list_t *lst);
static list_t *gen_bit_op(gen_info_t *ei,env_t *env,list_t *lst);
static list_t *gen_defvar(gen_info_t *ei,env_t *env,list_t *lst);
static list_t *gen_logical_not(gen_info_t *ei,env_t *env,list_t *lst);
static list_t *gen_not(gen_info_t *ei,env_t *env,list_t *lst);
static list_t *gen_sizeof(gen_info_t *ei,env_t *env,list_t *lst);
static int gen_sizeoftype(gen_info_t *ei,env_t *env,list_t *lst);
static void gen_arg(gen_info_t *ei,env_t *env,list_t *lst);
static void gen_struct_alloc(gen_info_t *ei,env_t *env,list_t *lst);
static void gen_struct_alloc_param(gen_info_t *ei,env_t *env,list_t *lst,int index);
static void gen_union_alloc(gen_info_t *ei,env_t *env,list_t *lst);
static void gen_struct_ref_param(gen_info_t *ei,env_t *env,list_t *lst,int index,symbol_t *sym);
static int calc_local_area(gen_info_t *ei,env_t *env,list_t  *lst);
static void gen_if(gen_info_t *ei,env_t *env,list_t *lst);
static list_t *gen_integer(gen_info_t *ei,list_t *lst);
static void gen_float(gen_info_t *ei,list_t *lst);
static void gen_cmp(gen_info_t *ei);
static void gen_je(gen_info_t *ei,char *label);
static void gen_jmp(gen_info_t *ei,char *label);
static void gen_label(gen_info_t *ei,char *label);
static void gen_load_convert(gen_info_t *ei,env_t *env,list_t *lst);
static void gen_deref(gen_info_t *ei,env_t *env,list_t *lst);
static list_t *gen_cast(gen_info_t *ei,env_t *env,list_t *lst);
static void gen_cast_int(gen_info_t *ei,type_t src_type);
static void gen_cast_long(gen_info_t *ei,type_t src_type);
static int align(env_t *env,list_t *lst);

static char *gen_cmp_inst(list_t *lst);
static void gen_arg_regs(gen_info_t *ei,list_t *insts,list_t *floats);
static list_t *lookup_member(list_t *lst,char *name);
static void gen_arg_regs_after_call(gen_info_t *ei,list_t *ints, list_t *floats);
static char *get_type(list_t *lst);
static list_t *get_name(list_t *lst);

static void gen_ret(gen_info_t *ei);
static void emitf(gen_info_t *ei,int line, char *fmt, ...);
static void push(gen_info_t *ei,char *reg);
static void pop(gen_info_t *ei,char *reg);
static void push_xmm(gen_info_t *ei,int reg);
static void pop_xmm(gen_info_t *ei,int reg);
static int push_struct(gen_info_t *ei,int size);
static void pop_ints_args(gen_info_t *ei,list_t *ints);
static void pop_floats_args(gen_info_t *ei,list_t *floats);
static int is_type(list_t *lst);
static int get_size_of_param(env_t *env,list_t *lst);
static int calc_size_of_struct(list_t *lst);

static list_t *gen_lvar(gen_info_t *ei,env_t *env,list_t *lst);
static void gen_load(gen_info_t *ei,char *base,env_t *env,list_t *lst,symbol_t *symbol);
static void gen_load_func(gen_info_t *ei,env_t *env,list_t *lst);
static void gen_gload(gen_info_t *ei,env_t *env,list_t *lst,symbol_t *sym);
static char *get_load_inst(gen_info_t *ei,env_t *env,list_t *lst,symbol_t *symbol);
static void gen_save(gen_info_t *ei,env_t *env,list_t *lst);
static void gen_gsave(gen_info_t *ei,env_t *env,list_t *name,symbol_t *sym);
static void gen_lsave(gen_info_t *ei,env_t *env,symbol_t *sym);
static char *get_int_reg(int size);
static void gen_while(gen_info_t *ei,env_t *env,list_t *lst);
static void gen_for(gen_info_t *ei,env_t *env,list_t *lst);
static list_t *gen_array(gen_info_t *ei,env_t *env,list_t *lst);
static void gen_index_array(gen_info_t *ei,env_t *env,list_t *lst);
static void gen_typedef(gen_info_t *ei,env_t *env,list_t *lst);
static list_t *gen_type_of_typedef(gen_info_t *ei,env_t *env,list_t *lst);

static list_t *gen_access_member(gen_info_t *ei,env_t *env,list_t *lst);
static list_t *gen_ref_member(gen_info_t *ei,env_t *env,list_t *lst);
static void gen_continue(gen_info_t *ei,env_t *env,list_t *lst);
static int get_size_of_type(list_t *lst);
static int calc_align_size(list_t *lst);
static int calc_length_of_array(list_t *lst);
static char *choose_increment_op(list_t *lst);
static void gen_struct_args(gen_info_t *ei,env_t *env,int index,symbol_t *sym);

static void gen_enum_elements(gen_info_t *ei,env_t *env,list_t *lst);
static void gen_enum_element(gen_info_t *ei,env_t *env,list_t *lst);
static void gen_enum_value(gen_info_t *ei,env_t *env,list_t *lst);

static list_t* classify_args(list_t *args,env_t *env);
static list_t *lookup_primitive_symbol(env_t *env,string_t name);
static int check_primitive_symbol(symbol_t *sym);
static int gen_regsave(gen_info_t *ei);
static void gen_builtin_function(list_t *lst);
static void gen_builtin_va_start(list_t *lst);
static list_t *calc_type(list_t *lst);
static string_t emit_no_name_label(gen_info_t *ei);

gen_info_t *create_gen_info(){
  
  gen_info_t *ei;

  ei = mem(sizeof(gen_info_t));
  ei->stack_pos = 0;
  ei->off_set = 0;
  ei->off_set_index = 0;
  ei->assing_flag = FALSE;
  ei->array_assing = FALSE;
  ei->off_set_struct = 0;
  ei->no_name_label_no = 0;
  
  return ei;
}

void init_gen_info(gen_info_t *ei){
  
  ei->stack_pos = 0;
  ei->off_set = 0;
  ei->off_set_index = 0;
  ei->assing_flag = FALSE;
  ei->array_assing = FALSE;
  ei->off_set_struct = 0;

  return;
}

list_t *gen(gen_info_t *ei,env_t *env,list_t *lst,bool_t recursive_flag){
  
  list_t *val;
  #ifdef __DEBUG__
  printf("gen\n");
  #endif
  
  init_gen_info(ei);
  SET_STACK_POS(ei,8);
  
  val = make_null();
  if(IS_NOT_NULL_LIST(lst)){
    if(IS_INTEGER(((list_t *)car(lst)))){
      if(is_typdef(car(lst))){
	gen_typedef(ei,env,cdr(car(lst)));
      }
    } else {
      char *symbol = (char *)car(car(lst));
      if(STRCMP(symbol,STRUCT_DEF)){
	val = gen_struct(ei,env,cdr(car(lst)));
      } else if(STRCMP(symbol,UNION_DEF)){
	gen_union(ei,env,cdr(car(lst)));
      } else if(STRCMP(symbol,ENUM)){
	gen_enum(ei,env,lst);
      } else if(STRCMP(symbol,FUNC_DEF)){
	list_t *function = cdr(car(lst));
	env_t *new_env = extend_env(env);
	list_t *body;
	list_t *name;
	list_t *func;
      
	if(is_static(function) || is_extern(function)){
	  function = cdr(function);
	  body = cdr(cdr(cdr(function)));
	} else {
	  body = cdr(cdr(cdr(function)));
	}
      
	if(IS_LIST(cdr(function))){
	  name = cdr(cdr(function));
	  func = cdr(function);
	} else {
	  name = cdr(function);
	  func = function;
	}
      
	if(IS_NULL_LIST(lookup(env,name))){
	  gen_func_decl(ei,env,func);
	}
	
	gen_function(ei,new_env,function);
	gen_body(ei,new_env,car(body));
	gen_ret(ei);
      } else if(STRCMP(symbol,FUNC_DECL)){
	gen_func_decl(ei,env,cdr(car(lst)));
      } else {
	printf("Undefined behavior\n");
	exit(1);
      }
    }
    
    if(recursive_flag){
      val = gen(ei,env,cdr(lst),recursive_flag);
    }
  }

  return val;
}

void delete_gen_info(gen_info_t *ei){

  fre(ei);

  return;
}

static list_t *get_args(list_t *lst){
  return car(cdr(cdr(lst)));
}

static list_t *get_local_vars(list_t *lst){
  return car(cdr(cdr(cdr(cdr(lst)))));
}

static void gen_info_set_offset(gen_info_t *ei,int size){

  ei->off_set = size;
  
  return;
}

static int gen_info_get_offset(gen_info_t *ei){
  return ei->off_set;
}

static list_t *gen_struct(gen_info_t *ei,env_t *env,list_t *lst){

#ifdef __DEBUG__
  printf("gen_struct\n");
#endif

  compound_def_t *compound_def;
  list_t *body;
  list_t *members;
  list_t *val;
  string_t name;
  int size;
  
  val = make_null();
  body = car(cdr(lst));
  
  name = car(lst);
  if(STRCMP(name,NO_NAME)){
    name = emit_no_name_label(ei);
  }
  
  compound_def = create_compound_type_def();
  members = gen_members_of_struct(ei,env,body);
  size = calc_size_of_struct(members);
  
  COMPOUND_TYPE_SET_TYPE(compound_def,STRUCT_COMPOUND_TYPE);
  COMPOUND_TYPE_SET_MEMBERS(compound_def,members);
  COMPOUND_TYPE_SET_SIZE(compound_def,size);

  insert_obj(env,name,compound_def);
  val = add_symbol(val,STRUCT);
  val = add_symbol(val,name);
  
  return val;
}

static void gen_union(gen_info_t *ei,env_t *env,list_t *lst){
  
#ifdef __DEBUG__
  printf("gen_union\n");
#endif
  
  compound_def_t *compound_def;
  list_t *body;
  list_t *members;
  int size;
  
  body = car(cdr(lst));
  
  compound_def = create_compound_type_def();
  members = gen_members_of_struct(ei,env,body);
  size = calc_size_of_struct(members);

  COMPOUND_TYPE_SET_TYPE(compound_def,UNION_COMPOUND_TYPE);
  COMPOUND_TYPE_SET_MEMBERS(compound_def,members);
  COMPOUND_TYPE_SET_SIZE(compound_def,size);

  insert_obj(env,car(lst),compound_def);
  
  return;
}

static list_t *gen_members_of_struct(gen_info_t *ei,env_t *env,list_t *lst){

#ifdef __DEBUG__
  printf("gen_members_of_struct\n");
#endif

  if(IS_NOT_NULL_LIST(lst)){
    list_t *member_lst = gen_member_of_struct(ei,env,car(lst));
    return concat(member_lst,gen_members_of_struct(ei,env,cdr(lst)));
  } else {
    ei->off_set_struct = 0;
    return make_null();
  }
}

static list_t *gen_member_of_struct(gen_info_t *ei,env_t *env,list_t *lst){
  
  list_t *member_lst;
  int size;
#ifdef __DEBUG__
  printf("gen_member_of_struct\n");
#endif
  
  size = get_size_of_type(car(car(lst)));
  member_lst = add_symbol(make_null(),car(cdr(car(lst))));
  member_lst = concat(member_lst,add_number(make_null(),get_size_of_type(lst)));
  member_lst = concat(member_lst,add_number(make_null(),ei->off_set_struct));
  ei->off_set_struct += size;
  
  return add_list(make_null(),member_lst);
}

static void gen_function(gen_info_t *ei,env_t *env,list_t *function){

  list_t *name;
  list_t *args;
  int localarea = 0;
#ifdef __DEBUG__
  printf("gen_function\n");
#endif
  EMIT(ei,".text");
  if(IS_LIST(cdr(function))){
    name = cdr(cdr(function));
  } else {
    name = cdr(function);
  }

#ifdef __LINUX__
  EMIT(ei,".global %s",(string_t)car(name));
  EMIT_NO_INDENT(ei,"%s:",(string_t)car(name));
#else
  EMIT(ei,".global _%s",(string_t)car(name));
  EMIT_NO_INDENT(ei,"_%s:",(string_t)car(name));
#endif
  EMIT(ei,"nop");
  push(ei,"rbp");
  EMIT(ei,"mov #rsp, #rbp");

  args = get_args(function);
  if(IS_NOT_NULL_LIST(args)){
    gen_func_parms(ei,env,args);
  }
  
  list_t *local = get_local_vars(function);
  localarea = calc_local_area(ei,env,local);
  
  if(localarea){
    EMIT(ei,"sub $%d,#rsp",localarea);
    ei->stack_pos += localarea;
  }
  
  return;
}

static void gen_func_decl(gen_info_t *ei,env_t *env,list_t *lst){
  
  string_t name;
  symbol_t *sym;
  
#ifdef __DEBUG__
  printf("gen_func_decl\n");
#endif
  
  sym = create_symbol(sym);
  SYMBOL_SET_VAR_TYPE(sym,OTHER_VARIABLE);
  SYMBOL_SET_TYPE(sym,TYPE_FUNCTION);
  SYMBOL_SET_KIND(sym,KIND_FUNCTION);
  
  name = car(cdr(lst));
  insert_obj(env,name,sym);
  
  return;
}
 
static int calc_local_area(gen_info_t *ei,env_t *env,list_t *lst){
  
  list_t *lal;
  list_t *sub_lst;
  int localarea = 0;
  int size;

#ifdef __DEBUG__
  printf("calc_local_area\n");
#endif
  
  lal = lst;
  while(IS_NOT_NULL_LIST(lal)){
    list_t *p = car(cdr(car(lal)));
    if(IS_LIST(p)){
      p = car(p);
    } else {
      p = car(cdr(p));
    }
    
    size = align(env,p);
    lal = cdr(lal);
    localarea += size;
  }
  
  return localarea;
}

static void gen_func_parms(gen_info_t *ei,env_t *env,list_t *lst){

  list_t *p;
  list_t *tail_lst;
  int i = 0;
  int offset = 0;
  type_t type;
  list_t *name;
  symbol_t *sym;

#ifdef __DEBUG__
  printf("gen_func_parms\n");
#endif  

  for(p = lst,i = 0; IS_NOT_NULL_LIST(p); i++,p = cdr(p)){
    if(STRCMP((string_t)car(p),"...")){
      break;
    }

    list_t *type_lst;
    list_t *q = car(p);
    if(IS_SYMBOL(q)){
      if(STRCMP(car(q),CONST)){
	q = cdr(q);
      }
    }

    type_lst = car(q);
    int size = get_size_of_param(env,q);
    offset -= size;
    sym = create_symbol();
    SYMBOL_SET_OFFSET(sym,offset);
    SYMBOL_SET_VAR_TYPE(sym,LOCAL_VARIABLE);
    SYMBOL_SET_SIZE(sym,size);
    if(is_structtype(tail(type_lst))){
      if(is_pointer(type_lst)){
	gen_struct_ref_param(ei,env,q,i,sym);
      } else {
	gen_struct_alloc_param(ei,env,q,i);
      }
    } else {
      name = cdr(q);
      type = gen_type(type_lst);
      SYMBOL_SET_VAR_TYPE(sym,LOCAL_VARIABLE);
      SYMBOL_SET_TYPE(sym,type);
      SYMBOL_SET_TYPE_LST(sym,car(car(q)));
      SYMBOL_SET_KIND(sym,select_type(SYMBOL_GET_TYPE_LST(sym)));
      push(ei,regs[i]);
      insert_obj(env,car(name),sym);
    }
  }
  
  ei->off_set_index = i;
  ei->off_set = offset;
  
  return;
}

static list_t *gen_body(gen_info_t *ei,env_t *env,list_t *lst){

  list_t *val;

#ifdef __DEBUG__
  printf("gen_body\n");
#endif
  
  val = make_null();
  switch(LIST_GET_TYPE(lst)){
  case LIST:
    val = gen_body(ei,env,car(lst));
    if(IS_NOT_NULL_LIST(cdr(lst))){
      val = gen_body(ei,env,cdr(lst));
    }
    break;
  case INTEGER:
    val = gen_integer(ei,lst);
    break;
  case SYMBOL:
    val = gen_symbol(ei,env,lst);
    break;
  case DECIMAL:
    gen_float(ei,lst);
    break;
  case NULL_LIST:
    return val;
    break;
  case STRING:
    gen_string(ei,env,lst);
    break;
  default:
    break;
  }
  
  return val;
}

static void gen_operand(gen_info_t *ei,env_t *env,list_t *lst){

  switch(LIST_GET_TYPE(lst)){
  case LIST:
    gen_operand(ei,env,car(lst));
    break;
  case INTEGER:
    gen_integer(ei,lst);
    break;
  case SYMBOL:
    gen_symbol(ei,env,lst);
    break;
  case NULL_LIST:
    return;
    break;
  case STRING:
    gen_string(ei,env,lst);
    break;
  default:
    break;
  }

  return;
}

static void gen_string(gen_info_t *ei,env_t *env,list_t *lst){
  
  char *l;
  EMIT(ei,".data");
  l = make_label();
  gen_label(ei,l);
  EMIT(ei,".string %s",(char *)car(lst));
  EMIT(ei,".text");
  EMIT(ei,"lea %s(#rip), #rax",l);

  return;
}

static list_t *gen_symbol(gen_info_t *ei,env_t *env,list_t *lst){

#ifdef __DEBUG__
  printf("gen_symbol\n");
#endif
  
  list_t *val;
  
  val = make_null();
  if(STRCMP(car(lst),RETURN)){
    gen_return(ei,env,cdr(lst));
  } else if(STRCMP(car(lst),ENUM)){
    val = gen_defvar(ei,env,cdr(lst));
  } else if(STRCMP(car(lst),ASSIGN)){
    gen_assign(ei,env,lst);
  } else if((STRCMP(car(lst),INCREMNT_ASSING))
	    || (STRCMP(car(lst),DECREMNT_ASSING))){
    val = gen_increment_assign(ei,env,lst);
  } else if((STRCMP(car(lst),INCREMENT))
	    || (STRCMP(car(lst),DECREMENT))){
    gen_increment(ei,env,lst);
  } else if(STRCMP(car(lst),IF)){
    gen_if(ei,env,lst);
  } else if(STRCMP(car(lst),WHILE)){
    gen_while(ei,env,lst);
  } else if(STRCMP(car(lst),FOR)){
    gen_for(ei,env,lst);
  } else if((STRCMP(car(lst),ADD))
	    || (STRCMP(car(lst),SUB))
	    || (STRCMP(car(lst),MUL))){
    val = gen_bin_op(ei,env,lst);
  } else if((STRCMP(car(lst),LESS))
	    || (STRCMP(car(lst),GREATER))){
    val = gen_bin_cmp_op(ei,env,lst);
  } else if((STRCMP(car(lst),LESS_EQAUL))
	    || (STRCMP(car(lst),GREATER_EQUAL))){
    val = gen_bin_cmp_op(ei,env,lst);
  } else if((STRCMP(car(lst),ARRAY))){
    val = gen_array(ei,env,lst);
  } else if((STRCMP(car(lst),FUNC_CALL))){
    gen_func_call(ei,env,cdr(lst));
  } else if(STRCMP(car(lst),DOT)){
    val = gen_access_member(ei,env,cdr(lst));
  } else if(STRCMP(car(lst),REF_MEMBER_ACCESS)){
    val = gen_ref_member(ei,env,cdr(lst));
  } else if(STRCMP(car(lst),CONTINUTE)){
    gen_continue(ei,env,cdr(lst));
  } else if(LIST_GET_SYMBOL_KIND(lst) == KIND_ENUM){
    gen_enum_value(ei,env,lst);
  } else if(STRCMP(car(lst),DECL_VAR)){
    val = gen_defvar(ei,env,car(cdr(lst)));
  } else if(STRCMP(car(lst),CAST)){
    val = gen_cast(ei,env,cdr(lst));
  } else if(STRCMP(car(lst),BIT_LEFT_SHIFT)
	    || STRCMP(car(lst),BIT_RIGHT_SHIFT)){
    val = gen_bit_shift_op(ei,env,lst);
  } else if((STRCMP((string_t)car(lst),AND))
	    || (STRCMP((string_t)car(lst),OR))
	    || (STRCMP((string_t)car(lst),XOR))){
    val = gen_bit_op(ei,env,lst);
  } else if(STRCMP((string_t)car(lst),NOT)){
    val = gen_logical_not(ei,env,lst);
  } else if(STRCMP(car(lst),BIT_REVERSAL)){
    val = gen_not(ei,env,lst);
  } else if(STRCMP(car(lst),SIZEOF)){
    val = gen_sizeof(ei,env,cdr(lst));
  } else {
    if(IS_ASSIGN_FLAG(ei)){
      val = lst;
    } else {
      val = gen_lvar(ei,env,lst);
    }
  }
  
  return val;
}

static void gen_assign(gen_info_t *ei,env_t *env,list_t *lst){

  list_t *var;

  #ifdef __DEBUG__
  printf("gen_assign\n");
  #endif

  gen_body(ei,env,cdr(cdr(lst)));
  if(IS_LIST(cdr(lst))){
    ENABLE_ASSIGN_FLAG(ei);
    push(ei,"rax");
    var = gen_body(ei,env,car(cdr(lst)));
    if(IS_NOT_NULL_LIST(var)){ 
      pop(ei,"rax");
    }
    DISENABLE_ASSIGN_FLAG(ei);
  } else {
    var = cdr(lst);
  }
  
  if(IS_NOT_NULL_LIST(var)){ 
    gen_load_convert(ei,env,var);
    gen_save(ei,env,var);
  }

  return;
}

static list_t *gen_increment(gen_info_t *ei,env_t *env,list_t *lst){
  
  char *op;
  list_t *obj;
  int offset;
  symbol_t *sym;
  
  op = choose_increment_op(lst);
  gen_operand(ei,env,cdr(lst));
  push(ei,"rax");
  EMIT(ei,"mov $%d, #rax",1);
  pop(ei,"rcx");
  if((STRCMP(car(lst),DECREMENT))
     || (STRCMP(car(lst),DECREMNT_ASSING))){
    EMIT(ei,"%s #rax, #rcx",op);
    EMIT(ei,"mov #rcx, #rax");
  } else {
    EMIT(ei,"%s #rcx, #rax",op);
  }
  
  sym = lookup_obj(env,car(cdr(lst)));
  
  offset = SYMBOL_GET_OFFSET(sym);
  EMIT(ei,"mov #rax, %d(#rbp)",offset);
  
  return make_null();
}

static void gen_enum_elements(gen_info_t *ei,env_t *env,list_t *lst){

  if(IS_NOT_NULL_LIST(lst)){
    gen_enum_element(ei,env,lst);
    gen_enum_elements(ei,env,cdr(lst));
  }
  
  return;
}

static void gen_enum_element(gen_info_t *ei,env_t *env,list_t *lst){

  if(IS_LIST(lst)){
    
  } else if(IS_SYMBOL(lst)){
    insert(env,add_symbol(make_null(),(string_t )car(lst)),add_number(make_null(),GET_ENUM_VALUE(ei)));
    INCREMENT_ENUM_VALUE(ei);
  }
}

static void gen_enum_value(gen_info_t *ei,env_t *env,list_t *lst){

  list_t *val;

  val = lookup(env,lst);
  if(IS_NULL_LIST(val)){
    return;
  }
  
  EMIT(ei,"mov $%u, #rax",*(int *)car(val));
  
  return;
}

static void gen_struct_args(gen_info_t *ei,env_t *env,int index,symbol_t *sym){
  
  int addr;
  
  addr = SYMBOL_GET_OFFSET(sym);
  ei->off_set -= SIZE;
  ei->off_set_index++;
  
  EMIT(ei,"leaq %d(#rbp), #rax",addr);
  EMIT(ei,"movq #rax,%d(#rbp)",ei->off_set);
  EMIT(ei,"movq (#rax), #%s",regs[index]);
  
  return;
 }

static list_t *gen_increment_assign(gen_info_t *ei,env_t *env,list_t *lst){
  
  char *op;
  symbol_t *sym;
  int offset;

  gen_operand(ei,env,cdr(lst));
  gen_body(ei,env,cdr(lst));
  push(ei,"rax");
  gen_operand(ei,env,cdr(cdr(lst)));
  pop(ei,"rcx");

  op = choose_increment_op(lst);
  if((STRCMP(car(lst),DECREMENT))
     || (STRCMP(car(lst),DECREMNT_ASSING))){
    EMIT(ei,"%s #rax, #rcx",op);
    EMIT(ei,"mov #rcx, #rax");
  } else {
    EMIT(ei,"%s #rcx, #rax",op);
  }
  
  sym = sym = lookup_obj(env,car(cdr(lst)));
  
  offset = SYMBOL_GET_OFFSET(sym);
  EMIT(ei,"mov #rax, %d(#rbp)",offset);
  
  return make_null();
}



static void gen_return(gen_info_t *ei,env_t *env,list_t *lst){
  
  gen_body(ei,env,lst);
  
  EMIT(ei,"cltq");
  EMIT(ei,"leave");
  EMIT(ei,"ret");
  
  return;
}

static void gen_func_call(gen_info_t *ei,env_t *env,list_t *lst){

  list_t *args;
  int pos;
  int len = 0;
  int padding = 0;
  list_t *l;
  list_t *floats;
  list_t *ints;
  list_t *rests;
  list_t *obj;
  string_t name;
  symbol_t *sym;
  
  args = car(cdr(lst));
  l = classify_args(args,env);
  ints = car(l);
  floats = car(cdr(l));
  rests  = car(cdr(cdr(l)));
  gen_arg_regs(ei,ints,floats);

  padding = CALC_PASSING_OF_STACK_POS(ei,16);
  if(padding){
    EMIT(ei,"sub $8, #rsp");
    INCREMENT_STACK(ei,8);
  }
  
  int rest_size = gen_func_call_args(ei,env,rests);
  
  gen_func_call_args(ei,env,ints);
  gen_func_call_args(ei,env,floats);

  pop_floats_args(ei,floats);
  pop_ints_args(ei,ints);

  name = car(lst);
  sym = lookup_obj(env,name);
  
  if(sym){
    switch(SYMBOL_GET_KIND(sym)){
    case KIND_POINTER:
      gen_func_pointer_call(ei,env,lst,sym);
      break;
    case KIND_FUNCTION:
#ifdef __LINUX__
      EMIT(ei,"call %s",name);
#else 
      EMIT(ei,"call _%s",name);
#endif
      break;
    default:
      exit(1);
      break;
    }
  } else {
    #ifdef __LINUX__
    EMIT(ei,"call %s",name);
#else 
    EMIT(ei,"call _%s",name);
#endif
  }
  
  if(rest_size >0){
    EMIT(ei,"add $%d,#rsp",rest_size);
    DECREMENT_STACK(ei,rest_size);
  }

  if(padding){
    EMIT(ei,"add $8, #rsp");
    DECREMENT_STACK(ei,8);
  }
  
  gen_arg_regs_after_call(ei,ints,floats);
  
  return;
}
  
static void gen_func_pointer_call(gen_info_t *ei,env_t *env,list_t *lst,symbol_t *sym){
  
  EMIT(ei,"movq %d(#rbp), #rdx",SYMBOL_GET_OFFSET(sym));
  EMIT(ei,"call *#rdx");
  
  return;
}

static void gen_enum(gen_info_t *ei,env_t *env,list_t *lst){

  list_t *enum_lst;
  list_t *new_enum_lst;

  enum_lst = ENUM_LIST(car(lst));
  INIT_ENUM_VALUE(ei);
  
  gen_enum_elements(ei,env,(list_t *)car(enum_lst));
  INIT_ENUM_VALUE(ei);
  
  return;
}

static int gen_func_call_args(gen_info_t *ei,env_t *env,list_t *lst){

  list_t *p;
  int r;
  int i;
  symbol_t *sym;
  
  r = 0;
  if(IS_NULL_LIST(lst)){
    return 0;
  }
  
  for(p = lst,i = 0; IS_NOT_NULL_LIST(p); p = cdr(p),i++){
    sym = NULL;
    if(IS_SYMBOL(p)){
      sym = lookup_obj(env,car(p));
    }
    
    if(is_struct(sym) && is_variable_kind(sym)){
      gen_struct_args(ei,env,i,sym);
    } else if(is_float(sym) || IS_DECIMAL(p)){
      gen_body(ei,env,p);
      EMIT(ei,"cvtps2pd #xmm0, #xmm0");
      push_xmm(ei,0);
      r += SIZE;
    } else {
      gen_body(ei,env,p);
      push(ei,"rax");
      r += SIZE;
    }
  }

  return r;
}

static void pop_ints_args(gen_info_t *ei,list_t *ints){
  
  int len = length_of_list(ints);
  for(int i = len - 1; i >= 0; i--){
    pop(ei,regs[i]);
  }
  
  return;
}

static void pop_floats_args(gen_info_t *ei,list_t *floats){

  int len = length_of_list(floats);
  for(int i = len - 1; i >= 0; i--){
    pop_xmm(ei,i);
  }

  return;
}

static list_t *gen_bin_cmp_op(gen_info_t *ei,env_t *env,list_t *lst){

  char *op;
  list_t *l;
  list_t *r;
  char *inst;
  
  inst = gen_cmp_inst(lst);
  l = cdr(lst);
  gen_body(ei,env,l);
  push(ei,"rax");
  r = cdr(cdr(lst));
  gen_body(ei,env,r);
  pop(ei,"rcx");
  EMIT(ei,"cmp #eax, #ecx");
  
  EMIT(ei,"%s #al", inst);
  EMIT(ei,"movzb #al, #eax");

  return make_null();
}

static void gen_bin_op_int(gen_info_t *ei,env_t *env,list_t *lst){
  
  char *op;
  switch(*(char *)car(lst)){
  case '+':
    op = "add";
    break;
  case '-':
    op = "sub";
    break;
  case '*':
    op = "imul";
    break;
  case '/':
    op = "idiv";
    break;
  default:
    break;
  }
  
  gen_operand(ei,env,cdr(lst));
  push(ei,"rax");
  gen_operand(ei,env,cdr(cdr(lst)));
  pop(ei,"rcx");
  if(*(char *)car(lst) == '-'){
    EMIT(ei,"%s #rax, #rcx",op);
    EMIT(ei,"mov #rcx, #rax");
  } else if(*(char *)car(lst) == '/'){
    EMIT(ei,"%s #rcx, #rax",op);
  } else {
    EMIT(ei,"%s #rcx, #rax",op);
  }
  
  return;
}

static void gen_bin_op_float(gen_info_t *ei,env_t *env,list_t *lst){

  char *op;
  switch(*(char *)car(lst)){
  case '+':
    op = "addss";
    break;
  case '-':
    op = "subss";
    break;
  case '*':
    op = "mulss";
    break;
  case '/':
    op = "divss";
    break;
  default:
    break;
  }
  
  gen_operand(ei,env,cdr(lst));
  push_xmm(ei,0);
  gen_operand(ei,env,cdr(cdr(lst)));
  EMIT(ei,"%s #xmm0,#xmm1","movsd");
  pop_xmm(ei,0);
  EMIT(ei,"%s #xmm1,#xmm0",op);
  
  return;
}

static list_t *gen_bin_op(gen_info_t *ei,env_t *env,list_t *lst){

  int flag_of_float;

  flag_of_float = FALSE;
  if(IS_SYMBOL(lst)){
    symbol_t *sym = lookup_obj(env,car(lst));
    if(is_float(sym)){
      flag_of_float = TRUE;
    }
  } else if(IS_DECIMAL(lst)){
    flag_of_float = TRUE;
  }
  
  if(flag_of_float){
    gen_bin_op_float(ei,env,lst);
  } else {
    gen_bin_op_int(ei,env,lst);
  }
  
  return make_null();
}

static list_t *gen_bit_shift_op(gen_info_t *ei,env_t *env,list_t *lst){

  string_t op;

  #ifdef __DEBUG__
  printf("gen_bit_shift_op\n");
  #endif

  if(STRCMP((string_t)car(lst),BIT_LEFT_SHIFT)){
    op = "shl";
  } else if(STRCMP((string_t)car(lst),BIT_LEFT_SHIFT)){
    op = "shr";
  } else {
    exit(1);
  }
  
  gen_operand(ei,env,cdr(lst));
  push(ei,"rax");
  gen_operand(ei,env,cdr(cdr(lst)));
  pop(ei,"rcx");
  EMIT(ei,"%s #rcx, #rax",op);
  
  return make_null();
}

static list_t *gen_bit_op(gen_info_t *ei,env_t *env,list_t *lst){

  string_t op;

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

  gen_operand(ei,env,cdr(lst));
  push(ei,"rax");
  gen_operand(ei,env,cdr(cdr(lst)));
  pop(ei,"rcx");
  EMIT(ei,"%s #rcx, #rax",op);
  
  return make_null();
}

static int get_size_of_param(env_t *env,list_t *lst){

  compound_def_t *compound_def;
  list_t *l;
  list_t *tail_lst;
  int size;
  
  l = IS_LIST(lst) ? car(lst) : lst;
  tail_lst = tail(l);
  if(is_pointer(l)){
    size = SIZE;
  } else if(STRCMP(car(tail_lst),STRUCT_ALLOC)){
    l = get_at(l,length_of_list(l) - 2);
    compound_def = lookup_obj(env,car(l));
    size = COMPOUND_TYPE_GET_SIZE(compound_def);
  } else {
    size = get_size_of_type(lst);
  }
  
  return size;
}

static int get_size_of_type(list_t *lst){
  
  int size = 0;
  #ifdef __DEBUG__
  printf("get_size_of_type\n");
  #endif
  
  if(STRCMP(INT,car(lst))){
    size = SIZE;
  } else if(STRCMP(CHAR,car(lst))){
    size = sizeof(char);
  } else if(STRCMP(FLOAT,car(lst))){
    size = sizeof(float);
  } else if(STRCMP(DOUBLE,car(lst))){
    size = sizeof(double);
  } else if(STRCMP(SHORT,car(lst))){
    size = sizeof(short);
  } else if(STRCMP(LONG,car(lst))){
    size = sizeof(long);
  } else {
    size = 8;
  }
  
  return size;
}

static list_t *gen_defvar(gen_info_t *ei,env_t *env,list_t *lst){
  
  list_t *new_lst;
  list_t *type_lst;
  list_t *name;
  symbol_t *sym;
  type_t type;
  kind_t kind;
  int size = 0;
  list_t * arr_info;
  int len;
  
  name = make_null();
  if(IS_SYMBOL(lst)){
    if(STRCMP(CONST,car(lst))){
      lst = cdr(lst);
    }
  }
  
  if(STRCMP(car(tail(car(lst))),STRUCT_ALLOC)){
    gen_struct_alloc(ei,env,lst);
  } else if(STRCMP(car(tail(car(lst))),UNION_ALLOC)){
    gen_union_alloc(ei,env,lst);
  } else {
    if(is_function_pointer(lst)){
      name = tail(car(get_at(lst,length_of_list(lst) - 2)));
      type_lst = lst;
      size = SIZE;
      type = TYPE_FUNCTION;
      kind = KIND_POINTER;
      ei->off_set = CALC_OFF(ei->off_set_index);
      ei->off_set_index++;
    } else if(is_array(lst)){
      name = car(cdr(lst));
      arr_info = cdr(car(cdr(lst)));
      len = calc_length_of_array(arr_info);
      type_lst = calc_type(car(lst));
      size = get_size_of_type(type_lst);
      type = gen_type(type_lst);
      kind = select_type(type_lst);
      ei->off_set_index+=(len - 1);
      ei->off_set = CALC_OFF(ei->off_set_index);
      ei->off_set_index++;
    } else {
      name = get_name(lst);
      type_lst = calc_type(car(lst));
      size = get_size_of_type(type_lst);
      type = gen_type(type_lst);
      if(TYPE_UNDEFINE == type){
	type_lst = lookup_primitive_symbol(env,car(type_lst));
	kind = select_type(type_lst);
	type = gen_type(tail(type_lst));
      } else {
	kind = select_type(type_lst);
      }
      
      if(TYPE_STRUCT == type){
	list_t *l = add_symbol(make_null(),car(name));
	l = add_list(l,add_symbol(make_null(),car(type_lst)));
	gen_struct_alloc(ei,env,l);
	return make_null();;
      }
      
      ei->off_set = CALC_OFF(ei->off_set_index);
      ei->off_set_index++;
    }
    
    sym = create_symbol(sym);     
    SYMBOL_SET_VAR_TYPE(sym,LOCAL_VARIABLE);
    SYMBOL_SET_TYPE(sym,type);
    SYMBOL_SET_OFFSET(sym,ei->off_set);
    SYMBOL_SET_SIZE(sym,size);
    SYMBOL_SET_TYPE_LST(sym,car(lst));
    SYMBOL_SET_KIND(sym,kind);
    if(is_inttype(type) || is_function(type)){
      insert_obj(env,car(name),sym);
      EMIT(ei,"mov $%d, #rax",INIT_VALUE,ei->off_set);
      EMIT(ei,"mov #%s, %d(#rbp)",get_int_reg(size),ei->off_set);
    } else {
      name = make_null();
    }
  }
  
  return name;
}

static list_t *gen_logical_not(gen_info_t *ei,env_t *env,list_t *lst){

  gen_operand(ei,env,cdr(lst));
  push(ei,"rax");
  EMIT(ei,"mov $0, #rax");
  pop(ei,"rcx");
  EMIT(ei,"cmp #rax, #rcx");
  EMIT(ei,"sete #al");
  EMIT(ei,"movzbq #al, #rax");
  
  return make_null();
}

static list_t *gen_not(gen_info_t *ei,env_t *env,list_t *lst){
  
  gen_operand(ei,env,cdr(lst));
  EMIT(ei,"not #rax");
  
  return make_null();
}

static list_t *gen_sizeof(gen_info_t *ei,env_t *env,list_t *lst){

  int size;

#ifdef __DEBUG__
  printf("gen_sizeof\n");
#endif
  
  if(STRCMP(TYPE,(string_t)car(lst))){
    size = gen_sizeoftype(ei,env,car(cdr(lst)));
  } else if(STRCMP(EXPR,(string_t)car(lst))){
    exit(1);
  } else {
    exit(1);
  }

  EMIT(ei,"mov $%d, #rax",size);
  
  return make_null();
}

static int gen_sizeoftype(gen_info_t *ei,env_t *env,list_t *lst){

  list_t *type_lst;
  type_t type;
  int size;
  
  #ifdef __DEBUG__
  printf("gen_sizeoftype\n");
  #endif
  
  type_lst = tail(lst);
  type = gen_type(type_lst);
  if(is_pointer(type_lst)){
    return size;
  }

  if(TYPE_UNDEFINE != type){
    size = get_size_of_type(type_lst);
  } else {
    error_no_info("Undefined code");
  }
  
  return size;
}

static void gen_arg(gen_info_t *ei,env_t *env,list_t *lst){

  list_t *new_lst;
  list_t *name;

  name = cdr(lst);
  ei->off_set = CALC_OFF(ei->off_set_index);
  ei->off_set_index++;

  new_lst = add_number(make_null(),ei->off_set);
  new_lst = concat(new_lst,add_number(make_null(),sizeof(int)));
  new_lst = add_number(new_lst,LOCAL_VARIABLE);
  new_lst = add_number(new_lst,LIST_GET_SYMBOL_KIND(name));
  insert(env,name,new_lst);

  return;
}

static void gen_struct_alloc(gen_info_t *ei,env_t *env,list_t *lst){
  
  symbol_t *symbol;
  list_t *body;
  list_t *alloc_body;
  list_t *name;
  compound_def_t *compound_def;
  list_t *struct_name;
  int size;

  struct_name = car(lst);
  name = cdr(lst);
  compound_def = lookup_obj(env,car(struct_name));
  size = COMPOUND_TYPE_GET_SIZE(compound_def);
  ei->off_set -= size;
  ei->off_set_index += (size / SIZE);
  
  symbol = create_symbol();
  SYMBOL_SET_VAR_TYPE(symbol,LOCAL_VARIABLE);
  SYMBOL_SET_TYPE(symbol,TYPE_STRUCT);
  SYMBOL_SET_OFFSET(symbol,ei->off_set);
  SYMBOL_SET_SIZE(symbol,size);
  SYMBOL_SET_KIND(symbol,KIND_VARIABLE);
  SYMBOL_SET_TYPE_LST(symbol,struct_name);
  
  insert_obj(env,car(name),symbol);
  
  return;
}

static void gen_union_alloc(gen_info_t *ei,env_t *env,list_t *lst){

  #ifdef __DEBUG__
  printf("gen_union_alloc\n");
  #endif

  symbol_t *symbol;
  list_t *body;
  list_t *alloc_body;
  list_t *union_name;
  list_t *name;
  compound_def_t *compound_def;
  int size;
  
  union_name = car(lst);
  name = cdr(lst);
  compound_def = lookup_obj(env,car(union_name));

  size = COMPOUND_TYPE_GET_SIZE(compound_def);
  ei->off_set -= size;
  ei->off_set_index += (size / SIZE);
  
  symbol =  create_symbol();
  SYMBOL_SET_VAR_TYPE(symbol,LOCAL_VARIABLE);
  SYMBOL_SET_TYPE(symbol,TYPE_UNION);
  SYMBOL_SET_OFFSET(symbol,ei->off_set);
  SYMBOL_SET_SIZE(symbol,size);
  SYMBOL_SET_KIND(symbol,KIND_VARIABLE);
  SYMBOL_SET_TYPE_LST(symbol,union_name);
  
  insert_obj(env,car(name),symbol);
  
  return;
}

static void gen_struct_alloc_param(gen_info_t *ei,env_t *env,list_t *lst,int index){
  
  list_t *body;
  list_t *alloc_body;
  string_t name;
  compound_def_t *compound_def;
  int size;
  int addr;
  symbol_t *sym;
  list_t *struct_name;

  name = car(cdr(lst));
  struct_name = car(lst);
  
  compound_def = lookup_obj(env,car(struct_name));
  
  size = COMPOUND_TYPE_GET_SIZE(compound_def);
  ei->off_set -= size;
  ei->off_set_index += (size / SIZE);
  
  sym = create_symbol();
  SYMBOL_SET_VAR_TYPE(sym,LOCAL_VARIABLE);
  SYMBOL_SET_TYPE(sym,TYPE_STRUCT);
  SYMBOL_SET_OFFSET(sym,ei->off_set);
  SYMBOL_SET_SIZE(sym,size);
  SYMBOL_SET_KIND(sym,KIND_VARIABLE);
  SYMBOL_SET_TYPE_LST(sym,struct_name);

  insert_obj(env,name,sym);
  EMIT(ei,"movq #%s,%d(#rbp)",regs[index],SYMBOL_GET_OFFSET(sym));
  
  return;
}

static void gen_struct_ref_param(gen_info_t *ei,env_t *env,list_t *lst,int index,symbol_t *sym){

  string_t name;
  
  name = car(cdr(lst));
  SYMBOL_SET_KIND(sym,KIND_POINTER);
  SYMBOL_SET_TYPE(sym,TYPE_STRUCT);
  SYMBOL_SET_TYPE_LST(sym,get_at(car(lst),length_of_list(car(lst)) - 2));
  push(ei,regs[index]);
  insert_obj(env,name,sym);
  
  return;
}

static void gen_if(gen_info_t *ei,env_t *env,list_t *lst){

  list_t *cond;
  list_t *conseq;
  list_t *altern;
  char *l0;

  conseq = car(cdr(cdr(lst)));
  altern = car(cdr(cdr(cdr(lst))));
  cond = car(cdr(lst));

  gen_body(ei,env,cond);
  gen_cmp(ei);
  l0 = make_label();
  gen_je(ei,l0);
  
  gen_body(ei,env,conseq);
  if(IS_NOT_NULL_LIST(altern)){
    char *l1 = make_label();
    gen_jmp(ei,l1);
    gen_label(ei,l0);
    gen_body(ei,env,altern);
    gen_label(ei,l1);
  } else {
    gen_label(ei,l0);
  }
  
  return;
}

static list_t *gen_integer(gen_info_t *ei,list_t *lst){
  
  #ifdef __DEBUG__
  printf("gen_integer\n");
  #endif
  
  EMIT(ei,"mov $%u, #rax",*(int *)car(lst));

  return lst;
}

static void gen_float(gen_info_t *ei,list_t *lst){

  char *l;
  
  #ifdef __DEBUG__
  printf("gen_float\n");
  #endif
  
  l = make_label();
  EMIT(ei,".data");
  gen_label(ei,l);
  EMIT(ei,".quad %lu",*(unsigned long int *)car(lst));
  EMIT(ei,".text");
  EMIT(ei,"movsd %s(#rip), #xmm0", l);
  
  return;
}

static void gen_cmp(gen_info_t *ei){

  #ifdef __DEBUG__
  printf("gen_cmp\n");
  #endif
  
  EMIT(ei,"test #rax, #rax");

  return;
}

static void gen_arg_regs(gen_info_t *ei,list_t *ints,list_t *floats){
  
  list_t *p;
  int i;

  #ifdef __DEBUG__
  printf("gen_arg_regs\n");
  #endif
  
  for(p = ints,i = 0; IS_NOT_NULL_LIST(p); i++,p = cdr(p)){
    push(ei,regs[i]);
  }

  for(p = floats, i = 0; IS_NOT_NULL_LIST(p); i++,p = cdr(p)){
    push_xmm(ei,i);
  }
  
  return;
}

static void gen_arg_regs_after_call(gen_info_t *ei,list_t *ints, list_t *floats){

  int len = 0;
  int i = 0;
  
#ifdef __DEBUG__
  printf("gen_arg_regs_after_call\n");
#endif
  
  len = length_of_list(floats);
  for(i = len - 1; 0 <= i; i--){
    pop_xmm(ei,i);
  }
  
  len = length_of_list(ints);
  for(i = len - 1; 0 <= i; i--){
    pop(ei,regs[i]);
  }
  
  return;
}

static list_t *lookup_member(list_t *lst,char *name){

  list_t *p;

#ifdef __DEBUG__
  printf("lookup_member\n");
#endif
  p = lst;
  while(IS_NOT_NULL_LIST(p)){
    if(STRCMP(car(car(p)),name)){
      return cdr(car(p));
    }    
    p = cdr(p);
  }

  return make_null();
}

static char *get_type(list_t *lst){

  char *type = NULL;

#ifdef __DEBUG__
  printf("get_type\n");
#endif
  
  if(STRCMP((char *)car(lst),INT)){
    type = ".long";
  } else if(STRCMP((char *)car(lst),CHAR)){
    type = ".byte";
  }
  
  return type;
}

static list_t *get_name(list_t *lst){
  
#ifdef __DEBUG__
  printf("get_name\n");
#endif
  return tail(lst);
}

static void gen_je(gen_info_t *ei,char *label){
  
#ifdef __DEBUG__
  printf("gen_je\n");
#endif  
  EMIT(ei,"je %s", label);

  return;
}

static void gen_jmp(gen_info_t *ei,char *label){

#ifdef __DEBUG__
  printf("gen_jmp\n");
#endif
  EMIT(ei,"jmp %s",label);

  return;
}

static void gen_label(gen_info_t *ei,char *label){
  
#ifdef __DEBUG__
  printf("gen_label\n");
#endif
  EMIT(ei,"%s:",label);

  return;
}

static void gen_load_convert(gen_info_t *ei,env_t *env,list_t *lst){

  list_t *var;
  symbol_t *sym;
  type_t type;

#ifdef __DEBUG__
  printf("gen_load_convert\n");
#endif
  
  if(IS_NULL_LIST(lst)){
    return;
  }
  
  sym = lookup_obj(env,car(lst));
  if(!sym){
    return;
  }
  
  type = SYMBOL_GET_VAR_TYPE(sym);
  if((type == TYPE_FLOAT)
     && (LIST_GET_TYPE(cdr(lst)) == DECIMAL)){
    EMIT(ei,"cvtpd2ps #xmm0, #xmm0");
  }
  
  return;
}

static void gen_deref(gen_info_t *ei,env_t *env,list_t *lst){

  char addr[256];
  list_t *val;
  symbol_t *symbol;
  int offset;
  
#ifdef __DEBUG__
  printf("gen_deref\n");
#endif  
  
  symbol = lookup_obj(env,car(lst));
  if(!symbol){
    error_no_info("Not found object [%s]\n",car(lst));
    exit(1);
  }
  
  offset = SYMBOL_GET_OFFSET(symbol);
  push(ei,"rax");
  
  sprintf(addr,"%d(%%rbp)",offset);
  EMIT(ei,"mov %s, #%s",addr,"rax");
  pop(ei,"rcx");
  EMIT(ei,"mov #rcx, (#rax)");
  
  return;
}

static char *gen_cmp_inst(list_t *lst){
  
  char *inst = NULL;
#ifdef __DEBUG__
  printf("gen_cmp_inst\n");
#endif
  if(STRCMP(car(lst),LESS)){
    inst = "setl";
  } else if(STRCMP(car(lst),LESS_EQAUL)){
    inst = "setle";
  }

  return inst;
}

static void gen_ret(gen_info_t *ei){

#ifdef __DEBUG__
  printf("gen_ret\n");
#endif
  
  EMIT(ei,"leave");
  EMIT(ei,"ret");
  ei->off_set = 0;
  ei->off_set_index = 0;
  
  return;
}

static list_t* gen_lvar(gen_info_t *ei,env_t *env,list_t *lst){

  list_t *val;
  symbol_t *sym;
  variable_type_t variable_type;

#ifdef __DEBUG__
  printf("gen_lvar\n");
#endif
  
  sym = lookup_obj(env,car(lst));
  if(!sym){
    error_no_info("Not found object [%s]\n",car(lst));
    exit(1);
  }
  
  variable_type = SYMBOL_GET_VAR_TYPE(sym);
  switch(variable_type){
  case LOCAL_VARIABLE:
    gen_load(ei,"rbp",env,lst,sym);
    break;
    case GLOBAL_VARIABLE:
      gen_gload(ei,env,lst,sym);
      break;
  case OTHER_VARIABLE:
    if(is_function(SYMBOL_GET_TYPE(sym))){
      gen_load_func(ei,env,lst);
    }
    break;
  default:
    break;
  }
  
  return make_null();
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
  int col = vfprintf(GET_OUTPUT_FILE(ei),buf,args);
  va_end(args);
  
  fprintf(GET_OUTPUT_FILE(ei),"\n");
  
  return;
}

static void push(gen_info_t *ei,char *reg){

  EMIT(ei,"push #%s",reg);
  ei->stack_pos += SIZE;
  
  return;
}

static void pop(gen_info_t *ei,char *reg){
  
  EMIT(ei,"pop #%s",reg);
  ei->stack_pos -= SIZE;

  return;
}

static void push_xmm(gen_info_t *ei,int reg){

  EMIT(ei,"sub $8, #rsp");
  EMIT(ei,"movsd #xmm%d, (#rsp)",reg);
  INCREMENT_STACK(ei,8);
  
  return;
}

static int push_struct(gen_info_t *ei,int size){

  int aligned;
  int i;
  aligned = calc_align(size,8);
  EMIT(ei,"sub $%d, #rsp",aligned);
  EMIT(ei,"#rcx, -8(#rsp)");
  EMIT(ei,"r11,  -16(#rsp)");
  EMIT(ei,"#rax, #rcx");
  
  for(i = 0; i < size; i+= 8){
    EMIT(ei,"movq %d(#rcx),#r11",i);
    EMIT(ei,"mov #r11,%d(#rsp)",i);
  }
  
  for(; i < size; i+= 4){
    EMIT(ei,"movl %d(#rcx),r11",i);
    EMIT(ei,"movl #r11d,%d(#rsp)",i);
  }
  
  for(; i < size; i++){
    EMIT(ei,"movb %d(#rcx), #r11",i);
    EMIT(ei,"movb #r11b, %d(#rsp)",i);
  }

  EMIT(ei,"mov -8(#rsp),  #rcx");
  EMIT(ei,"mov -16(#rsp), #r11");
  
  INCREMENT_STACK(ei,aligned);

  return aligned;
}

static void pop_xmm(gen_info_t *ei,int reg){
  
  EMIT(ei,"movsd (#rsp), #xmm%d",reg);
  EMIT(ei,"add $8, #rsp");
  DECREMENT_STACK(ei,8);
  
  return;
}

static int is_type(list_t *lst){

  int flag;

  flag = FALSE;
  if((STRCMP(car(lst),INT))
     || (STRCMP(car(lst),VOID))
     || (STRCMP(car(lst),CHAR))
     || (STRCMP(car(lst),FLOAT))
     || (STRCMP(car(lst),DOUBLE))){
    flag = TRUE;
  }
  
  return flag;
}

static int calc_size_of_struct(list_t *lst){

  list_t *p;
  int size;
  
  p = lst;
  size = 0;
  while(IS_NOT_NULL_LIST(p)){
    size += *(int *)car(cdr(car(p)));
    p = cdr(p);
  }
  
  return size;
}

static void gen_load(gen_info_t *ei,char *base,env_t *env,list_t *lst,symbol_t *symbol){

  int off;
  type_t type;
  
  off = SYMBOL_GET_OFFSET(symbol);
  type = SYMBOL_GET_TYPE(symbol);
   
  char *inst = get_load_inst(ei,env,lst,symbol);
  EMIT(ei,"%s %d(#%s), #rax", inst, off, base);
  if(is_deref(cdr(lst))){
    EMIT(ei,"mov (#rax),#rax");
  }
  
  return;
}

static void gen_load_func(gen_info_t *ei,env_t *env,list_t *lst){
  
#ifdef __LINUX__
  EMIT(ei,"leaq %s(#rip),#rax",(string_t)car(lst));
#else
  EMIT(ei,"leaq _%s(#rip),#rax",(string_t)car(lst));
#endif
  
  return;
}

static void gen_gload(gen_info_t *ei,env_t *env,list_t *lst,symbol_t *sym){
  
  char *inst = get_load_inst(ei,env,lst,sym);
  EMIT(ei,"%s %s+%d(#rip), #rax", inst, (char *)car(lst), 0);
  
  return;
}

static char *get_load_inst(gen_info_t *ei,env_t *env,list_t *lst,symbol_t *symbol){

  char *inst;
  list_t *next;
  
  next = cdr(lst);
  if(is_address(next)){
    return "lea";
  } else if(is_pointer(next)){
    return "mov";
  }

  switch(SYMBOL_GET_SIZE(symbol)){
  case 1:
    inst = "movsb";
    break;
  case 2:
    inst = "movsw";
    break;
  case 4:
    inst = "movslq";
    break;
  case 8:
    inst = "mov";
    break;
  }
  
  return inst;
}

static void gen_save(gen_info_t *ei,env_t *env,list_t *lst){
  
  if(is_deref(cdr(lst))){
    gen_deref(ei,env,lst);
  } else {
    symbol_t *sym;
    sym = (symbol_t *)car(lookup(env,lst));
    if(LOCAL_VARIABLE == SYMBOL_GET_VAR_TYPE(sym)){
      gen_lsave(ei,env,sym);
    } else if(GLOBAL_VARIABLE == SYMBOL_GET_VAR_TYPE(sym)){
      gen_gsave(ei,env,lst,sym);
    }
  }

  return;
}

static void gen_gsave(gen_info_t *ei,env_t *env,list_t *name,symbol_t *sym){

  char *reg;
  char addr[256];
  
  reg = get_int_reg(SYMBOL_GET_SIZE(sym));
  
  sprintf(addr,"%s+%d(%%rip)",(string_t)car(name),0);
  EMIT(ei,"mov #%s, %s", reg, addr);
  
  return;
}

static void gen_lsave(gen_info_t *ei,env_t *env,symbol_t *sym){

  int off;
  char *reg;
  type_t type;
  char addr[256];
  
  type = SYMBOL_GET_TYPE(sym);
  off = SYMBOL_GET_OFFSET(sym);
  reg = get_int_reg(SYMBOL_GET_SIZE(sym));
  
  sprintf(addr,"%d(%%rbp)",off);
  EMIT(ei,"mov #%s,%s",reg,addr);
  
  return;
}

static void gen_while(gen_info_t *ei,env_t *env,list_t *lst){
  
  char *l0;
  char *l1;
  
  l0 = make_label();
  l1 = make_label();
  gen_label(ei,l0);
  gen_body(ei,env,car(cdr(lst)));
  gen_cmp(ei);
  gen_je(ei,l1);
  gen_body(ei,env,cdr(cdr(lst)));
  gen_jmp(ei,l0);
  gen_label(ei,l1);
  
  return;
}

static void gen_for(gen_info_t *ei,env_t *env,list_t *lst){

  list_t *new_lst;
  char *l0;
  char *l1;
  
  l0 = make_label();
  l1 = make_label();
  
  gen_body(ei,env,car(cdr(lst)));

  gen_label(ei,l0);
  gen_body(ei,env,car(cdr(cdr(lst))));
  gen_cmp(ei);
  gen_je(ei,l1);
  gen_body(ei,env,car(cdr(cdr(cdr(cdr(lst))))));
  gen_body(ei,env,car(cdr(cdr(cdr(lst)))));
  gen_jmp(ei,l0);
  gen_label(ei,l1);
  
  return;
}

static list_t *gen_array(gen_info_t *ei,env_t *env,list_t *lst){

  list_t *obj;
  symbol_t *sym;
  char *inst;
  int off;
  int flag;

  sym = lookup_obj(env,car(cdr(lst)));
  if(!sym){
    error_no_info("Not found object [%s]\n",car(lst));
    exit(1);
  }
  
  off = SYMBOL_GET_OFFSET(sym);
  EMIT(ei,"lea %d(#%s),#%s",off,"rbp","rax");
  push(ei,"rax");
  ENABLE_ARRAY_FLAG(ei);
  
  flag = FALSE;
  if(IS_TO_ENABLE_ASSIGN_FLAG(ei)){
    DISENABLE_ASSIGN_FLAG(ei);
    flag = TRUE;
  }
  
  gen_index_of_array(ei,env,cdr(cdr(lst)));
  
  if(flag){
    ENABLE_ASSIGN_FLAG(ei);
  }
  
  DISENABLE_ARRAY_FLAG(ei);
  if(IS_TO_ENABLE_ASSIGN_FLAG(ei)){
    pop(ei,"rcx");
    EMIT(ei,"mov #%s,(#%s)","rcx","rax");
  } else {
    EMIT(ei,"mov (#%s),#%s","rax","rax");
  }
  
  
  return make_null();
}

static void gen_index_of_array(gen_info_t *ei,env_t *env,list_t *lst){
  
  gen_body(ei,env,lst);
  EMIT(ei,"mov $%d,#%s",8,"rcx");
  EMIT(ei,"imul #%s,#%s","rcx","rax");
  pop(ei,"rcx");
  EMIT(ei,"add #%s,#%s","rcx","rax");
  
  return;
}

static list_t *gen_cast(gen_info_t *ei,env_t *env,list_t *lst){

  list_t *val;
  symbol_t *symbol;
  type_t src_type;
  type_t cast_type;
  
  #ifdef __DEBUG__
  printf("gen_cast\n");
  #endif

  val = make_null();
  cast_type = gen_type(tail(car(lst)));
  symbol = lookup_obj(env,car(cdr(lst)));
  if(!symbol){
    exit(1);
  }

  gen_operand(ei,env,cdr(lst));
  src_type = SYMBOL_GET_TYPE(symbol);
  switch(cast_type){
  case TYPE_INT:
    gen_cast_int(ei,src_type);
    break;
  case TYPE_LONG:
    gen_cast_long(ei,src_type);
    break;
  default:
    break;
  }
  
  return val;
}

static void gen_cast_int(gen_info_t *ei,type_t src_type){

  #ifdef __DEBUG__
  printf("gen_cast_int\n");
  #endif

  switch(src_type){
  case TYPE_SHORT:
    EMIT(ei,"movswq #ax,#rax");
    break;
  default:
    exit(1);
  }
  
  return;
}

static void gen_cast_long(gen_info_t *ei,type_t src_type){

  #ifdef __DEBUG__
  printf("gen_cast_int\n");
  #endif
  
  switch(src_type){
  case TYPE_SHORT:
    EMIT(ei,"movswq #ax,#rax");
    break;
  case TYPE_INT:
    EMIT(ei,"movslq #eax,#rax");
    break;
  case TYPE_LONG:
    break;
  default:
    exit(1);
  }
  
  return;
}

static void gen_typedef(gen_info_t *ei,env_t *env,list_t *lst){
  
  symbol_t *sym;
  list_t *type_lst;
  list_t *val;
  string_t name;
  
#ifdef __DEBUG__
  printf("gen_typedef\n");
#endif
  
  name = car(cdr(lst));
  sym = create_symbol(sym);
  SYMBOL_SET_VAR_TYPE(sym,OTHER_VARIABLE);
  SYMBOL_SET_TYPE(sym,TYPE_TYPE);
  SYMBOL_SET_KIND(sym,KIND_TYPE);
  type_lst = gen_type_of_typedef(ei,env,lst);
  SYMBOL_SET_TYPE_LST(sym,type_lst);
  insert_obj(env,name,sym);

  return;
}

static list_t *gen_type_of_typedef(gen_info_t *ei,env_t *env,list_t *lst){

  list_t *val;

#ifdef __DEBUG__
  printf("gen_type_of_typedef\n");
#endif
  
  val = make_null();
  if(is_compund_type(car(lst))){
    val = gen(ei,env,lst,FALSE);
  } else {
    val = car(lst);
  }
  
  return val;
}

static list_t *gen_access_member(gen_info_t *ei,env_t *env,list_t *lst){

  list_t *member;
  symbol_t *sym;
  compound_def_t *compound;
  int size;
  int addr;
  int base;
  
  sym = lookup_obj(env,car(lst));
  
  compound = lookup_obj(env,car(SYMBOL_GET_TYPE_LST(sym)));
  
  member = lookup_member(COMPOUND_TYPE_GET_MEMBERS(compound),car(cdr(lst)));
  
  base = SYMBOL_GET_OFFSET(sym);
  addr = base + *(int *)car(cdr(member));
  
  if(IS_TO_ENABLE_ASSIGN_FLAG(ei)){
    char *reg = get_int_reg(*(int *)car(member));
    pop(ei,reg);
    EMIT(ei,"mov #%s, %d(#rbp)",reg,addr);
  } else {
    EMIT(ei,"mov %d(#rbp), #%s",addr,get_int_reg(*(int *)car(member)));
  }
  
  return make_null();
}

static list_t *gen_ref_member(gen_info_t *ei,env_t *env,list_t *lst){

  list_t *member;
  symbol_t *sym;
  compound_def_t *compound;
  int base;
  int offset;

  sym = lookup_obj(env,car(lst));
  compound = lookup_obj(env,car(SYMBOL_GET_TYPE_LST(sym)));
  
  member = lookup_member(COMPOUND_TYPE_GET_MEMBERS(compound),car(cdr(lst)));
  
  base = SYMBOL_GET_OFFSET(sym);
  offset = *(int *)car(cdr(member));
  if(IS_TO_ENABLE_ASSIGN_FLAG(ei)){
    pop(ei,"rax");
    push(ei,"rax");
    EMIT(ei,"mov %d(#rbp), #rax",base);
    pop(ei,"rcx");
    EMIT(ei,"mov #rcx, %d(#rax)",offset);
  } else {
    EMIT(ei,"mov %d(#rbp), #rax",base);
    EMIT(ei,"mov %d(#rax), #rcx",offset);
    push(ei,"rcx");
    pop(ei,"rax");
  }

  return make_null();
}

static void gen_continue(gen_info_t *ei,env_t *env,list_t *lst){

  
  
  return;
}

static char *get_int_reg(int size){

  char *reg;
  
  switch(size){
  case 1:
    reg = "al";
    break;
  case 2:
    reg = "ax";
    break;
  case 4:
    reg = "eax";
  case 8:
    reg = "rax";
    break;
  }

  return reg;
}

static int align(env_t *env,list_t *lst){
  
  int size = 0;
  list_t *tail_lst;
  
  tail_lst = tail(lst);
  if(STRCMP(car(cdr(tail_lst)),ARRAY)){
    size = SIZE * calc_align_size(car(cdr(cdr(lst))));
  } else if(STRCMP(car(tail_lst),STRUCT_ALLOC)){
    size = get_size_of_param(env,lst);
  } else {
    size = SIZE;
  }

  return size;
}

static int calc_align_size(list_t *lst){
  
  if(IS_INTEGER(cdr(lst))){
    return *(int *)car(cdr(lst));
  } else {
    return 0;
  }
}

static int calc_length_of_array(list_t *lst){

  list_t *p;
  int len;
  
  len = 0;
  p = lst;
  while((IS_NOT_NULL_LIST(p))){
    len += *(int *)car(cdr(car(p)));
    p = cdr(p);
  }
  
  return len;
}

static char *choose_increment_op(list_t *lst){

  char *op;

  op = NULL;
  if(STRCMP(car(lst),INCREMENT)){
    op = "add";
  } else if(STRCMP(car(lst),DECREMENT)){
    op = "sub";
  } else if(STRCMP(car(lst),INCREMNT_ASSING)){
    op = "add";
  } else if(STRCMP(car(lst),DECREMNT_ASSING)){
    op = "sub";
  }
  
  return op;
}

static list_t *classify_args(list_t *args,env_t *env){
  
  list_t *ints;
  list_t *floats;
  list_t *rests;
  list_t *l;
  list_t *lst;
  
  ints = make_null();
  floats = make_null();
  rests = make_null();
  l = args;
  while(IS_NOT_NULL_LIST(l)){
    list_t *tmp;
    tmp = cdr(l);
    LIST_SET_NEXT(l,make_null());
    if(IS_DECIMAL(l)){
      floats = concat(floats,l);
    } else if(IS_INTEGER(l)){
      ints = concat(ints,l);
    } else if(IS_SYMBOL(l)){
      symbol_t *sym;
      sym = lookup_obj(env,car(l));
      
      if(is_float(sym)){
	floats = concat(floats,l);
      } else if(is_struct(sym)){
	rests = concat(rests,l);
      } else {
	ints = concat(ints,l);
      }
    } else {
      ints = concat(ints,l);
    }
    l = tmp;
  }

  lst = make_null();
  lst = add_list(lst,rests);
  lst = add_list(lst,floats);
  lst = add_list(lst,ints);

  return lst;
}

static list_t *lookup_primitive_symbol(env_t *env,string_t name){

  symbol_t *symbol;
  
  #ifdef __DEBUG__
  printf("lookup_primitive_symbol\n");
  #endif
  
  symbol = lookup_obj(env,name);
  if(!symbol){
    printf("%d : Unknown object : [%s]\n",__LINE__,name);
    exit(1);
  }
  
  if(TYPE_TYPE == SYMBOL_GET_TYPE(symbol)){
    return SYMBOL_GET_TYPE_LST(symbol);
  } else if(TYPE_STRUCT != SYMBOL_GET_TYPE(symbol)){
    if(check_primitive_symbol(symbol)){
      return 0;
    } else { 
      list_t *lst = SYMBOL_GET_TYPE_LST(symbol);
      type_t type = gen_type(tail(lst));
      if(TYPE_UNDEFINE == type){
	return lookup_primitive_symbol(env,car(lst));
      } else {
	return lst;
      }
    }
  }
  
  return make_null();
}

static int check_primitive_symbol(symbol_t *sym){

  int flag;
  
#ifdef __DEBUG__
  printf("check_primitive_symbol\n");
#endif
  
  switch(SYMBOL_GET_TYPE(sym)){
  case TYPE_INT:
  case TYPE_CHAR:
  case TYPE_SHORT:
  case TYPE_DOUBLE:
  case TYPE_FLOAT:
    flag  = TRUE;
    break;
  default:
    flag = FALSE;
    break;
  }
  
  return flag;
}

static list_t *calc_type(list_t *lst){

  list_t *p;
  list_t *pre;
  list_t *tmp;
  
#ifdef __DEBUG__
  printf("calc_type\n");
#endif

  p = NULL;
  p = lst;
  while(TRUE){
    if(!STRCMP("*",(string_t)car(p))){
      return p;
    }
    pre = p;
    p = cdr(p);
  }
  
  lst = p;
  
  return p;
}

static string_t emit_no_name_label(gen_info_t *ei){
  
  char buf[BUF_SIZE];
  string_t name;
  int len;
  
#ifdef __DEBUG__
  printf("emit_no_name_label\n");
#endif
  
  len = 0;
  memset(buf,INIT_VALUE,sizeof(buf) / sizeof(buf[0]));
  ei->no_name_label_no++;
  sprintf(buf,"%s-%d",NO_NAME,ei->no_name_label_no);
  len = strlen(buf);
  name = mem(len + NULL_LEN);
  memcpy(name,buf,len);
  
  return name;
}

static int gen_regsave(gen_info_t *ei){

  #ifdef __DEBUG__
  printf("gen_regsave\n");
  #endif
  
  EMIT(ei,"sub $%d, #rsp",SAVED_REGISTER_SIZE);
  EMIT(ei,"mov #rdi,   (#rbp)");
  EMIT(ei,"mov #rsi, -8(#rbp)");
  EMIT(ei,"mov #rdx, -16(#rbp)");
  EMIT(ei,"mov #rcx, -24(#rbp)");
  EMIT(ei,"mov #r8,  -32(#rbp)");
  EMIT(ei,"mov #r9,  -40(#rbp)");

  EMIT(ei,"movaps #xmm0, -48(#rbp)");
  EMIT(ei,"movaps #xmm1, -64(#rbp)");
  EMIT(ei,"movaps #xmm2, -80(#rbp)");
  EMIT(ei,"movaps #xmm3, -96(#rbp)");
  EMIT(ei,"movaps #xmm4, -112(#rbp)");
  EMIT(ei,"movaps #xmm5, -128(#rbp)");
  EMIT(ei,"movaps #xmm6, -144(#rbp)");
  EMIT(ei,"movaps #xmm7, -160(#rbp)");

  return SAVED_REGISTER_SIZE;
}

static void gen_builtin_function(list_t *lst){
  
#ifdef __DEBUG__
  printf("gen_builtin_function\n");
#endif
  
  return;
}

static void gen_builtin_va_start(list_t *lst){

#ifdef __DEBUG__
  printf("gen_builtin_va_start\n");
#endif

  return;
}


