// 
//
//  Copyright 2019 Shota Hisai. Released under the MIT license.
//
//
#include <stdio.h>
#include <stdarg.h>
#include "error.h"

#define LABEL "\e[31m%s\e[0m: "

static void print_msg(string_t fmt,va_list args);

void error(int line,string_t name,string_t msg,...){
  
  va_list args;
  
#ifdef __DEBUG__
  printf("error\n");
#endif

  va_start(args,msg);
  fprintf(stderr,LABEL,"error");
  fprintf(stderr,"%s : %d : ",name,line);
  print_msg(msg,args);
  fprintf(stderr,"\n");
  va_end(args);
  
  return;
}

void warn(int line,string_t name,string_t msg,...){

  va_list args;

#ifdef __DEBUG__
  printf("warn\n");
#endif

  va_start(args,msg);
  fprintf(stderr,LABEL,"warning");
  fprintf(stderr,"%s : %d : ",name,line);
  print_msg(msg,args);
  fprintf(stderr,"\n");
  va_end(args);

  return;
}

void error_no_info(string_t msg,...){

  va_list args;

#ifdef __DEBUG__
  printf("error_no_info\n");
#endif

  va_start(args,msg);
  fprintf(stderr,"error : ");
  print_msg(msg,args);
  fprintf(stderr,"\n");
  va_end(args);
  
  return;
}

static void print_msg(string_t fmt,va_list args){
  
#ifdef __DEBUG__
  printf("print_msg\n");
#endif

  vfprintf(stderr, fmt,args);

  return;
}
