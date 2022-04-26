// 
//
//  Copyright 2019 Shota Hisai. Released under the MIT license.
//
//
#include <stdio.h>
#include "dump.h"

static void dump_body(list_t *lst);

void dump_ast(list_t *lst){

  if(lst){
    dump_body(lst);
    fprintf(stdout,"\n");
  }

  return;
}

static void dump_body(list_t *lst){
  
  switch(LIST_GET_TYPE(lst)){
  case NULL_LIST:
    return;
    break;
  case INTEGER:
    fprintf(stdout,"%d ",(*(int *)car(lst)));
    break;
  case DECIMAL:
    fprintf(stdout,"%s ",(string_t)car(lst));
    break;
  case LIST:
    fprintf(stdout,"( ");
    dump_body((list_t *)car(lst));
    fprintf(stdout," )");
    break;
  case SYMBOL:
    fprintf(stdout,"%s ",(char *)car(lst));
    break;
  case STRING:
    fprintf(stdout,"%s ",(char *)car(lst));
    break;
  case CHARACTER:
    fprintf(stdout,"'%c' ",*(char *)car(lst));
  default:
    break;
  }
  dump_body(cdr(lst));

  return;
}
