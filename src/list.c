// 
//
//  Copyright 2019 Shota Hisai. Released under the MIT license.
//
//
#include <stdio.h> 
#include <string.h>
#include "list.h" 
#include "mem.h"
#include "common.h"
#include "error.h"

list_t *g_lst = NULL;

static list_t *create_list();
static void *car_obj(list_t *lst);

list_t *make_null(){
  
  if(g_lst == NULL){
    g_lst = create_list();
    g_lst->next = g_lst;
    g_lst->type = NULL_LIST;
  }

  return g_lst;
}

void *car(list_t *lst){
  return car_obj(lst);
}

list_t *cdr(list_t *lst){
  return lst->next;
}

list_t *cons(list_t *lst,void *obj){

  list_t *new_lst;
  
  new_lst = create_list();
  new_lst->type = OBJECT;
  new_lst->next = lst;
  new_lst->obj.obj = obj;
  
  return new_lst;
}

static list_t *create_list(){

  list_t *lst;

  lst = mem(sizeof(list_t));
  lst->next = NULL;
  lst->line_no = 0;
  
  return lst;
}

static void *car_obj(list_t *lst){

  list_type_t type;
  void *obj;

  type = LIST_GET_TYPE(lst);
  switch(type){
  case INTEGER:
    obj = &(lst->obj.integer.num);
    break;
  case NULL_LIST:
    obj = make_null();
    break;
  case SYMBOL:
    obj = lst->obj.symbol.text;
    break;
  case LIST:
    obj = lst->obj.lst;
    break;
  case STRING:
    obj = lst->obj.string.text;
    break;
  case DECIMAL:
	obj = lst->obj.float_t.fval;
    break;
  case OBJECT:
    obj = lst->obj.obj;
    break;
  case CHARACTER:
	obj = &lst->obj.charc.ch;
	break;
  default:
    error_no_info("Unknonw list type %d\n",type);
    exit(1);
    break;
  }

  return obj; 
}

list_t *add_number(list_t *lst,int num){

  list_t *new_lst;
  
  new_lst = create_list();
  new_lst->type = INTEGER;
  new_lst->next = lst;
  new_lst->obj.integer.num = num;
  
  return new_lst;
}

list_t *add_float(list_t *lst,string_t fval){

  list_t *new_lst;
  
  new_lst = create_list();
  new_lst->type = DECIMAL;
  new_lst->next = lst;
  new_lst->obj.float_t.fval = fval;
  
  return new_lst;
}

list_t *add_symbol(list_t *lst,char *text){

  list_t *new_lst;

  new_lst = create_list();
  new_lst->type = SYMBOL;
  new_lst->next = lst;
  new_lst->obj.symbol.text = text;
  new_lst->obj.symbol.offset = 0;
  new_lst->obj.symbol.len = STRLEN(text);
  new_lst->obj.symbol.src = NULL;
  new_lst->obj.symbol.lineno = 0;

  return new_lst;
}

list_t *add_char(list_t *lst,char *text){

  list_t *new_lst;

  new_lst = create_list();
  new_lst->type = CHARACTER;
  new_lst->next = lst;
  new_lst->obj.charc.ch = *(text + 1);

  return new_lst;
}

list_t *add_string(list_t *lst,char *text){

  list_t *new_lst;

  new_lst = create_list();
  new_lst->type = STRING;
  new_lst->next = lst;
  new_lst->obj.string.text = text;
  
  return new_lst;
}

list_t *add_list(list_t *parent,list_t *child){
  
  list_t *new_lst;
  
  new_lst = create_list();
  new_lst->type = LIST;
  new_lst->next = parent;
  new_lst->obj.lst = child;

  return new_lst; 
}

list_t *add(list_t *lst1,int index,list_t *lst2){

  list_t *head;
  list_t *p;
  int i;
  int cnt;

  cnt = length_of_list(lst1);
  if(cnt < index){
    return make_null();
  }
  
  for(i = 0, p = lst1; i < index && IS_NOT_NULL_LIST(p); i++,p = cdr(p)){
    ;
  }

  if(IS_NULL_LIST(p)){
    head = lst2;
  } else {
    list_t *tmp = cdr(p);
    p->next = lst2;
    head = concat(lst1,tmp);
  }

  return head;
}

list_t *concat(list_t *head,list_t *tail){
  
  list_t *lst;
  
  if((!head) || IS_NULL_LIST(head)){
    return tail;
  }
  
  if((!tail) || IS_NULL_LIST(tail)){
    return head;
  }
  
  if(head == tail){
    return head;
  }
  
  lst = head;
  while(IS_NOT_NULL_LIST(cdr(lst))){
    lst = cdr(lst);
  }

  lst->next = tail;

  return head;
}

int length_of_list(list_t *lst){

  list_t *p;
  int cnt;

  cnt = 0;
  p = lst;

  while(IS_NOT_NULL_LIST(p)){
    p = cdr(p);
    cnt++;
  }

  return cnt;
}

list_t *reverse(list_t *lst){

  list_t *pre;
  list_t *cur;
  list_t *next;

  pre = make_null();
  cur = lst;
  while(IS_NOT_NULL_LIST(cur)){
    next = cdr(cur);
    LIST_SET_NEXT(cur,pre);
    pre = cur;
    cur = next;
  }

  return pre;
}

list_t *tail(list_t *lst){

  list_t *p;
  
  p = lst;
  while(IS_NOT_NULL_LIST(cdr(p))){
    p = cdr(p);
  }
  
  return p;
}

list_t *get_at(list_t *lst,int index){

  list_t *p;
  int i;
  
  p = lst;
  i = 0;
  for(i = 0, p = lst; i < index; i++,p = cdr(p)){
    ;
  }

  return p;
}
