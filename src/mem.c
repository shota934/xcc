// 
//
//  Copyright 2019 Shota Hisai. Released under the MIT license.
//
//
#include <stdlib.h>
#include <string.h>
#include "mem.h"
#include "common.h"
#include "error.h"

void *mem(int size){
  
  void *obj;
  
  obj = (void *)malloc(size);
  memset(obj,INIT_VALUE,size);
  
  return obj;
}

void *remem(void *obj,int size){

  void *re_obj;
  
  re_obj = realloc(obj,size);
  if(!re_obj){
    error_no_info("Could not allocate memory.");
    exit(1);
  }
  
  return re_obj;
}

void fre(void *obj){

  if(obj){
    free(obj);
  }

  return;
}
