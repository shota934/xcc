#include "sema.h"
#include "util.h"
#include "inst.inc"
#include "mem.h"

bool_t check_body(sema_t*sema,list_t *lst);
bool_t check_symbol(sema_t*sema,list_t *lst);
bool_t check_integer(sema_t*sema,list_t *lst);
bool_t check_func_def(sema_t*sema,list_t *lst);

sema_t *create_sema(string_t name){

  sema_t *sema;

  sema = mem(sizeof(sema_t));
  sema->set = set_create();
  sema->src = name;

  return sema;
}

bool_t check(sema_t *sema,list_t *lst){

#ifdef __DEBUG__
  printf("check\n");
#endif

  return check_body(sema,lst);
}

bool_t check_body(sema_t*sema,list_t *lst){

  list_type_t type;
  bool_t ret;
#ifdef __DEBUG__
  printf("check_body\n");
#endif

  type = LIST_GET_TYPE(lst);
  switch(type){
  case LIST:
	ret = check_body(sema,car(lst));
	if(IS_NOT_NULL_LIST(cdr(lst))){
	  ret = check_body(sema,cdr(lst));
	}
	break;
  case SYMBOL:
	ret = check_symbol(sema,lst);
	break;
  case INTEGER:
	ret = check_integer(sema,lst);
	break;
  case NULL_LIST:
	return TRUE;
	break;
  }

  return TRUE;
}

bool_t check_symbol(sema_t*sema,list_t *lst){

  string_t name;
#ifdef __DEBUG__
  printf("check_symbol\n");
#endif

  name = car(lst);
  if(STRCMP(name,FUNC_DEF)){
	return check_func_def(sema,cdr(lst));
  } else {

  }

  return TRUE;
}

bool_t check_func_def(sema_t*sema,list_t *lst){

  list_t *l;
  string_t name;
  set_t *set;

#ifdef __DEBUG__
  printf("check_func_def\n");
#endif

  set = SEMA_GET_SET(sema);
  l = get_func_name(lst);
  name = LIST_GET_SYMBOL_SRC(l);
  if(STRCMP(name,SEMA_GET_SRC(sema))){
	set_add_obj(set,(string_t)car(l));
  }

  return TRUE;
}

bool_t check_integer(sema_t*sema,list_t *lst){

#ifdef __DEBUG__
  printf("check_integer\n");
#endif

  return TRUE;
}
