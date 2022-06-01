#include "type.h"
#include "env.h"
#include "list.h"
#include "common.h"
#include "symbol.h"
#include "inst.inc"

static list_t *eval_symbol(env_t *env,list_t *lst);
static list_t *eval_operand(env_t *env,list_t *lst);
static list_t *eval_number(env_t *env,list_t *lst);
static list_t *eval_bin_op(env_t *env,list_t *lst);
static list_t *lookup(env_t *env,list_t *lst);
static bool_t eval_complex_value(env_t *env,list_t *lst);

list_t *eval(env_t *env,list_t *lst){

  list_t *val;
#ifdef __DEBUG__
  printf("eval\n");
#endif

  switch(LIST_GET_TYPE(lst)){
  case LIST:
    val = eval(env,(list_t *)car(lst));
	break;
  case SYMBOL:
	val = eval_symbol(env,lst);
	break;
  case INTEGER:
    val = eval_number(env,lst);
	break;
  default:
	break;
  }

  return val;
}

static list_t *eval_symbol(env_t *env,list_t *lst){

  list_t *val;
  string_t symbol;

#ifdef __DEBUG__
  printf("eval_symbol\n");
#endif
  symbol = (string_t)car(lst);
  if(STRCMP(symbol,ADD)
	 || STRCMP(symbol,SUB)
	 || STRCMP(symbol,MUL)
	 || STRCMP(symbol,DIV)){
    val = eval_bin_op(env,lst);
  } else {
	val = lookup(env,lst);
  }

  return val;
}

static list_t *eval_operand(env_t *env,list_t *lst){

  list_t *ret;

#ifdef __DEBUG__
  printf("eval_operand\n");
#endif

  switch(LIST_GET_TYPE(lst)){
  case LIST:
    ret = eval_operand(env,(list_t *)car(lst));
    break;
  case SYMBOL:
    ret = eval_symbol(env,lst);
    break;
  case INTEGER:
    ret = eval_number(env,lst);
    break;
  }
  
  return ret;
}

static list_t *eval_number(env_t *env,list_t *lst){
  
#ifdef __DEBUG__
  printf("eval_number\n");
#endif
  
  return add_number(make_null(),*(integer_t *)car(lst));
}

static list_t *eval_bin_op(env_t *env,list_t *lst){

  list_t *l;
  list_t *r;
  string_t op;
  integer_t ret;
  
#ifdef __DEBUG__
  printf("eval_bin_op\n");
#endif

  op = (string_t)car(lst);
  l = eval_operand(env,cdr(lst));
  r = eval_operand(env,cdr(cdr(lst)));

  if(STRCMP(op,ADD)){
    ret = *(integer_t*)car(l) + *(integer_t*)car(r);
  } else if(STRCMP(op,SUB)){
    ret = *(integer_t*)car(l) - *(integer_t*)car(r);
  } else if(STRCMP(op,MUL)){
    ret = *(integer_t*)car(l) * *(integer_t*)car(r);
  } else if(STRCMP(op,DIV)){
    ret = *(integer_t *)car(l) / *(integer_t *)car(r);
  }

  return add_number(make_null(),ret);
}

static list_t *lookup(env_t *env,list_t *lst){

  list_t *val;
  string_t name;
  string_t val_name;
  symbol_t *sym;

#ifdef __DEBUG__
  printf("lookup\n");
#endif

  name = (string_t)car(lst);
  sym = lookup_obj(env,name);
  if(!sym){
	return make_null();
  }

  val = make_null();
  switch(SYMBOL_GET_SCOPE(sym)){
  case STATIC_LOCAL:
  case STATIC_GLOBAL:
	val_name = SYMBOL_GET_STATIC_VAR(sym);
	break;
  case GLOBAL:
	val_name = SYMBOL_GET_NAME(sym);
	break;
  default:
	return make_null();
	break;
  }

  if(!eval_complex_value(env,cdr(lst))){
	return val;
  }
  
  return add_symbol(val,val_name);
}

static bool_t eval_complex_value(env_t *env,list_t *lst){

  string_t symbol;

  if(IS_NULL_LIST(lst)){
	return FALSE;
  }

  symbol = (string_t)car(lst);
  if(STRCMP(symbol,ADDRESS)){
	return TRUE;
  } else {
	return FALSE;
  }
}



