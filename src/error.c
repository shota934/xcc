// 
//
//  Copyright 2019 Shota Hisai. Released under the MIT license.
//
//
#include <stdio.h>
#include <stdarg.h>
#include "error.h"

static void print_error(string_t msg,va_list arg);

void error(int line,string_t name,string_t msg,...){
  
  va_list args;
  
#ifdef __DEBUG__
  printf("error\n");
#endif
  
  va_start(args,msg);
  fprintf(stderr,"%s : %d",name,line);
  fprintf(stderr,"ERROR");
  print_error(msg,args);
  fprintf(stderr,"\n");
  va_end(args);
  
  return;
}

void error_no_info(string_t msg,...){

  va_list args;

#ifdef __DEBUG__
  printf("error_no_lineno\n");
#endif

  va_start(args,msg);
  fprintf(stderr,"ERROR : ");
  print_error(msg,args);
  fprintf(stderr,"\n");
  va_end(args);
  
  return;
}

static void print_error(string_t msg,va_list arg){

#ifdef __DEBUG__
  printf("print_error\n");
#endif

  
  
  return;
}
