// 
//
//  Copyright 2019 Shota Hisai. Released under the MIT license.
//
//
#include <stdio.h>
#include <string.h>
#include "file.h"
#include "common.h"
#include "mem.h"

file_t *file_create(){
  return mem(sizeof(file_t));
}

int file_open(file_t *file){
  
  FILE *fp;
  
  fp = fopen(FILE_GET_NAME(file),FILE_GET_MODE(file));
  if(!fp){
    return FALSE;
  }

  FILE_SET_FP(file,fp);
  
  return TRUE;
}

char *file_read_as_string(file_t *file){

  char *str;
  int len;

  len = file_get_size(file);
  str = mem(sizeof(char) * len + NULL_LEN);
  memset(str,INIT_VALUE,len + NULL_LEN);
  fread(str,sizeof(char),len,FILE_GET_FP(file));
  
  return str;
}


int file_get_size(file_t *file){

  int size;

  fseek(FILE_GET_FP(file),0, SEEK_SET);
  fseek(FILE_GET_FP(file),0,SEEK_END);
  size = ftell(FILE_GET_FP(file));
  fseek(FILE_GET_FP(file),0, SEEK_SET);
  
  return size;
}

void file_close(file_t *file){
  
  fclose(FILE_GET_FP(file));

  return;
}
