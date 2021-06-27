#include "tych.h"
#include "common.h"
#include "inst.inc"
#include "func.h"
#include "compound_type.h"

static type_t check_op_type_list(env_t *env,list_t *lst);
static type_t check_op_type_sym(env_t *env,list_t *lst);
static type_t check_op_type_string();
static type_t check_op_type_int();
static type_t check_op_type_flot();
static type_t check_op_type_char();
static type_t check_arith_op(env_t *env,list_t *lst);
static type_t check_not_op(env_t *env,list_t *lst);
static type_t check_bit_op(env_t *env,list_t *lst);
static type_t check_bit_shift_op(env_t *env,list_t *lst);

static type_t check_op_func(env_t *env,list_t *lst);
static type_t check_op_dot(env_t *env,list_t *lst);
static type_t check_op_ref(env_t *env,list_t *lst);
static type_t check_op_array(env_t *env,list_t *lst);
static type_t check_op_cmp(env_t *env,list_t *lst);
static type_t convert_integer(type_t type);
static type_t check_type_enum();
static type_t check_sizeof(env_t *env,list_t *lst);
static type_t check_ttype(env_t *env,list_t *lst);

static type_t analyze_type(env_t *env,list_t *type_lst,list_t *lst);
static type_t analyze_utype(list_t *lst,type_t ty);
static type_t analyze_array(env_t *env,list_t *type_lst,list_t *lst);


type_t check_type(env_t *env,list_t *lst){

  type_t val;
#ifdef __DEBUG__
  printf("check_type\n");
#endif

  val = check_op_type_list(env,lst);

  return val;
}

static type_t check_op_type_list(env_t *env,list_t *lst){

  type_t val;
  list_type_t type;
#ifdef __DEBUG__
  printf("check_op_type\n");
#endif

  type = LIST_GET_TYPE(lst);
  switch(type){
  case LIST:
	val = check_op_type_list(env,car(lst));
	break;
  case SYMBOL:
	val = check_op_type_sym(env,lst);
	break;
  case INTEGER:
	val = check_op_type_int();
	break;
  case DECIMAL:
	val = check_op_type_flot();
	break;
  case STRING:
	val = check_op_type_string();
	break;
  case CHARACTER:
	val = check_op_type_char();
	break;
  default:
	exit(1);
	break;
  }

  return val;
}

static type_t check_op_type_string(){

#ifdef __DEBUG__
  printf("check_op_type_string\n");
#endif

  return TYPE_STRING;
}

static type_t check_op_type_char(){

#ifdef __DEBUG__
  printf("check_op_type_char\n");
#endif

  return TYPE_CHAR;
}

static type_t check_op_type_sym(env_t *env,list_t *lst){

  string_t name;
  symbol_t *sym;
  object_t *obj;

#ifdef __DEBUG__
  printf("check_op_type_sym\n");
#endif

  name = car(lst);
  if(STRCMP(name,ADD)){
	return check_arith_op(env,cdr(lst));
  } else if(STRCMP(name,SUB)){
	return check_arith_op(env,cdr(lst));
  } else if(STRCMP(name,MUL)){
	return check_arith_op(env,cdr(lst));
  } else if(STRCMP(name,DIV)){
	return check_arith_op(env,cdr(lst));
  } else if(STRCMP(name,MOD)){
	return check_arith_op(env,cdr(lst));
  } else if(STRCMP(FUNC_CALL,name)){
	return check_op_func(env,cdr(lst));
  } else if(STRCMP(DOT,name)){
	return check_op_dot(env,cdr(lst));
  } else if(STRCMP(REF_MEMBER_ACCESS,name)){
	return check_op_ref(env,cdr(lst));
  } else if(STRCMP(ARRAY,name)){
	return check_op_array(env,cdr(lst));
  } else if(STRCMP(LESS,name)
			|| STRCMP(GREATER,name)
			|| STRCMP(LESS_EQAUL,name)
			|| STRCMP(NOT_EQUAL,name)
			|| STRCMP(EQUAL,name)){
	return check_op_cmp(env,cdr(lst));
  } else if(STRCMP(BIT_LEFT_SHIFT,name)
			|| (STRCMP(BIT_RIGHT_SHIFT,name))){
	return check_bit_shift_op(env,cdr(lst));
  } else if(STRCMP(AND,name)
			|| STRCMP(OR,name)
			|| STRCMP(XOR,name)){
	return check_bit_op(env,cdr(lst));
  } else if(STRCMP(SIZEOF,name)){
	return check_sizeof(env,cdr(lst));
  } else if(STRCMP(NOT,name)){
	return check_not_op(env,cdr(lst));
  } else if(STRCMP(TYPE,name)){
	return check_ttype(env,cdr(lst));
  } else {
	obj = lookup_obj(env,name);
	if(!obj){
	  printf("name : %s\n",name);
	  exit(1);
	}
	switch(OBJ_GET_TYPE(obj)){
	case TYPE_SYMBOL:
	case TYPE_COMPOUND:
	  sym = (symbol_t *)obj;
	  return analyze_type(env,SYMBOL_GET_TYPE_LST(sym),cdr(lst));
	  break;
	case TYPE_ENUMULATE:
	  return check_type_enum();
	  break;
	}
  }

  return -1;
}

static type_t check_op_type_int(){

#ifdef __DEBUG__
  printf("check_op_type_int\n");
#endif

  return TYPE_INT;
}

static type_t check_op_type_flot(){

#ifdef __DEBUG__
  printf("check_op_type_flot\n");
#endif

  return TYPE_FLOAT;
}

static type_t check_arith_op(env_t *env,list_t *lst){

  type_t l;
  type_t r;

#ifdef __DEBUG__
  printf("check_arith_op\n");
#endif

  l = convert_integer(check_op_type_list(env,lst));
  r = convert_integer(check_op_type_list(env,cdr(lst)));

  if(l == r){
	return l;
  } else if(l == TYPE_POINTER && r == TYPE_INT){
	return TYPE_POINTER;
  } else if(l == TYPE_INT && r == TYPE_POINTER){
	return TYPE_POINTER;
  } else if(l == TYPE_FLOAT && r == TYPE_INT){
	return TYPE_FLOAT;
  } else if(l == TYPE_INT && r == TYPE_FLOAT){
	return TYPE_FLOAT;
  } else if(l == TYPE_INT && r == TYPE_DOUBLE){
	return TYPE_DOUBLE;
  } else if(l == TYPE_DOUBLE && r == TYPE_INT){
	return TYPE_DOUBLE;
  } else {
	DUMP_AST(lst);
  }

  return -1;
}

static type_t check_not_op(env_t *env,list_t *lst){

#ifndef __DEBUG__
  printf("check_not_op\n");
#endif

  exit(1);

  return -1;
}

static type_t check_bit_op(env_t *env,list_t *lst){

  type_t l;
  type_t r;

#ifndef __DEBUG__
  printf("check_bit_op\n");
#endif

  l = convert_integer(check_op_type_list(env,lst));
  r = convert_integer(check_op_type_list(env,cdr(lst)));
  if(l == r){
	return l;
  }

  return -1;
}

static type_t check_bit_shift_op(env_t *env,list_t *lst){

  type_t l;
  type_t r;

#ifndef __DEBUG__
  printf("check_bit_shift_op\n");
#endif

  l = convert_integer(check_op_type_list(env,lst));
  r = convert_integer(check_op_type_list(env,cdr(lst)));
  if(l == r){
	return l;
  }

  return -1;
}


static type_t check_op_func(env_t *env,list_t *lst){

  func_t *func;
  string_t name;

#ifdef __DEBUG__
  printf("check_op_func\n");
#endif

  name = car(lst);
  func = lookup_obj(env,name);
  if(!func){
	exit(1);
  }

  DUMP_AST(FUNC_GET_RET_TYPE(func));
  return analyze_type(env,FUNC_GET_RET_TYPE(func),make_null());
}

static type_t check_op_dot(env_t *env,list_t *lst){

  type_t type;
  symbol_t *sym;
  symbol_t *sym_mem;
  compound_def_t *com;

#ifdef __DEBUG__
  printf("check_op_dot\n");
#endif
  sym = lookup_obj(env,car(lst));
  if(!sym){
	exit(1);
  }

  com = lookup_obj(env,car(tail(SYMBOL_GET_TYPE_LST(sym))));
  if(!com){
	exit(1);
  }

  if(IS_LIST(cdr(lst))){
	type = check_op_type_list(COMPOUND_TYPE_GET_ENV(com),car(cdr(lst)));
  } else {
	sym_mem = lookup_member(COMPOUND_TYPE_GET_ENV(com),car(cdr(lst)));
	type = analyze_type(env,SYMBOL_GET_TYPE_LST(sym_mem),make_null());
  }

  return type;
}

static type_t check_op_ref(env_t *env,list_t *lst){

  type_t type;
  symbol_t *sym;
  symbol_t *sym_mem;
  compound_def_t *com;

#ifdef __DEBUG__
  printf("check_op_ref\n");
#endif

  sym = lookup_obj(env,car(lst));
  if(!sym){
	exit(1);
  }

  com = lookup_obj(env,car(tail(SYMBOL_GET_TYPE_LST(sym))));
  if(!com){
	exit(1);
  }

  if(IS_LIST(cdr(lst))){
	type = check_op_type_list(COMPOUND_TYPE_GET_ENV(com),car(cdr(lst)));
  } else {
	sym_mem = lookup_member(COMPOUND_TYPE_GET_ENV(com),car(cdr(lst)));
	type = analyze_type(env,SYMBOL_GET_TYPE_LST(sym_mem),make_null());
  }

  return type;
}

static type_t check_op_array(env_t *env,list_t *lst){

  type_t type;
#ifdef __DEBUG__
  printf("check_op_array\n");
#endif

  exit(1);
  return type;
}

static type_t check_op_cmp(env_t *env,list_t *lst){

  type_t l;
  type_t r;

#ifdef __DEBUG__
  printf("check_op_cmp\n");
#endif

  l = convert_integer(check_op_type_list(env,lst));
  r = convert_integer(check_op_type_list(env,cdr(lst)));
  if(l == r){
	return l;
  }

  return -1;
}

static type_t convert_integer(type_t type){

#ifdef __DEBUG__
  printf("convert_integer\n");
#endif

  switch(type){
  case TYPE_CHAR:
  case TYPE_SHORT:
  case TYPE_INT:
  case TYPE_LONG:
  case TYPE_SIGNED:
  case TYPE_UNSIGNED:
	return TYPE_INT;
  default:
	break;
  }

  return type;
}

static type_t check_type_enum(){

#ifdef __DEBUG__
  printf("check_type_enum\n");
#endif

  return TYPE_ENUM;
}

static type_t check_sizeof(env_t *env,list_t *lst){

#ifdef __DEBUG__
  printf("check_sizeof\n");
#endif

  return check_op_type_list(env,lst);
}

static type_t check_ttype(env_t *env,list_t *lst){

#ifdef __DEBUG__
  printf("check_ttype\n");
#endif

  return analyze_type(env,lst,make_null());
}

static type_t analyze_type(env_t *env,list_t *type_lst,list_t *lst){

  string_t name;
  type_t type;
  symbol_t *sym;

#ifdef __DEBUG__
  printf("analyze_type\n");
#endif

  DUMP_AST(lst);
  if(IS_NULL_LIST(type_lst) || !IS_SYMBOL(type_lst)){
	return -1;
  }

  name = car(type_lst);
  if(STRCMP(name,"*")){
	type = analyze_utype(lst,TYPE_POINTER);
  } else if(STRCMP(name,"char")){
	type = analyze_utype(lst,TYPE_CHAR);
  } else if(STRCMP(name,"int")){
	type = analyze_utype(lst,TYPE_INT);
  } else if(STRCMP(name,"float")){
	type = analyze_utype(lst,TYPE_FLOAT);
  } else if(STRCMP(name,ARRAY)){
	type = analyze_array(env,cdr(type_lst),lst);
  } else if(STRCMP(name,STRUCT_ALLOC)){
	type = analyze_utype(lst,TYPE_STRUCT);
  } else {
	sym = lookup_obj(env,name);
	if(sym){
	  switch(SYMBOL_GET_SYM_TYPE(sym)){
	  case TYPE_ENUM:
		return analyze_utype(lst,TYPE_ENUM);
		break;
	  }
	}

	DUMP_AST(type_lst);
	exit(1);
  }

  return type;
}

static type_t analyze_utype(list_t *lst,type_t ty){

  string_t name;
  type_t type;

#ifdef __DEBUG__
  printf("analyze_utype\n");
#endif

  name = car(lst);
  if(STRCMP(name,"*") && ty == TYPE_POINTER){
	type =  TYPE_INT;
  } else if(STRCMP(name,"&")){
	type = TYPE_POINTER;
  } else {
	type = ty;
  }

  return type;
}

static type_t analyze_array(env_t *env,list_t *type_lst,list_t *lst){

  type_t type;

#ifdef __DEBUG__
  printf("analyze_array\n");
#endif

  if(IS_NULL_LIST(cdr(type_lst))){
	return TYPE_ARRAY;
  }

  return analyze_type(env,cdr(type_lst),make_null());
}
