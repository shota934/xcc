#ifndef __INCLUDE__SEMA__
#define __INCLUDE__SEMA__
#include "type.h"
#include "list.h"
#include "set.h"

#define SEMA_SET_SET(sema,s)  sema->set = set
#define SEMA_GET_SET(sema)    sema->set
#define SEMA_SET_SRC(sema,s)  sema-src = s
#define SEMA_GET_SRC(sema)    sema->src

struct sema_t{
  set_t *set;
  string_t src;
};

sema_t *create_sema(string_t name);
bool_t check(sema_t *sema,list_t *lst);

#endif
