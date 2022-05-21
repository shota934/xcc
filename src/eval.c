#include "type.h"
#include "env.h"
#include "list.h"
#include "common.h"
#include "inst.inc"

static list_t *eval_symbol(env_t *env,list_t *lst);
static list_t *eval_operand(env_t *env,list_t *lst);
static list_t *eval_number(env_t *env,list_t *lst);
static list_t *eval_bin_op(env_t *env,list_t *lst);

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
	val = make_null();
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

