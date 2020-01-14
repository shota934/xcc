// 
//
//  Copyright 2019 Shota Hisai. Released under the MIT license.
//
//
#ifndef __INCLUDE__FILE__
#define __INCLUDE__FILE__
#include <stdio.h>
#include "common.h"

struct file_t {
  FILE *fp;
  char *name;
  char *mode;  
};

#define FILE_GET_NAME(f) f->name
#define FILE_GET_MODE(f) f->mode
#define FILE_SET_NAME(f,n) f->name = n
#define FILE_SET_MODE(f,m) f->mode = m
#define FILE_SET_FP(f,fp) f->fp = fp
#define FILE_GET_FP(f) f->fp

file_t *file_create();
int file_open(file_t *file);
char *file_read_as_string(file_t *file);
int file_get_size(file_t *file);
void file_close(file_t *file);

#endif
