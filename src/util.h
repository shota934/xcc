// 
//
//  Copyright 2019 Shota Hisai. Released under the MIT license.
//
//
#ifndef __INCLUDE__UTIL__
#define __INCLUDE__UTIL__
#include <string.h>
#include "type.h"
#include "symbol.h"

#define TABLE_SIZE 256

char* make_label();
int hash(char *str);
bool_t is_inttype(type_t type);
bool_t is_decimaltype(type_t type);
bool_t is_function(type_t type);
type_t util_get_type(list_t *lst);
bool_t is_float(symbol_t *sym);
bool_t is_struct(symbol_t *sym);
bool_t is_structtype(list_t *lst);
bool_t is_compund_type(list_t *lst);
bool_t is_pointer(list_t *lst);
bool_t is_address(list_t *lst);
bool_t is_deref(list_t *lst);
bool_t is_nedd_cast(list_t *lst);
bool_t is_function_pointer(list_t *lst);
bool_t is_array(list_t *lst);
kind_t select_type(list_t *lst);
type_t gen_type(list_t *lst);
int calc_align(int n,int m);
bool_t is_static(list_t *lst);
bool_t is_extern(list_t *lst);
bool_t is_typdef(list_t *lst);
int convert_hex_to_int(char *hex);
bool_t is_var_len_args(list_t *lst);
list_t *get_def_type_lst(list_t *lst);
bool_t is_var_args(list_t *lst);
bool_t is_compound_type(list_t *lst);
bool_t is_compound_alloc_type(list_t *lst);

#endif
