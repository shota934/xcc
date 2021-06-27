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
    fprintf(stdout,"%d ",(*(int *)CAR(lst)));
    break;
  case DECIMAL:
    fprintf(stdout,"%f ",(*(double *)CAR(lst)));
    break;
  case LIST:
    fprintf(stdout,"( ");
    dump_body((list_t *)CAR(lst));
    fprintf(stdout," )");
    break;
  case SYMBOL:
    fprintf(stdout,"%s ",(char *)CAR(lst));
    break;
  case STRING:
    fprintf(stdout,"%s ",(char *)CAR(lst));
    break;
  case CHARACTER:
    fprintf(stdout,"'%c' ",*(char *)CAR(lst));
  default:
    break;
  }
  dump_body(cdr(lst));

  return;
}
