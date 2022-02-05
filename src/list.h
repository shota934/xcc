// 
//
//  Copyright 2019 Shota Hisai. Released under the MIT license.
//
//
#ifndef __INCLUDE__LIST__ 
#define __INCLUDE__LIST__
#include "type.h"
#include "lex.h"

#define LIST_SET_OPERATOR_TYPE(l,t) l->obj.symbol.type = t
#define LIST_GET_OPERATOR_TYPE(l) l->obj.symbol.type

#define LIST_GET_SYMBOL_OFFSET(l) l->obj.symbol.offset
#define LIST_SET_SYMBOL_OFFSET(l,o) l->obj.symbol.offset = o

#define LIST_GET_SYMBOL_LINE_NO(l) l->obj.symbol.lineno
#define LIST_GET_SYMBOL_SRC(l)     l->obj.symbol.src

enum list_type_t{
  LIST,
  INTEGER,
  DECIMAL,
  SYMBOL,
  CHARACTER,
  STRING,
  NULL_LIST,
  MACRO,
  OBJECT
};

struct list_t {
  list_type_t type;
  list_t *next;
  int line_no;
  
  union {
    struct {
      int num;
    } integer;
    
    struct {
      string_t text;
      type_t type;
	  int offset;
	  string_t src;
	  int len;
	  int lineno;
    } symbol;

    struct {
      char *text;
      int len;
    } string;

    struct {
      char ch;
    } charc;

    struct {
	  string_t fval;
    } float_t;
    
    void *obj;
    list_t *lst;

  } obj;
};

#define LIST_GET_TYPE(l) l->type
#define LIST_SET_NEXT(l,n) l->next = n
#define IS_NULL_LIST(l) (l->type == NULL_LIST)
#define IS_NOT_NULL_LIST(l) (l->type != NULL_LIST)
#define IS_LIST(l) (l->type == LIST)
#define IS_NOT_LIST(l) (l->type != LIST)
#define IS_SYMBOL(l) (l->type == SYMBOL)
#define IS_INTEGER(l) (l->type == INTEGER)
#define IS_DECIMAL(l) (l->type == DECIMAL)
#define IS_OBJECT(l)  (l->type == OBJECT)

#define CDR(l) l->next
#define CAR(l) car(l)


list_t *make_null();
void *car(list_t *lst);
list_t *cdr(list_t *lst);
list_t *cons(list_t *lst,void *obj);
list_t *concat(list_t *head,list_t *tail);
list_t *add_number(list_t *lst,int num);
list_t *add_float(list_t *lst,string_t fval);
list_t *add_string(list_t *lst,char *text);
list_t *add_symbol(list_t *lst,char *text);
list_t *add_char(list_t *lst,char *text);
list_t *add_macro(list_t *lst,list_t *parm,list_t *body);
list_t *add_list(list_t *parent,list_t *child);
list_t *add_object(list_t *lst1, list_t *lst2);
list_t *add(list_t *lst1,int index,list_t *lst2);
int length_of_list(list_t *lst);
list_t *reverse(list_t *lst);
list_t *tail(list_t *lst);
list_t *get_at(list_t *lst,int index);
list_t *copy_list(list_t *lst) ;

#endif
