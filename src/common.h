// 
//
//  Copyright 2019 Shota Hisai. Released under the MIT license.
//
//
#ifndef __INCLUDE__COMMON__
#define __INCLUDE__COMMON__
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "error.h"
#include "dump.h"

#define __LINUX__
//#define __DEBUG__
//#define __CPP__DEBUG__

#define XCC_VERSION "0.1"

#define TRUE !0
#define FALSE 0
#define NULL_CHAR '\0'
#define FILE_OPEN_MODE "rb"
#define FILE_WRITE_MODE "w"
#define OUTPUT_FILE_EXTENSION "s"
#define INIT_VALUE     0x00
#define NULL_LEN       0x01
#define TABLE_SIZE 256
#define BUF_SIZE   256
#define BOOL_TRUE_VAL "true"
#define BOOL_FALSE_VAL "false"
#define FOR_CONDITION_EXPR "<="
#define NIL "NIL"
#define ERROR_FILE          stderr
#define DEFAULT_OUTPUT_FILE "tmp.s"
#define END_OF_CHAR 0x00
#define TRUE_OBJ            "1"

#define INCLUDE_PATH             "/usr/include/"
#define INCLUDE_LINUX_PATH       "/usr/include/linux/"
#define INCLUDE_GNU_LINUX_PATH   "/usr/include/x86_64-linux-gnu/"
#define INCLUDE_XCC              "include/"

#define STRCMP(s1,s2) (strcmp(s1,s2) == 0)
#define STRLEN(s) (int)strlen(s);
#define EQ_NAME(s1,s2) (strcmp(s1,s2) == 0)

#define CONV_STR_INT(n) (int)atoi(n)
#define MEM_SET(s,l)     memset(s,INIT_VALUE,l)
#define MEM_CPY(d,s,l)   memcpy(d,s,l)

#ifdef __DEBUG__
#define DUMP_AST(a) dump_ast(a)
#else
#define DUMP_AST(a)
#endif

#endif
