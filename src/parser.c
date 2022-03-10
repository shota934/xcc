// 
//
//  Copyright 2019 Shota Hisai. Released under the MIT license.
//
//
#include <stdio.h> 
#include <stdlib.h>
#include "parser.h"
#include "mem.h"
#include "error.h"
#include "common.h"
#include "env.h"
//----
#include "dump.h"
//----

#define ARRAY         "[]"
#define UNDEFINED_SIZE "undefined-size"
#define INIT_LIST    "init-list"
#define FUNC          "FUNC"
#define FUNC_CALL     "func-call"
#define FUNC_DEF      "func-def"
#define FUNC_DECL     "func-decl"
#define COMP_DEF      "comp-def"
#define STRUCT_TYPE   "struct"
#define NO_NAME       "no-name"
#define UNION_TYPE    "union"
#define ENUM_TYPE     "enum"
#define DECL_VAR      "decl-var"
#define TYPE_DEF      "typedef"
#define CAST          "cast"
#define TYPE          "type"
#define BLOCK         "block"
#define BUILTIN_FUNC  "builtin-func"

#define STATIC          "static"
#define REGISTER        "register"
#define EXTERNAL        "extern"
#define TYPEDEF         "typedef"
#define INLINE          "inline"
#define LABEL           "label"


#define IS_CHAR(t)     (TOKEN_GET_TYPE(t) == TOKEN_CHARACTER)
#define IS_SWITCH(t)   (TOKEN_GET_TYPE(t) == TOKEN_SWITCH)
#define IS_CASE(t)     (TOKEN_GET_TYPE(t) == TOKEN_CASE)
#define IS_DEFAULT(t)  (TOKEN_GET_TYPE(t) == TOKEN_DEFAULT)
#define IS_NUMBER(t)   (TOKEN_GET_TYPE(t) == TOKEN_NUMBER)
#define IS_FLOAT(t)    (TOKEN_GET_TYPE(t) == TOKEN_FLOAT) \
                       || (TOKEN_GET_TYPE(t) == TOKEN_DOUBLE)
#define IS_HEX(t)      (TOKEN_GET_TYPE(t) == TOKEN_HEX)
#define IS_LETTER(t)   (TOKEN_GET_TYPE(t) == TOKEN_LETTER)
#define IS_ASSIGN(t)   (TOKEN_GET_TYPE(t) == TOKEN_ASSIGN)
#define IS_LPAREN(t)   (TOKEN_GET_TYPE(t) == TOKEN_LPAREN)
#define IS_RPAREN(t)   (TOKEN_GET_TYPE(t) == TOKEN_RPAREN)
#define IS_LBRACE(t)   (TOKEN_GET_TYPE(t) == TOKEN_LBRACE)
#define IS_RBRACE(t)   (TOKEN_GET_TYPE(t) == TOKEN_RBRACE)
#define IS_EQUAL(t)    (TOKEN_GET_TYPE(t) == TOKEN_EQUAL)
#define IS_SEMI_COLON(t) (TOKEN_GET_TYPE(t) == TOKEN_SEMI_COLON)
#define IS_COMMA(t)    (TOKEN_GET_TYPE(t) == TOKEN_COMMA)
#define IS_LBRACKET(t) (TOKEN_GET_TYPE(t) == TOKEN_LBRACKET)
#define IS_RBRACKET(t) (TOKEN_GET_TYPE(t) == TOKEN_RBRACKET)
#define IS_RET(t)      (TOKEN_GET_TYPE(t) == TOKEN_RET)
#define IS_IF(t)      (TOKEN_GET_TYPE(t) == TOKEN_IF)
#define IS_ELSE(t)    (TOKEN_GET_TYPE(t) == TOKEN_ELSE)
#define IS_WHILE(t)    (TOKEN_GET_TYPE(t) == TOKEN_WHILE)
#define IS_STRING(t)  (TOKEN_GET_TYPE(t) == TOKEN_STRING)
#define IS_FOR(t)    (TOKEN_GET_TYPE(t) == TOKEN_FOR)
#define IS_ASTERISK(t) (TOKEN_GET_TYPE(t) == TOKEN_MUL)
#define IS_MUL(t)  (TOKEN_GET_TYPE(t) == TOKEN_MUL)
#define IS_DIV(t)  (TOKEN_GET_TYPE(t) == TOKEN_DIV)
#define IS_MOD(t)  (TOKEN_GET_TYPE(t) == TOKEN_MOD)
#define IS_IF(t)   (TOKEN_GET_TYPE(t) == TOKEN_IF)
#define IS_AND(t)  (TOKEN_GET_TYPE(t) == TOKEN_AND)
#define IS_OR(t)   (TOKEN_GET_TYPE(t) == TOKEN_OR)
#define IS_XOR(t)  (TOKEN_GET_TYPE(t) == TOKEN_XOR)
#define IS_NOT(t)  (TOKEN_GET_TYPE(t) == TOKEN_NOT)
#define IS_TILDE(t) (TOKEN_GET_TYPE(t) == TOKEN_TILDE)
#define IS_AND_BOOL(t) (TOKEN_GET_TYPE(t) == TOKEN_BOOL_AND)
#define IS_OR_BOOL(t)  (TOKEN_GET_TYPE(t) == TOKEN_BOOL_OR)
#define IS_POINTER(t) (TOKEN_GET_TYPE(t) == TOKEN_MUL)
#define IS_TYPE(t)     (TOKEN_GET_TYPE(t) == TOKEN_INT)         \
                       || (TOKEN_GET_TYPE(t) == TOKEN_VOID)     \
                       || (TOKEN_GET_TYPE(t) == TOKEN_CHAR)     \
                       || (TOKEN_GET_TYPE(t) == TOKEN_FLOAT)    \
                       || (TOKEN_GET_TYPE(t) == TOKEN_SHORT)    \
                       || (TOKEN_GET_TYPE(t) == TOKEN_LONG)     \
                       || (TOKEN_GET_TYPE(t) == TOKEN_DOUBLE)   \
                       || (TOKEN_GET_TYPE(t) == TOKEN_SIGNED)   \
                       || (TOKEN_GET_TYPE(t) == TOKEN_UNSIGNED) \

#define IS_END_T(t)    (TOKEN_GET_TYPE(t) == TOKEN_EOT)
#define IS_COP(t)      ((TOKEN_GET_TYPE(t) == TOKEN_LESS) || \
			(TOKEN_GET_TYPE(t) == TOKEN_GREATER) || \
			(TOKEN_GET_TYPE(t) == TOKEN_LESS_EQUAL) || \
			(TOKEN_GET_TYPE(t) == TOKEN_GREATER_EQUAL) || \
			(TOKEN_GET_TYPE(t) == TOKEN_EQUAL)         || \
                        (TOKEN_GET_TYPE(t) == TOKEN_NOT_EQUAL))

#define IS_CREMENT_OPERATOR(t) ((TOKEN_GET_TYPE(t) == TOKEN_INCREMENT_BY_ONE) || \
				(TOKEN_GET_TYPE(t) == TOKEN_DECREMENT_BY_ONE))
#define IS_CREMENT_ASSING_OPERATOR(t)  ((TOKEN_GET_TYPE(t) == TOKEN_INCREMENT) || \
					(TOKEN_GET_TYPE(t) == TOKEN_DECREMENT))
#define IS_SIZEOF(t)      (TOKEN_GET_TYPE(t) == TOKEN_SIZE_OF)
#define IS_STRUCT(t)      (TOKEN_GET_TYPE(t) == TOKEN_STRUCT)
#define IS_TYPEDEF(t)     (TOKEN_GET_TYPE(t) == TOKEN_TYPEDEF)
#define IS_DOT(t)         (TOKEN_GET_TYPE(t) == TOKEN_DOT)
#define IS_BREAK(t)       (TOKEN_GET_TYPE(t) == TOKEN_BREAK)
#define IS_CONTINUE(t)    (TOKEN_GET_TYPE(t) == TOKEN_CONTINUE)
#define IS_GOTO(t)         (TOKEN_GET_TYPE(t) == TOKEN_GOTO)
#define IS_COLON(t)       (TOKEN_GET_TYPE(t) == TOKEN_COLON)
#define IS_ARROW_OP(t)    (TOKEN_GET_TYPE(t) == TOKEN_ARROW_OP)
#define IS_ENUM(t)        (TOKEN_GET_TYPE(t) == TOKEN_ENUM)
#define IS_STATIC(t)      (TOKEN_GET_TYPE(t) == TOKEN_STATIC)
#define IS_EXTERN(t)      (TOKEN_GET_TYPE(t) == TOKEN_EXTERN)
#define IS_CONST(t)       (TOKEN_GET_TYPE(t) == TOKEN_CONST)
#define IS_REGISTER(t)    (TOKEN_GET_TYPE(t) == TOKEN_REGISTER)
#define IS_AUTO(t)        (TOKEN_GET_TYPE(t) == TOKEN_AUTO)
#define IS_UNION(t)       (TOKEN_GET_TYPE(t) == TOKEN_UNION)
#define IS_VOLATILE(t)    (TOKEN_GET_TYPE(t) == TOKEN_VOLATILE)
#define IS_RESTRICT(t)    (TOKEN_GET_TYPE(t) == TOKEN_RESTRICT)
#define IS_VAR_ARG_LIST(t) (TOKEN_GET_TYPE(t) ==  TOKEN_ARGMENT_LIST)
#define IS_INLINE(t)      (TOKEN_GET_TYPE(t) == TOKEN_INLINE)

#define IS_DEFINED(t)    (TOKEN_GET_TYPE(t) == TOKEN_DEFINED)
#define IS_BIT_SHIFT(t)  ((TOKEN_GET_TYPE(t) == TOKEN_BIT_LEFT_SHIFT) || (TOKEN_GET_TYPE(t) == TOKEN_BIT_RIGHT_SHIFT))


#define IS_QUESTION(t)   (TOKEN_GET_TYPE(t) == TOKEN_QUESTION)
#define IS_ATTRIBUTE(t)  (TOKEN_GET_TYPE(t) == TOKEN_ATTRIBUTE)
#define IS_DO(t)  (TOKEN_GET_TYPE(t) == TOKEN_DO)
#define IS_ASM(t) (TOKEN_GET_TYPE(t) == TOKEN_ASM)

static list_t *parser_parse_def(parser_t *parser);
static list_t *parser_parse_storage_class_dcl(parser_t *parser);

static list_t *parser_parse_type(parser_t *parser,int flag_of_typedef,int flag_of_type);
static list_t *parser_parse_ext_dcls(parser_t *parser);
static list_t *parser_parse_dclation(parser_t *parser);
static list_t *parser_parse_declaration_specifiers(parser_t *parser);
static list_t *parser_parse_declvar(list_t *type,list_t *lst,list_t *init_lst);
static list_t *parser_parse_func(parser_t *parser,list_t *type,list_t *lst,list_t *q);
static list_t *parser_parse_enum(parser_t *parser);
static list_t *parser_parse_enum_elements(parser_t *parser);
static list_t *parser_parse_enum_element(parser_t *parser);
static list_t *parser_parse_typedef_union_or_struct(parser_t *parser,list_t *lst,bool_t struct_type);
static list_t *parser_parse_union_or_struct(parser_t *parser,bool_t struct_type);
static list_t *parser_parse_def_struct(parser_t *parser);
static list_t *parser_parse_mems(parser_t *parser,list_t *type_lst);
static list_t *parser_parse_members(parser_t *parser);
static list_t *parser_parse_member(parser_t *parser);
static list_t *parser_parse_funcargs(parser_t *parser,int cnt);
static list_t *parser_parse_funcarg(parser_t *parser);
static list_t *parser_parse_attribute(parser_t *parser,list_t *lst);
static list_t *parser_parse_defvars(parser_t *parser);

static list_t *parser_constant_text(parser_t *parser);
static list_t *parser_parse_defined(parser_t *parser);

static list_t *parser_parse_stmts(parser_t *parser);
static list_t *parser_parse_stmt(parser_t *parser);
static list_t *parser_parse_array(parser_t *parser);
static list_t *parser_parse_stmt_expr(parser_t *parser);
static list_t *parser_parse_vars(parser_t *parser);
static list_t *parser_parse_var(parser_t *parser);
static list_t *parser_parse_while(parser_t *parser);
static list_t *parser_parse_do_while(parser_t *parser);
static list_t *parser_parse_asm(parser_t *parser);
static list_t *parser_parse_switch(parser_t *parser);
static list_t *parser_parse_switch_cases(parser_t *parser);
static list_t *parser_parse_switch_case(parser_t *parser);
static list_t *parser_parse_for(parser_t *parser);
static list_t *parser_parse_cond_expr(parser_t *parser);
static list_t *parser_parse_lexpr(parser_t *parser);
static list_t *parser_parse_expr(parser_t *parser);
static list_t *parser_parse_cexpr(parser_t *parser);
static list_t *parser_parse_bexpr(parser_t *parser);
static list_t *parser_parse_term(parser_t *parser);
static list_t *parser_parse_factor(parser_t *parser);
static list_t *parser_parse_if(parser_t *parser);
static list_t *parser_parse_goto(parser_t *parser);
static list_t *parser_baracket_expr(parser_t *parser);
static list_t *parser_parse_else(parser_t *parser);
static list_t *parser_parse_sub(parser_t *parser,list_t *lst);
static list_t *parser_parse_sub_op(parser_t *parser,list_t *lst);
static list_t *parser_parse_struct_access(parser_t *parser,list_t *lst,token_t *t);
static list_t *parser_parse_sub_member(parser_t *parser);
static list_t *parser_parse_func_call(parser_t *parser);
static list_t *parser_parse_func_args(parser_t *parser);
static list_t *parser_parse_func_arg(parser_t *parser);
static list_t *parser_parse_sizeoftype(parser_t *parser);
static list_t *parser_parse_cast(parser_t *parser,list_t *cast_type);
static list_t *parser_parse_unary_expr(parser_t *parser);
static list_t *parser_parse_pointer(parser_t *parser);
static list_t *parser_parse_type_qualifier_list(parser_t *parser);

static list_t *parser_parse_dcls(parser_t *parser,list_t *type_lst);
static list_t *parser_parse_dcl(parser_t *parser);

static list_t *parser_parse_dirdcl(parser_t *parser);
static list_t *parser_parse_subdirdcl(parser_t *parser);
static list_t *parser_parse_dirdclvar(parser_t *parser,list_t *var);

static list_t *parser_parse_initializer(parser_t *parser);
static list_t *parser_parse_array_initialyzer(parser_t *parser);
static list_t *parser_parse_initialye_value(parser_t *parser);
static list_t *parser_parse_initialyze_value(parser_t *parser);

static list_t *parser_parse_block_items(parser_t *parser);
static list_t *parser_parse_block_item(parser_t *parser);
static list_t *parser_parse_block(parser_t *parser);

static list_t *parser_parse_ternary(parser_t *parser);

static bool_t is_type_qualifier(token_t *t);

static int is_dclvar(list_t *lst);
static int is_dcl(list_t *lst);
static int is_typedef_name(parser_t *parser,token_t *token);
static bool_t is_buildin_func(list_t *lst);
static void record_obj_set(parser_t *parser,list_t *lst);
static bool_t has_array(list_t *lst);
static bool_t is_func_decl(list_t *lst);
static bool_t is_compound_def_type(list_t *lst);
static bool_t is_typedef(list_t *lst);
static void make_func(list_t *lst);
static list_t *get_ret_lst(list_t *lst);

static list_t *create_func(parser_t *parser,list_t *lst);
static list_t *create_ret_lst(parser_t *parser,list_t *lst);
static list_t *make_symbol(list_t *lst,token_t *t);
static list_t *make_keyword(list_t *lst,string_t text);
static string_t make_no_name(parser_t *parser);


parser_t *parser_create(){
  
  parser_t *parser = mem(sizeof(parser_t));
  parser_init(parser);
  
  return parser;
}

void parser_init(parser_t *parser){

  parser->var_lst = make_null();
  parser->set = set_create();
  parser->cnt = 0;

  return;
}

list_t *parser_parse(parser_t *parser){

  list_t *lst;
  
  return parser_parse_def(parser);
}

void parser_set_src(parser_t *parser,char *src){
  
  lexer_set_src(PARSER_GET_LEX(parser),src);
  
  return;
}

list_t *parser_constant_texts(parser_t *parser){

  list_t *expr;
  
  #ifdef __DEBUG__
  printf("parser_constant_texts\n");
  #endif
  
  expr = parser_constant_text(parser);
  
  return expr;
}

static list_t *parser_constant_text(parser_t *parser){

  list_t *expr;
  
  #ifdef __DEBUG__
  printf("parser_constant_text\n");
  #endif

  expr = parser_parse_cond_expr(parser);
  
  return expr;
}

static list_t *parser_parse_defined(parser_t *parser){

  list_t *expr;
  token_t *t;
  int flag;
  

  #ifdef __DEBUG__
  printf("parser_parse_defined\n");
  #endif

  flag = FALSE;
  t = lexer_get_token(PARSER_GET_LEX(parser));
  expr = make_null();
  if(IS_LPAREN(t)){
    flag = TRUE;
  } else if(IS_LETTER(t)){
    expr = make_symbol(expr,t);
  }

  if(flag){
    t = lexer_get_token(PARSER_GET_LEX(parser));
    expr = make_symbol(expr,t);
    
    t = lexer_get_token(PARSER_GET_LEX(parser));
    if(!IS_RPAREN(t)){
      
    }
  }
  
  return expr;
}

static list_t *parser_parse_def(parser_t *parser){ 

  list_t *cpp_lst;
  list_t *new_lst;
  
  new_lst = parser_parse_ext_dcls(parser);

  return new_lst;
}

static list_t *parser_parse_enum(parser_t *parser){

  list_t *new_lst;
  token_t *t;

  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(IS_LETTER(t)){
	new_lst = make_symbol(make_null(),t);
  } else {
	new_lst = make_keyword(make_null(),make_no_name(parser));
	lexer_put_token(PARSER_GET_LEX(parser),t);
  }
  
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(IS_LBRACE(t)){
    list_t *elements = parser_parse_enum_elements(parser);
    new_lst = concat(new_lst,add_list(make_null(),elements));
  } else {
	lexer_put_token(PARSER_GET_LEX(parser),t);
  }
  
  return new_lst;
}

static list_t *parser_parse_enum_elements(parser_t *parser){

  list_t *new_lst;
  token_t *t;

  new_lst = parser_parse_enum_element(parser);
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(IS_COMMA(t)){
    list_t *element_lst = parser_parse_enum_elements(parser);
    new_lst = concat(new_lst,element_lst);
  } else if(IS_RBRACE(t)){
    return new_lst;
  }
  
  return new_lst;
}

static list_t *parser_parse_enum_element(parser_t *parser){

  list_t *new_lst;
  token_t *t;

#ifdef __DEBUG__
  printf("parser_parse_enum_element\n");
#endif

  new_lst = parser_parse_cexpr(parser);
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(IS_ASSIGN(t)){
	new_lst = add_list(make_null(),concat(new_lst,parser_parse_stmt_expr(parser)));
  } else if(IS_COMMA(t) || IS_RBRACE(t)){
	lexer_put_token(PARSER_GET_LEX(parser),t);
  } else {
	exit(1);
  }

  return new_lst;
}

static list_t *parser_parse_typedef_union_or_struct(parser_t *parser,list_t *lst,bool_t struct_type){

  list_t *new_lst;

#ifdef __DEBUG__
  printf("parser_parse_typedef_union_or_struct\n");
#endif
  
  if(struct_type){
	new_lst = make_keyword(lst,STRUCT_TYPE);
  } else {
	new_lst = make_keyword(lst,UNION_TYPE);
  }

  return new_lst;
}

static list_t *parser_parse_union_or_struct(parser_t *parser,bool_t struct_type){
  
  list_t *new_lst;
  list_t *l;
  token_t *t;

#ifdef __DEBUG__
  printf("parser_parse_union_or_struct\n");
#endif
  
  new_lst = make_null();
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(IS_LETTER(t)){
    new_lst = make_symbol(new_lst,t);
    t = lexer_get_token(PARSER_GET_LEX(parser));
  } else {
	new_lst = make_keyword(new_lst,make_no_name(parser));
  }

  if(IS_LBRACE(t)){
	l = add_list(make_null(),parser_parse_members(parser));
	if(struct_type){
	  l = make_keyword(l,STRUCT_TYPE);
	} else {
	  l = make_keyword(l,UNION_TYPE);
	}
	l = add_list(make_null(),l);
    new_lst = add_list(make_null(),concat(new_lst,l));
  } else {
    lexer_put_token(PARSER_GET_LEX(parser),t);
  }
  
  return new_lst;
}

static list_t *parser_parse_members(parser_t *parser){

  list_t *new_lst;
  token_t *t;

#ifdef __DEBUG__
  printf("parser_parse_members\n");
#endif
  
  new_lst = parser_parse_member(parser);
  t = lexer_get_token(PARSER_GET_LEX(parser));  
  if(IS_RBRACE(t)){
    return new_lst;
  } else {
    lexer_put_token(PARSER_GET_LEX(parser),t);
    return concat(new_lst,parser_parse_members(parser));
  }
}

static list_t *parser_parse_member(parser_t *parser){
  
  list_t *new_lst;
  list_t *type_lst;
  list_t *quali_type_lst;
  token_t *t;

#ifdef __DEBUG__
  printf("parser_parse_member\n");
#endif
  
  new_lst = make_null();
  quali_type_lst = parser_parse_type_qualifier_list(parser);
  type_lst = parser_parse_type(parser,FALSE,FALSE);
  new_lst = parser_parse_dcl(parser);
  new_lst = parser_parse_func(parser,type_lst,new_lst,quali_type_lst);

  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(IS_COMMA(t)){
	new_lst = add_list(make_null(),new_lst);
	new_lst = concat(new_lst,parser_parse_mems(parser,type_lst));
  } else if(!IS_SEMI_COLON(t)){
    error(TOKEN_GET_LINE_NO(t),TOKEN_GET_NAME(t),"Expected : [%s] but got [%s]\n",";",TOKEN_GET_STR(t));
	exit(1);
  } else {
	new_lst = add_list(make_null(),new_lst);
  }

  return new_lst;
}

static list_t *parser_parse_mems(parser_t *parser,list_t *type_lst){

  list_t *new_lst;
  token_t *t;

#ifdef __DEBUG__
  printf("parser_parse_mems\n");
#endif

  new_lst = parser_parse_dcl(parser);
  t = lexer_get_token(PARSER_GET_LEX(parser));
  new_lst = concat(new_lst,copy_list(type_lst));
  new_lst = add_list(make_null(),new_lst);
  if(IS_COMMA(t)){
	new_lst = concat(new_lst,parser_parse_dcls(parser,type_lst));
  } else if(IS_SEMI_COLON(t)){
	return new_lst;
  } else {
	exit(1);
  }

  return new_lst;
}

static list_t *parser_parse_storage_class_dcl(parser_t *parser){

  list_t *new_lst;
  token_t *t;

#ifdef __DEBUG__
  printf("parser_parse_storage_class_dcl\n");
#endif
  t = lexer_get_token(PARSER_GET_LEX(parser));
  new_lst = make_null();
  if(IS_STATIC(t)){
    new_lst = make_keyword(new_lst,STATIC);
  } else if(IS_EXTERN(t)){
    new_lst = make_keyword(new_lst,EXTERNAL);
  } else if(IS_REGISTER(t)){
    new_lst = make_keyword(new_lst,REGISTER);
  } else if(IS_TYPEDEF(t)){
    new_lst = make_keyword(new_lst,TYPEDEF);
  } else {
    lexer_put_token(PARSER_GET_LEX(parser),t);
  }
  
  return new_lst;
}

static list_t *parser_parse_type(parser_t *parser,int flag_of_typedef,int flag_of_type){

  list_t *new_lst;
  list_t *qu_ty_lst;
  token_t *t;

  new_lst = make_null();
#ifdef __DEBUG__
  printf("parser_parse_type\n");
#endif
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(IS_TYPE(t) || is_typedef_name(parser,t)){
    new_lst = parser_parse_type(parser,flag_of_typedef,flag_of_type);
    new_lst = make_symbol(new_lst,t);
  } else if(IS_CONST(t)){
	new_lst = parser_parse_type(parser,flag_of_typedef,flag_of_type);
    new_lst = make_symbol(new_lst,t);
  } else if(IS_STRUCT(t)){
	new_lst = parser_parse_union_or_struct(parser,TRUE);
	if(flag_of_typedef){
	  new_lst = parser_parse_typedef_union_or_struct(parser,new_lst,TRUE);
	} else {
	  t = lexer_get_token(PARSER_GET_LEX(parser));	
	  if(IS_SEMI_COLON(t) || IS_LETTER(t)){
		if(IS_LIST(new_lst)){
		  new_lst = make_keyword(new_lst,COMP_DEF);
		  new_lst = add_list(make_null(),new_lst);
		} else {
		  new_lst = make_keyword(new_lst,STRUCT_TYPE);
		}
		lexer_put_token(PARSER_GET_LEX(parser),t);
	  }
	}
  } else if (IS_UNION(t)){
	new_lst = parser_parse_union_or_struct(parser,FALSE);
	if(flag_of_typedef){
	  new_lst = parser_parse_typedef_union_or_struct(parser,new_lst,FALSE);
	} else {
	  t = lexer_get_token(PARSER_GET_LEX(parser));
	  if(IS_SEMI_COLON(t) || IS_LETTER(t)){
		if(IS_LIST(new_lst)){
		  new_lst = make_keyword(new_lst,COMP_DEF);
		  new_lst = add_list(make_null(),new_lst);
		} else {
		  new_lst = make_keyword(new_lst,UNION_TYPE);
		}
		lexer_put_token(PARSER_GET_LEX(parser),t);
	  }
	}
  } else if(IS_ENUM(t)){
	  new_lst = parser_parse_enum(parser);
	  if(IS_LIST(cdr(new_lst))){
		new_lst = make_keyword(new_lst,ENUM_TYPE);
		new_lst = add_list(make_null(),new_lst);
	  } else {
		new_lst = make_keyword(new_lst,ENUM_TYPE);
	  }
 } else if(IS_LETTER(t) && set_find_obj(PARSER_GET_SET(parser),TOKEN_GET_STR(t))){
    new_lst = make_symbol(new_lst,t);
  } else if(IS_POINTER(t) && flag_of_type){
	lexer_put_token(PARSER_GET_LEX(parser),t);
	new_lst = parser_parse_pointer(parser);
  } else {
	lexer_put_token(PARSER_GET_LEX(parser),t);
  }

  return new_lst;
}

static list_t *parser_parse_ext_dcls(parser_t *parser){
  
  list_t *new_lst;

#ifdef __DEBUG__
  printf("parser_parse_ext_dcls\n");
#endif
  new_lst = parser_parse_dclation(parser);
  if(IS_NOT_NULL_LIST(new_lst)){
    new_lst = concat(new_lst,parser_parse_ext_dcls(parser));
  }
  
  return new_lst;
}

static list_t *parser_parse_dclation(parser_t *parser){

  list_t *new_lst;
  list_t *sub_lst;
  list_t *type_lst;
  list_t *dcl_lst;
  list_t *quali_type_lst;
  token_t *t;
  list_t *q;
  bool_t flag;

#ifdef __DEBUG__
  printf("parser_parse_dclation\n");
#endif

  dcl_lst = parser_parse_declaration_specifiers(parser);
  if(is_typedef(dcl_lst)){
	flag = TRUE;
  } else {
	flag = FALSE;
  }

  quali_type_lst = parser_parse_type_qualifier_list(parser);
  type_lst = parser_parse_type(parser,flag,FALSE);
  if(IS_NULL_LIST(type_lst)){
    return dcl_lst;
  }

  new_lst = parser_parse_dcl(parser);
  new_lst = parser_parse_func(parser,type_lst,new_lst,quali_type_lst);
  new_lst = concat(dcl_lst,new_lst);
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(IS_LBRACE(t)){
    new_lst = concat(new_lst,add_list(make_null(),parser_parse_block_items(parser)));
    t = lexer_get_token(PARSER_GET_LEX(parser));
    if(!IS_RBRACE(t)){
	  exit(1);
    }
	new_lst = create_func(parser,new_lst);
  } else if(IS_ASSIGN(t)){
	list_t *lst = make_keyword(new_lst,DECL_VAR);
	new_lst = add_list(make_null(),lst);
	new_lst = concat(new_lst,parser_parse_initializer(parser));
	PARSER_SET_VAR_LST(parser,concat(PARSER_GET_VAR_LST(parser),add_list(make_null(),new_lst)));
    new_lst = make_symbol(new_lst,t);
    new_lst = add_list(make_null(),new_lst);
    t = lexer_get_token(PARSER_GET_LEX(parser));    
    if(!IS_SEMI_COLON(t)){
      lexer_put_token(PARSER_GET_LEX(parser),t);
    }
  } else if(IS_SEMI_COLON(t)){
	if(is_func_decl(cdr(new_lst))){
	  make_func(new_lst);
	  create_ret_lst(parser,new_lst);
      new_lst = add_list(make_null(),concat(make_keyword(make_null(),FUNC_DECL),new_lst));
	} else if(is_compound_def_type(new_lst)){
	  new_lst = add_list(make_null(),new_lst);
	} else if(is_typedef(new_lst)){
	  record_obj_set(parser,cdr(new_lst));
	  new_lst = add_list(make_null(),new_lst);
    } else if(!IS_LIST(new_lst)){
	  new_lst = make_keyword(new_lst,DECL_VAR);
	  PARSER_SET_VAR_LST(parser,concat(PARSER_GET_VAR_LST(parser),add_list(make_null(),add_list(make_null(),new_lst))));
      new_lst = add_list(make_null(),new_lst);
    }
  } else if(IS_COMMA(t)){
	new_lst = make_keyword(new_lst,DECL_VAR);
	PARSER_SET_VAR_LST(parser,concat(PARSER_GET_VAR_LST(parser),add_list(make_null(),add_list(make_null(),new_lst))));
	new_lst = concat(add_list(make_null(),new_lst),parser_parse_dcls(parser,type_lst));
  } else if(IS_LPAREN(t) || IS_RPAREN(t)){
	lexer_put_token(PARSER_GET_LEX(parser),t);
  } else {
	error(TOKEN_GET_LINE_NO(t),TOKEN_GET_NAME(t),"expected ';' before '%s' ",TOKEN_GET_STR(t));
	exit(1);
  }
  
  return new_lst;
}

static list_t *parser_parse_declaration_specifiers(parser_t *parser){

  list_t *new_lst;
  token_t *t;

#ifdef __DEBUG__
  printf("parser_parse_declaration_specifiers\n");
#endif

  new_lst = make_null();
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(IS_INLINE(t)){
	new_lst = concat(new_lst,parser_parse_storage_class_dcl(parser));
  } else {
	lexer_put_token(PARSER_GET_LEX(parser),t);
	new_lst = parser_parse_storage_class_dcl(parser);
	if(IS_NOT_NULL_LIST(new_lst)){
	  new_lst = concat(new_lst,parser_parse_declaration_specifiers(parser));
	}
  }

  return new_lst;
}

static list_t *parser_parse_declvar(list_t *type,list_t *lst,list_t *init_lst){

  list_t *new_lst;

  new_lst = make_null();

  return new_lst;
}

static list_t *parser_parse_func(parser_t *parser,list_t *type,list_t *lst,list_t *q){

  list_t *new_lst;

#ifdef __DEBUG__
  printf("parser_parse_func\n");
#endif

  new_lst = concat(lst,type);

  return new_lst;
}

static list_t *parser_parse_funcargs(parser_t *parser,int cnt){

  list_t *new_lst;
  token_t *t;
  
#ifdef __DEBUG__
  printf("parser_parse_funcargs\n");
#endif

  new_lst = make_null();
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(IS_RPAREN(t)){
	return parser_parse_attribute(parser,new_lst);
  } else if(IS_VAR_ARG_LIST(t) && 1 <= cnt){
	new_lst = make_symbol(make_null(),t);
	new_lst = add_symbol(new_lst,make_no_name(parser));
	new_lst = add_list(make_null(),new_lst);
	t = lexer_get_token(PARSER_GET_LEX(parser));
    if(IS_RPAREN(t)){
	  return parser_parse_attribute(parser,new_lst);
    }
  } else {
    lexer_put_token(PARSER_GET_LEX(parser),t);
    new_lst = parser_parse_funcarg(parser);
    t = lexer_get_token(PARSER_GET_LEX(parser));
    if(IS_COMMA(t)){
      new_lst = concat(new_lst,parser_parse_funcargs(parser,cnt + 1));
	} else if(IS_RPAREN(t)){
	  return parser_parse_attribute(parser,new_lst);
    }
  }

  return new_lst;
}

static list_t *parser_parse_funcarg(parser_t *parser){

  list_t *new_lst;
  list_t *type_lst;
  list_t *quali_type_lst;
  token_t *t;
  
#ifdef __DEBUG__
  printf("parser_parse_funcarg\n");
#endif

  quali_type_lst = parser_parse_type_qualifier_list(parser);
  type_lst = parser_parse_type(parser,FALSE,FALSE);
  new_lst = parser_parse_dcl(parser);
  new_lst = parser_parse_func(parser,type_lst,new_lst,quali_type_lst);
  new_lst = add_list(make_null(),new_lst);

  return new_lst;
}


static list_t *parser_parse_attribute(parser_t *parser,list_t *lst){

  token_t *t;
  bool_t flag;

#ifdef __DEBUG__
  printf("parser_parse_attribute\n");
#endif

  flag = FALSE;
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(IS_LBRACE(t)){
	lexer_put_token(PARSER_GET_LEX(parser),t);
	return lst;
  }

  while(!IS_SEMI_COLON(t)){
	if(IS_RPAREN(t) && !flag){
	  break;
	} else if(IS_ATTRIBUTE(t)){
	  flag = TRUE;
	}
	t = lexer_get_token(PARSER_GET_LEX(parser));
  }

  lexer_put_token(PARSER_GET_LEX(parser),t);

  return lst;
}

static list_t *parser_parse_block_items(parser_t *parser){
  
  list_t *new_lst;

#ifdef __DEBUG__
  printf("parser_parse_block_items\n");
#endif  
  new_lst = parser_parse_block_item(parser);
  
  if(IS_NOT_NULL_LIST(new_lst)){
    new_lst = concat(add_list(make_null(),new_lst),parser_parse_block_items(parser));
  }
  
  return new_lst;
}

static list_t *parser_parse_block_item(parser_t *parser){

  list_t *new_lst;

#ifdef __DEBUG__
  printf("parser_parse_block_item\n");
#endif
  new_lst = parser_parse_dclation(parser);
  if(IS_NULL_LIST(new_lst)){
    new_lst = parser_parse_stmt(parser);
  }

  return new_lst;
}

static list_t *parser_parse_block(parser_t *parser){

  list_t *new_lst;
  token_t *t;
#ifdef __DEBUG__
  printf("parser_parse_block\n");
#endif

  new_lst = parser_parse_block_items(parser);
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(!IS_RBRACE(t)){
	exit(1);
  }

  new_lst = add_list(make_null(),new_lst);
  new_lst = make_keyword(new_lst,BLOCK);
  new_lst = add_list(make_null(),new_lst);

  return new_lst;
}

static list_t *parser_parse_ternary(parser_t *parser){

  list_t *new_lst;
  token_t *t;

#ifdef __DEBUG__
  printf("parser_parse_ternary\n");
#endif

  new_lst = make_null();

  new_lst = add_list(make_null(),parser_parse_stmt_expr(parser));
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(IS_COLON(t)){
	new_lst = concat(new_lst,add_list(make_null(),parser_parse_cond_expr(parser)));
  } else {
	exit(1);
  }

  return new_lst;
}

static list_t *parser_parse_type_qualifier_list(parser_t *parser){
  
  token_t *t;
  list_t *new_lst;

#ifdef __DEBUG__
  printf("parser_parse_type_qualifier_list\n");
#endif

  new_lst = make_null();
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(is_type_qualifier(t)){
    list_t *const_lst = make_symbol(make_null(),t);
    new_lst = concat(const_lst,parser_parse_type_qualifier_list(parser));
  } else {
    lexer_put_token(PARSER_GET_LEX(parser),t);
  }
  
  return new_lst;
}

static list_t *parser_parse_stmts(parser_t *parser){

  list_t *new_lst;
  
#ifdef __DEBUG__
  printf("parser_parse_stmts\n");
#endif
  
  new_lst = parser_parse_stmt(parser);
  if(IS_NOT_NULL_LIST(new_lst)){
    new_lst = concat(new_lst,parser_parse_stmts(parser));
  }
  
  return new_lst;
}

static list_t *parser_parse_stmt(parser_t *parser){

  list_t *new_lst;
  token_t *t;
  
#ifdef __DEBUG__
  printf("parser_parse_stmt\n");
#endif
  
  t = lexer_get_token(PARSER_GET_LEX(parser));
  new_lst = make_null();
  if(IS_SEMI_COLON(t)){
	new_lst = add_list(make_null(),new_lst);
	return new_lst;
  } else if(IS_RET(t)){
	new_lst = concat(new_lst,add_list(make_null(),parser_parse_stmt_expr(parser)));
	new_lst = make_symbol(new_lst,t);
	new_lst = add_list(make_null(),new_lst);
  } else if(IS_IF(t)){
	new_lst = make_symbol(new_lst,t);
	new_lst = add_list(make_null(),concat(new_lst,parser_parse_if(parser)));
  } else if(IS_SWITCH(t)){
	new_lst = make_symbol(new_lst,t);
	new_lst = add_list(make_null(),concat(new_lst,parser_parse_switch(parser)));
  } else if(IS_WHILE(t)){
	new_lst = make_symbol(new_lst,t);
	new_lst = add_list(make_null(),concat(new_lst,parser_parse_while(parser)));
  } else if(IS_FOR(t)){
	new_lst = make_symbol(new_lst,t);
	new_lst = add_list(make_null(),concat(new_lst,parser_parse_for(parser)));
  } else if(IS_BREAK(t)){
	new_lst = make_symbol(new_lst,t);
	new_lst = add_list(make_null(),new_lst);
  } else if(IS_CONTINUE(t)){
	new_lst = make_symbol(new_lst,t);
	new_lst = add_list(make_null(),new_lst);
  } else if(IS_GOTO(t)){
	new_lst = make_symbol(new_lst,t);
	new_lst = concat(new_lst,parser_parse_goto(parser));
  } else if(IS_DO(t)){
	new_lst = make_symbol(new_lst,t);
	new_lst = concat(new_lst,parser_parse_do_while(parser));
  } else if(IS_ASM(t)){
	new_lst = make_symbol(new_lst,t);
	new_lst = concat(new_lst,parser_parse_asm(parser));
  } else if(IS_END_T(t)){
	new_lst = make_null();
  } else if(IS_LBRACE(t)){
	new_lst = parser_parse_block(parser);
  } else {
	if(IS_LETTER(t)){
	  token_t *tt = lexer_get_token(PARSER_GET_LEX(parser));
	  if(IS_COLON(tt)){
		new_lst = make_symbol(new_lst,t);
		new_lst = make_keyword(new_lst,LABEL);
	  } else {
		lexer_put_token(PARSER_GET_LEX(parser),tt);
		lexer_put_token(PARSER_GET_LEX(parser),t);
		new_lst = parser_parse_stmt_expr(parser);
	  }
	} else {
	  lexer_put_token(PARSER_GET_LEX(parser),t);
	  new_lst = parser_parse_stmt_expr(parser);
	}
  }
  
  if((!IS_IF(t)) && (!IS_WHILE(t)) && (!IS_FOR(t))){
	t = lexer_get_token(PARSER_GET_LEX(parser));
	if(!IS_SEMI_COLON(t)){
	  lexer_put_token(PARSER_GET_LEX(parser),t);
	}
  }
  
  return new_lst;
}

  static list_t *parser_parse_array(parser_t *parser){

  list_t *new_lst;
  token_t *t;

#ifdef __DEBUG__
  printf("parser_parse_array\n");
#endif
  
  new_lst = parser_parse_cexpr(parser);
  if(IS_NULL_LIST(new_lst)){
	new_lst = add_symbol(make_null(),UNDEFINED_SIZE);
  }
  new_lst = add_list(make_null(),new_lst);
  t = lexer_get_token(PARSER_GET_LEX(parser));
  new_lst = make_keyword(new_lst,ARRAY);
  
  return new_lst;
}

static list_t *parser_parse_stmt_expr(parser_t *parser){

  list_t *new_lst;
  list_t *sub_lst;
  token_t *t;

#ifdef __DEBUG__
  printf("parser_parse_stmt_expr\n");
#endif

  new_lst = parser_parse_cond_expr(parser);
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(IS_ASSIGN(t) || IS_CREMENT_ASSING_OPERATOR(t)){
    new_lst = make_symbol(new_lst,t);
	new_lst = concat(new_lst,parser_parse_cond_expr(parser));
	sub_lst = parser_parse_stmt_expr(parser);
	if(IS_NOT_NULL_LIST(sub_lst)){
	  new_lst = concat(new_lst,add_list(make_null(),sub_lst));
	} else {
	  new_lst = concat(new_lst,make_null());
	}
    new_lst = add_list(make_null(),new_lst);
  } else if(IS_QUESTION(t)){
	new_lst = add_list(make_null(),new_lst);
	new_lst = concat(new_lst,parser_parse_ternary(parser));
	new_lst = make_symbol(new_lst,t);
  } else {
    lexer_put_token(PARSER_GET_LEX(parser),t);
  }

  return new_lst;
}

static list_t *parser_parse_vars(parser_t *parser){

  list_t *new_lst;
  token_t *t;
  
#ifdef __DEBUG__
  printf("parser_parse_vars\n");
#endif
  new_lst = parser_parse_var(parser);
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(IS_COMMA(t)){
    new_lst = concat(new_lst,parser_parse_var(parser));
  } else if(IS_SEMI_COLON(t)){
    return new_lst;
  } else {
    lexer_put_token(PARSER_GET_LEX(parser),t);
  }
  
  return new_lst;
}

static list_t *parser_parse_var(parser_t *parser){

  list_t *new_lst;
  token_t *t;

#ifdef __DEBUG__
  printf("parser_parse_var\n");
#endif
  
  t = lexer_get_token(PARSER_GET_LEX(parser));
  new_lst = make_symbol(make_null(),t);
  
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(IS_LBRACKET(t)){
    new_lst = concat(new_lst,parser_parse_array(parser));
    new_lst = add_list(make_null(),new_lst);
    new_lst = make_keyword(new_lst,ARRAY);
  } else {
    lexer_put_token(PARSER_GET_LEX(parser),t);
  }

  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(!IS_ASSIGN(t)){
    lexer_put_token(PARSER_GET_LEX(parser),t);
  } else {
    new_lst = make_symbol(new_lst,t);
    new_lst = concat(new_lst,parser_parse_cexpr(parser));
    new_lst = add_list(make_null(),new_lst);
  }
  
  return new_lst;
}

static list_t *parser_parse_cond_expr(parser_t *parser){

#ifdef __DEBUG__
  printf("parser_parse_cond_expr\n");
#endif

  return parser_parse_lexpr(parser);
}

static list_t *parser_parse_bexpr(parser_t *parser){

  list_t *new_lst;
  token_t *t;

#ifdef __DEBUG__
  printf("parser_parse_bexpr\n");
#endif

  new_lst = parser_parse_cexpr(parser);
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(IS_AND(t) || IS_OR(t) || IS_XOR(t)){
    list_t *lst = make_symbol(make_null(),t);
    list_t *r = parser_parse_bexpr(parser);
    new_lst = concat(new_lst,r);
    new_lst = concat(lst,new_lst);
    new_lst = add_list(make_null(),new_lst);
  } else if(IS_AND_BOOL(t) || IS_OR_BOOL(t)){
    list_t *lst = make_symbol(make_null(),t);
    list_t *r = parser_parse_bexpr(parser);
    new_lst = concat(new_lst,r);
    new_lst = concat(lst,new_lst);
    new_lst = add_list(make_null(),new_lst);
  } else if(IS_BIT_SHIFT(t)){
    list_t *lst = make_symbol(make_null(),t);
    list_t *r = parser_parse_bexpr(parser);
    new_lst = concat(new_lst,r);
    new_lst = concat(lst,new_lst);
    new_lst = add_list(make_null(),new_lst);
  } else {
    lexer_put_token(PARSER_GET_LEX(parser),t);
  }
  
  return new_lst;
}

static list_t *parser_parse_cexpr(parser_t *parser){

  list_t *new_lst;
  list_t *lst;
  list_t *r;
  token_t *t;
  
  new_lst = parser_parse_expr(parser);
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(IS_COP(t)){
    lst = make_symbol(make_null(),t);
    r = parser_parse_cexpr(parser);
    new_lst = concat(new_lst,r);
    new_lst = concat(lst,new_lst);
    new_lst = add_list(make_null(),new_lst);
  } else {
    lexer_put_token(PARSER_GET_LEX(parser),t);
  }

  return new_lst;
}

static list_t *parser_parse_lexpr(parser_t *parser){

  #ifdef __DEBUG__
  printf("parser_parse_lexpr\n");
  #endif

  list_t *new_lst;
  token_t *t;
  
  new_lst = parser_parse_bexpr(parser);
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if((TOKEN_GET_TYPE(t) == TOKEN_BOOL_AND)
     || (TOKEN_GET_TYPE(t) == TOKEN_BOOL_OR)){
    list_t *op = make_symbol(make_null(),t);
    list_t *r = parser_parse_bexpr(parser);
    new_lst = concat(new_lst,r);
    new_lst = concat(op,new_lst);
    new_lst = add_list(make_null(),new_lst);
  } else {
    lexer_put_token(PARSER_GET_LEX(parser),t);    
  }
  
  return new_lst;
}

static list_t *parser_parse_expr(parser_t *parser){

  list_t *new_lst;
  list_t *op;
  list_t *r;
  token_t *t;

  new_lst = parser_parse_term(parser);
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(TOKEN_GET_TYPE(t) == TOKEN_ADD){
    op= make_symbol(make_null(),t);
    r = parser_parse_expr(parser);
    new_lst = concat(new_lst,r);
    new_lst = concat(op,new_lst);
    new_lst = add_list(make_null(),new_lst);
  } else if(TOKEN_GET_TYPE(t) == TOKEN_SUB){
    if(IS_NULL_LIST(new_lst)){
      new_lst = add_number(make_null(),0);
    }
    op= make_symbol(make_null(),t);
    r = parser_parse_expr(parser);
    new_lst = concat(new_lst,r);
    new_lst = concat(op,new_lst);
    new_lst = add_list(make_null(),new_lst);
  } else {
    lexer_put_token(PARSER_GET_LEX(parser),t);
  }
  
  return new_lst;
}

static list_t *parser_parse_term(parser_t *parser){

  list_t *new_lst;
  token_t *t;

#ifdef __DEBUG__
  printf("parser_parse_term\n");
#endif
  new_lst = parser_parse_factor(parser);
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(IS_MUL(t)
     || IS_DIV(t)
	 || IS_MOD(t)) {
    list_t *l = make_symbol(make_null(),t);
    list_t *r = parser_parse_term(parser);
    new_lst = concat(new_lst,r);
    new_lst = concat(l,new_lst);
    new_lst = add_list(make_null(),new_lst);
  } else {
    lexer_put_token(PARSER_GET_LEX(parser),t);
  }

  return new_lst;
}

static list_t *parser_parse_factor(parser_t *parser){
  
  list_t *new_lst;
  list_t *l;
  token_t *t;

#ifdef __DEBUG__
  printf("parser_parse_factor\n");
#endif
  new_lst = make_null();
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(IS_NUMBER(t)){
    new_lst = add_number(new_lst,atoi(TOKEN_GET_STR(t)));
  } else if(IS_HEX(t)){
    new_lst = add_number(new_lst,convert_hex_to_int(TOKEN_GET_STR(t)));
  } else if(IS_FLOAT(t)){
    new_lst = add_float(new_lst,TOKEN_GET_STR(t));
  } else if(IS_STRING(t)){
    new_lst = add_string(new_lst,TOKEN_GET_STR(t));
  } else if(IS_DEFINED(t)){
    new_lst = make_symbol(new_lst,t);
    new_lst = concat(new_lst,parser_parse_defined(parser));
    new_lst = add_list(make_null(),new_lst);
  } else if(IS_CHAR(t)){
	new_lst = add_char(new_lst,TOKEN_GET_STR(t));
  } else if(IS_LETTER(t)){
    new_lst = make_symbol(new_lst,t);
	new_lst = parser_parse_sub_op(parser,new_lst);
	if(IS_NOT_NULL_LIST(cdr(new_lst))){
	  new_lst = add_list(make_null(),new_lst);
	}
  } else if(IS_LPAREN(t)){
    new_lst = parser_baracket_expr(parser);
  } else if(IS_ASTERISK(t)){
	while(IS_ASTERISK(t)){
	  new_lst = concat(new_lst,make_symbol(make_null(),t));
	  t = lexer_get_token(PARSER_GET_LEX(parser));
	}
	
	if(!IS_LETTER(t)){
	  ;
	}
	
	l = parser_parse_sub(parser,make_symbol(make_null(),t));
	if(IS_LIST(l)){
	  l = car(l);
	}
	new_lst = concat(l,new_lst);
	new_lst = add_list(make_null(),new_lst);
  } else if(IS_AND(t)){
    new_lst = make_symbol(new_lst,t);
    t = lexer_get_token(PARSER_GET_LEX(parser));
    if(!IS_LETTER(t)){
      ;
    } else {
      new_lst = add_list(make_null(),make_symbol(new_lst,t));
    }
  } else if(IS_NOT(t) || IS_TILDE(t)){
    new_lst = make_symbol(new_lst,t);
    new_lst = add_list(make_null(),concat(new_lst,parser_parse_stmt_expr(parser)));
  } else if(IS_SIZEOF(t)){
    new_lst = make_symbol(new_lst,t);
    t = lexer_get_token(PARSER_GET_LEX(parser));
    if(IS_LPAREN(t)){
	  l = parser_parse_sizeoftype(parser);
	  if(IS_NULL_LIST(l)){
		l = parser_parse_cond_expr(parser);
	  }
	  new_lst = concat(new_lst,l);
      t = lexer_get_token(PARSER_GET_LEX(parser));
      if(!IS_RPAREN(t)){
		lexer_put_token(PARSER_GET_LEX(parser),t);
      }
	} else {
	  lexer_put_token(PARSER_GET_LEX(parser),t);
      new_lst = concat(new_lst,parser_parse_factor(parser));
    }
    new_lst = add_list(make_null(),new_lst);
  } else {
	lexer_put_token(PARSER_GET_LEX(parser),t);
	new_lst = parser_parse_dclation(parser);
	if(IS_NOT_NULL_LIST(new_lst)){
	  new_lst = add_list(make_null(),new_lst);
	}
  }

  return new_lst;
}

static list_t *parser_baracket_expr(parser_t *parser){

  list_t *new_lst;
  token_t *t;
#ifdef __DEBUG__
  printf("parser_baracket_expr\n");
#endif
  new_lst = parser_parse_type(parser,FALSE,TRUE);
  if(IS_NULL_LIST(new_lst)){
	new_lst = parser_parse_stmt_expr(parser);
    t = lexer_get_token(PARSER_GET_LEX(parser));
    if(IS_RPAREN(t)){
	  return new_lst;
    } else if(IS_END_T(t)){
      new_lst = make_null();
    } else {
	  t = lexer_get_token(PARSER_GET_LEX(parser));
	}
  } else {
    new_lst = add_list(make_null(),reverse(new_lst));
    t = lexer_get_token(PARSER_GET_LEX(parser));
    if(!IS_RPAREN(t)){
      exit(1);
    }

	new_lst = parser_parse_cast(parser,new_lst);
  }
    
  return new_lst;
}

static list_t *parser_parse_if(parser_t *parser){
  
  list_t *new_lst;
  list_t *seq_stmts;
  list_t *alt_stmts;
  token_t *t;
#ifdef __DEBUG__
  printf("parser_parse_if\n");
#endif
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(!IS_LPAREN(t)){
    
  }

  new_lst = parser_parse_bexpr(parser);
  if(!IS_LIST(new_lst)){
	new_lst = add_list(make_null(),new_lst);
  }

  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(!IS_RPAREN(t)){
    
  }

  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(IS_LBRACE(t)){
    seq_stmts = add_list(make_null(),parser_parse_block(parser));
  } else {
	lexer_put_token(PARSER_GET_LEX(parser),t);
    seq_stmts = add_list(make_null(),parser_parse_stmt(parser));
  }
  new_lst = concat(new_lst,seq_stmts);
  
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(IS_ELSE(t)){
	t = lexer_get_token(PARSER_GET_LEX(parser));
	if(IS_IF(t)){
	  alt_stmts = parser_parse_if(parser);
	  alt_stmts = make_symbol(alt_stmts,t);
	  alt_stmts = add_list(make_null(),alt_stmts);
	} else {
	  lexer_put_token(PARSER_GET_LEX(parser),t);
	  alt_stmts = parser_parse_else(parser);
	}
	new_lst = concat(new_lst,alt_stmts);
  } else {
    lexer_put_token(PARSER_GET_LEX(parser),t);
  }
  
  return new_lst;
}

static list_t *parser_parse_else(parser_t *parser){

  list_t *new_lst;
  token_t *t;
  
  new_lst = make_null();
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(IS_LBRACE(t)){
    new_lst = add_list(make_null(),parser_parse_block(parser));
  } else {
	lexer_put_token(PARSER_GET_LEX(parser),t);
    new_lst = add_list(make_null(),parser_parse_stmt(parser));
  }
  
  return new_lst;
}

static list_t *parser_parse_goto(parser_t *parser){

  list_t *new_lst;
  token_t *t;

#ifdef __DEBUG__
  printf("parser_parse_goto\n");
#endif

  new_lst = make_null();
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(!IS_LETTER(t)){

  }
  new_lst = make_symbol(new_lst,t);

  return new_lst;
}

static list_t *parser_parse_while(parser_t *parser){

  list_t *new_lst;
  list_t *cond;
  list_t *items;
  token_t *t;

  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(!IS_LPAREN(t)){

  }
  
  cond = parser_parse_stmt_expr(parser);
  
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(!IS_RPAREN(t)){
    
  }
  
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(IS_LBRACE(t)){
	items = add_list(make_null(),parser_parse_block(parser));
  } else {
	lexer_put_token(PARSER_GET_LEX(parser),t);
	items = add_list(make_null(),parser_parse_stmt(parser));
  }
  
  new_lst = concat(cond,items);
  
  return new_lst;
}

static list_t *parser_parse_do_while(parser_t *parser){

  token_t *t;
  list_t *stmt;
  list_t *cond;
  list_t *new_lst;

#ifdef __DEBUG__
  printf("parser_parse_do_while\n");
#endif

  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(IS_LBRACE(t)){
	stmt = add_list(make_null(),parser_parse_block(parser));
  } else {
	lexer_put_token(PARSER_GET_LEX(parser),t);
	stmt = add_list(make_null(),parser_parse_stmt(parser));
  }
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(!IS_WHILE(t)){

  }

  cond = parser_parse_stmt_expr(parser);
  new_lst = concat(stmt,cond);

  return new_lst;
}

static list_t *parser_parse_asm(parser_t *parser){

  list_t *new_lst;
  token_t *t;

#ifdef __DEBUG__
  printf("parser_parse_asm\n");
#endif
  new_lst = make_null();
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(!IS_LPAREN(t)){
	exit(1);
  }

  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(!IS_STRING(t)){
	exit(1);
  }

  new_lst = add_string(new_lst,TOKEN_GET_STR(t));
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(!IS_RPAREN(t)){
	exit(1);
  }

  return new_lst;
}

static list_t *parser_parse_switch(parser_t *parser){

  list_t *new_lst;
  list_t *sub_lst;
  token_t *t;

#ifdef __DEBUG__
  printf("parser_parse_switch\n");
#endif

  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(!IS_LPAREN(t)){
    error(TOKEN_GET_LINE_NO(t),TOKEN_GET_NAME(t),"Expected : [%s] but got [%s]\n","(",TOKEN_GET_STR(t));
	exit(1);
  }
  
  new_lst = parser_parse_cond_expr(parser);
  
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(!IS_RPAREN(t)){
    error(TOKEN_GET_LINE_NO(t),TOKEN_GET_NAME(t),"Expected : [%s] but got [%s]\n",")",TOKEN_GET_STR(t));
    exit(1);
  }

  new_lst = concat(new_lst,parser_parse_stmt_expr(parser));

  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(!IS_LBRACE(t)){
    error(TOKEN_GET_LINE_NO(t),TOKEN_GET_NAME(t),"Expected : [%s] but got [%s]\n","{",TOKEN_GET_STR(t));
    exit(1);
  }

  new_lst = concat(new_lst,add_list(make_null(),parser_parse_switch_cases(parser)));
  
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(!IS_RBRACE(t)){
    error(TOKEN_GET_LINE_NO(t),TOKEN_GET_NAME(t),"Expected : [%s] but got [%s]\n","}",TOKEN_GET_STR(t));
    exit(1);
  }
  
  return new_lst;
}

static list_t *parser_parse_switch_cases(parser_t *parser){

  list_t *new_lst;
  token_t *t;
  
#ifdef __DEBUG__
  printf("parser_parse_switch_cases\n");
#endif
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if((IS_CASE(t)) || (IS_DEFAULT(t))){
    new_lst = make_symbol(make_null(),t);
    new_lst = add_list(make_null(),concat(new_lst,parser_parse_switch_case(parser)));
    new_lst = concat(new_lst,parser_parse_switch_cases(parser));
  } else {
    new_lst = make_null();
    lexer_put_token(PARSER_GET_LEX(parser),t);
  }
  
  return new_lst;
}

static list_t *parser_parse_switch_case(parser_t *parser){
  
  list_t *new_lst;
  list_t *stmt_lst;
  token_t *t;

  #ifdef __DEBUG__
  printf("parser_parse_switch_carse\n");
  #endif
  new_lst = parser_parse_cexpr(parser);
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(!IS_COLON(t)){
    
  }
  
  stmt_lst = add_list(make_null(),parser_parse_stmts(parser));
  new_lst = concat(new_lst,stmt_lst);
  new_lst = add_list(make_null(),new_lst);
  
  return new_lst;
}


static list_t *parser_parse_for(parser_t *parser){
  
  list_t *new_lst;
  list_t *items;
  token_t *t;
  
  new_lst = make_null();
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(!IS_LPAREN(t)){
    
  }
  
  new_lst = add_list(make_null(),parser_parse_stmt_expr(parser));
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(!IS_SEMI_COLON(t)){
    
  }

  new_lst = concat(new_lst,add_list(make_null(),parser_parse_stmt_expr(parser)));
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(!IS_SEMI_COLON(t)){
    
  }
    
  new_lst = concat(new_lst,add_list(make_null(),parser_parse_stmt_expr(parser)));
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(!IS_RPAREN(t)){
    
  }

  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(!IS_LBRACE(t)){
    
  }

  items = add_list(make_null(),parser_parse_block(parser));
  new_lst = concat(new_lst,items);

  return new_lst;
}


static list_t *parser_parse_sub(parser_t *parser,list_t *lst){

  list_t *new_lst;
  list_t *sub_lst;
  token_t *t;
#ifdef __DEBUG__
  printf("parser_parse_sub\n");
#endif

  new_lst = make_null();
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(IS_LBRACKET(t)){
	new_lst = concat(new_lst,parser_parse_array(parser));
	new_lst = concat(lst,new_lst);
	new_lst = parser_parse_sub(parser,concat(lst,new_lst));
  } else if(IS_LPAREN(t)){
	if(is_buildin_func(lst) && !set_find_obj(PARSER_GET_SET(parser),car(lst))){
	  sub_lst = add_list(make_null(),lst);
	  new_lst = concat(sub_lst,concat(new_lst,parser_parse_func_call(parser)));
	  new_lst = add_list(make_null(),make_keyword(new_lst,BUILTIN_FUNC));
	  new_lst = parser_parse_sub(parser,new_lst);
	} else {
	  sub_lst = add_list(make_null(),lst);
	  new_lst = concat(sub_lst,concat(new_lst,parser_parse_func_call(parser)));
	  new_lst = add_list(make_null(),make_keyword(new_lst,FUNC_CALL));
	  new_lst = parser_parse_sub(parser,new_lst);
	}
  } else if(IS_DOT(t) || IS_ARROW_OP(t)){
	new_lst = parser_parse_struct_access(parser,lst,t);
  } else {
    lexer_put_token(PARSER_GET_LEX(parser),t);
	new_lst = lst;
  }

  return new_lst;
}

static list_t *parser_parse_struct_access(parser_t *parser,list_t *lst,token_t *t){

  list_t *new_lst;
  list_t *sub_lst;

#ifdef __DEBUG__
  printf("parser_parse_struct_access\n");
#endif

  sub_lst = parser_parse_sub_member(parser);
  new_lst = add_list(make_null(),lst);
  new_lst = concat(new_lst,parser_parse_sub(parser,sub_lst));
  new_lst = make_symbol(new_lst,t);
  new_lst = add_list(make_null(),new_lst);

  return new_lst;
}

static list_t *parser_parse_sub_op(parser_t *parser,list_t *lst){

  list_t *new_lst;
  list_t *sub_lst;
  token_t *t;

#ifdef __DEBUG__
  printf("parser_parse_sub_op\n");
#endif

  new_lst = parser_parse_sub(parser,lst);
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(IS_CREMENT_OPERATOR(t)){
	new_lst = make_symbol(new_lst,t);
  } else {
	lexer_put_token(PARSER_GET_LEX(parser),t);
  }
  
  return new_lst;
}

static list_t *parser_parse_sub_member(parser_t *parser){

  list_t *val;
  token_t *t;

#ifdef __DEBUG__
  printf("parser_parse_sub_member\n");
#endif

  val = make_null();
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(!IS_LETTER(t)){
	exit(1);
  }

  val = make_symbol(val,t);

  return val;
}

static list_t *parser_parse_func_call(parser_t *parser){

  list_t *new_lst;
  token_t *t;
#ifdef __DEBUG__
  printf("parser_parse_func_call\n");
#endif
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(IS_RPAREN(t)){
    new_lst = make_null();
  } else {
    lexer_put_token(PARSER_GET_LEX(parser),t);
	new_lst = parser_parse_func_args(parser);
  }

  new_lst = add_list(make_null(),new_lst);

  return new_lst;
}

static list_t *parser_parse_func_args(parser_t *parser){

  list_t *new_lst;
  token_t *t;

#ifdef __DEBUG__
  printf("parser_parse_func_args\n");
#endif

  new_lst = parser_parse_func_arg(parser);
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(IS_COMMA(t)){
    new_lst = concat(new_lst,parser_parse_func_args(parser));
  } else if(IS_RPAREN(t)){
    return new_lst;
  } else {

  }

  return new_lst;
}

static list_t *parser_parse_func_arg(parser_t *parser){

  list_t *new_lst;
#ifdef __DEBUG__
  printf("parser_parse_func_arg\n");
#endif

  new_lst = add_list(make_null(),parser_parse_cexpr(parser));

  return new_lst;
}

static list_t *parser_parse_sizeoftype(parser_t *parser){

  list_t *new_lst;
  token_t *t;
  
#ifdef __DEBUG__
  printf("parser_parse_sizeoftype\n");
#endif

  new_lst = parser_parse_dclation(parser);
  if(IS_NULL_LIST(new_lst)){
	return new_lst;
  } else {
	new_lst = make_keyword(new_lst,TYPE);
  }
  
  return new_lst;
}

static list_t *parser_parse_cast(parser_t *parser,list_t *cast_type){

  list_t *new_lst;
  list_t *sub_lst;

#ifdef __DEBUG__
  printf("parser_parse_cast\n");
#endif

  sub_lst = parser_parse_unary_expr(parser);
  sub_lst = reverse(sub_lst);
  new_lst = make_keyword(reverse(cast_type),CAST);
  new_lst = concat(new_lst,sub_lst);
  new_lst = add_list(make_null(),new_lst);

  return new_lst;
}

static list_t *parser_parse_unary_expr(parser_t *parser){

  list_t *new_lst;

#ifdef __DEBUG__
  printf("parser_parse_unary_expr\n");
#endif

  new_lst = parser_parse_factor(parser);

  return new_lst;
}

static list_t *parser_parse_pointer(parser_t *parser){

  list_t *new_lst;
  token_t *t;

#ifdef __DEBUG__
  printf("parser_parse_pointer\n");
#endif

  new_lst = make_null();
  t = lexer_get_token(PARSER_GET_LEX(parser));
  while(IS_ASTERISK(t)){
	new_lst = concat(new_lst,make_symbol(make_null(),t));
	t = lexer_get_token(PARSER_GET_LEX(parser));
	if(is_type_qualifier(t)){
	  new_lst = concat(new_lst,make_symbol(parser_parse_type_qualifier_list(parser),t));
	  t = lexer_get_token(PARSER_GET_LEX(parser));
	}
  }
  lexer_put_token(PARSER_GET_LEX(parser),t);

  return new_lst;
}

static list_t *parser_parse_dcls(parser_t *parser,list_t *type_lst){

  list_t *new_lst;
  token_t *t;

#ifdef __DEBUG__
  printf("parser_parse_dcls\n");
#endif

  new_lst = parser_parse_dcl(parser);
  t = lexer_get_token(PARSER_GET_LEX(parser));
  new_lst = concat(new_lst,copy_list(type_lst));
  new_lst = make_keyword(new_lst,DECL_VAR);
  PARSER_SET_VAR_LST(parser,concat(PARSER_GET_VAR_LST(parser),add_list(make_null(),add_list(make_null(),new_lst))));
  new_lst = add_list(make_null(),new_lst);
  if(IS_COMMA(t)){
	new_lst = concat(new_lst,parser_parse_dcls(parser,type_lst));
  } else if(IS_SEMI_COLON(t)){
	return new_lst;
  } else {
	exit(1);
  }

  return new_lst;
}

static list_t *parser_parse_dcl(parser_t *parser){

  list_t *new_lst;
  token_t *t;

#ifdef __DEBUG__
  printf("parser_parse_dcl\n");
#endif
  
  new_lst = parser_parse_pointer(parser);
  new_lst = concat(parser_parse_dirdcl(parser),new_lst);

  return new_lst;
}

static list_t *parser_parse_dirdcl(parser_t *parser){

  list_t *new_lst;
  token_t *t;
  
#ifdef __DEBUG__
  printf("parser_parse_dirdcl\n");
#endif
  new_lst = make_null();
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(IS_LETTER(t)){
    new_lst =  make_symbol(make_null(),t);
  } else if(IS_LPAREN(t)){
    new_lst = parser_parse_dcl(parser);
    t = lexer_get_token(PARSER_GET_LEX(parser));
    if(!IS_RPAREN(t)){
	  printf("%s\n",TOKEN_GET_STR(t));
	  exit(1);
    }
  } else {
    lexer_put_token(PARSER_GET_LEX(parser),t);    
  }

  new_lst = concat(new_lst,parser_parse_subdirdcl(parser));
  
  return new_lst;
}

static list_t *parser_parse_subdirdcl(parser_t *parser){

  list_t *new_lst;
  token_t *t;
#ifdef __DEBUG__
  printf("parser_parse_subdirdcl\n");
#endif

  new_lst = make_null();
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(IS_LBRACKET(t)){
    new_lst = concat(new_lst,parser_parse_array(parser));
	new_lst = concat(new_lst,parser_parse_subdirdcl(parser));
  } else if(IS_LPAREN(t)){
	PARSER_SET_VAR_LST(parser,make_null());
	new_lst = concat(new_lst,add_list(make_null(),parser_parse_funcargs(parser,0)));
	new_lst = make_keyword(new_lst,FUNC);
	new_lst = concat(new_lst,parser_parse_subdirdcl(parser));
  } else {
    lexer_put_token(PARSER_GET_LEX(parser),t);
  }
  
  return new_lst;
}

static list_t *parser_parse_initializer(parser_t *parser){
  
  list_t *new_lst;
  token_t *t;
#ifdef __DEBUG__
  printf("parser_parse_initializer\n");
#endif

  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(IS_LBRACE(t)){
	new_lst = parser_parse_array_initialyzer(parser);
	new_lst = add_list(make_null(),new_lst);
	new_lst = make_keyword(new_lst,INIT_LIST);
	new_lst = add_list(make_null(),new_lst);
  } else {
	lexer_put_token(PARSER_GET_LEX(parser),t);
	new_lst = parser_parse_cexpr(parser);
  }
  
  return new_lst;

}

static list_t *parser_parse_array_initialyzer(parser_t *parser){

  list_t *new_lst;
  token_t *t;
#ifdef __DEBUG__
  printf("parser_parse_array_initialyzer\n");
#endif

  new_lst = parser_parse_initialyze_value(parser);
  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(IS_RBRACE(t)){
	return new_lst;
  } else if(IS_COMMA(t)){
	return concat(new_lst,parser_parse_array_initialyzer(parser));
  } else {
	exit(1);
  }
}

static list_t *parser_parse_initialyze_value(parser_t *parser){

  list_t *new_lst;
  token_t *t;

#ifdef __DEBUG__
  printf("parser_parse_initialyze_value\n");
#endif

  t = lexer_get_token(PARSER_GET_LEX(parser));
  if(IS_LBRACE(t)){
	new_lst = add_list(make_null(),parser_parse_array_initialyzer(parser));
  } else {
	lexer_put_token(PARSER_GET_LEX(parser),t);
	new_lst = parser_parse_cexpr(parser);
  }
  
  return new_lst;
}

static int is_dclvar(list_t *lst){
  return (2 == length_of_list(lst)) || IS_LIST(cdr(lst));
}

static int is_dcl(list_t *lst){
  return STRCMP(COMP_DEF,car(lst));
}

static int is_typedef_name(parser_t *parser,token_t *token){

  set_t *set;

  set = PARSER_GET_SET(parser);
  if(set_find_obj(set,TOKEN_GET_STR(token))){
    return TRUE;
  } else {
    return FALSE;
  }
}

static bool_t is_buildin_func(list_t *lst){

#ifdef __DEBUG__
  printf("is_buildin_func\n");
#endif

  if(STRCMP(BUILT_IN_VA_START,car(lst))
	 || STRCMP(BUILT_IN_VA_ARG,car(lst))
	 || STRCMP(BUILT_IN_VA_END,car(lst))){
	return TRUE;
  }

  return FALSE;
}

static void record_obj_set(parser_t *parser,list_t *lst){

  list_t *l;
#ifdef __DEBUG__
  printf("record_obj_set\n");
#endif

  if(IS_LIST(lst)){
	set_add_obj(PARSER_GET_SET(parser),car(car(lst)));
  } else {
	set_add_obj(PARSER_GET_SET(parser),car(lst));
  }

  return;
}


static bool_t has_array(list_t *lst){

  if(IS_LIST(lst)){
	return TRUE;
  }

  return FALSE;
}

static bool_t is_func_decl(list_t *lst){

  if(is_pointer(lst)){
	return FALSE;
  }

  if(STRCMP(FUNC,car(lst))
	 || STRCMP(FUNC,car(cdr(lst)))){
	return TRUE;
  }

  return FALSE;
}

static bool_t is_compound_def_type(list_t *lst){

  string_t name;
  name = (string_t)car(lst);
  if(STRCMP(COMP_DEF,name)){
	return TRUE;
  }

  return FALSE;
}

static bool_t is_typedef(list_t *lst){

  string_t name;
  name = (string_t)car(lst);
  if(STRCMP(TYPEDEF,name)){
	return TRUE;
  }

  return FALSE;
}

static bool_t is_type_qualifier(token_t *t){

#ifdef __DEBUG__
  printf("is_type_qualifier\n");
#endif

  if(IS_CONST(t) ||
	 IS_VOLATILE(t) ||
	 IS_RESTRICT(t)){
	return TRUE;
  }

  return FALSE;
}

static list_t *create_func(parser_t *parser,list_t *lst){

  list_t *new_lst;
#ifdef __DEBUG__
  printf("create_func\n");
#endif

  make_func(lst);
  new_lst = concat(make_keyword(make_null(),FUNC_DEF),lst);
  create_ret_lst(parser,new_lst);
  new_lst = concat(new_lst,add_list(make_null(),PARSER_GET_VAR_LST(parser)));
  PARSER_SET_VAR_LST(parser,make_null());
  new_lst = add_list(make_null(),new_lst);

  return new_lst;
}

static list_t *create_ret_lst(parser_t *parser,list_t *lst){

  list_t *new_lst;
  list_t *ret_lst;
  list_t *f;
  list_t *p;
  list_t *q;

#ifdef __DEBUG__
  printf("create_ret_lst\n");
#endif

  new_lst = make_null();
  f = get_func_name(lst);
  ret_lst = get_ret_lst(f);
  for(p = ret_lst; !IS_LIST(cdr(p)); p = cdr(p)){
	if(IS_NULL_LIST(cdr(p))){
	  break;
	}
  }

  q = cdr(p);
  LIST_SET_NEXT(p,make_null());
  LIST_SET_NEXT(cdr(f),make_null());
  new_lst = concat(lst,add_list(make_null(),ret_lst));
  new_lst = concat(new_lst,q);

  return new_lst;
}

static void make_func(list_t *lst){

  list_t *p;
  list_t *pre;
#ifdef __DEBUG__
  printf("make_func\n");
#endif

  pre = make_null();
  for(p = lst; IS_NOT_NULL_LIST(p); p = cdr(p)){
	if(STRCMP(FUNC,car(p))){
	  break;
	}
	pre = p;
  }

  LIST_SET_NEXT(pre,cdr(p));

  return;
}

static list_t *get_ret_lst(list_t *lst){

  list_t *p;

#ifdef __DEBUG__
  printf("get_ret_lst\n");
#endif

  p = get_func_name(lst);

  return cdr(cdr(p));

}


static list_t *make_symbol(list_t *lst,token_t *t){

  list_t *new_lst;

#ifdef __DEBUG__
  printf("make_symbol\n");
#endif

  new_lst = add_symbol(lst,TOKEN_GET_STR(t));
  new_lst->obj.symbol.src = TOKEN_GET_NAME(t);
  new_lst->obj.symbol.lineno = TOKEN_GET_LINE_NO(t);

  return new_lst;
}

static list_t *make_keyword(list_t *lst,string_t text){

  list_t *new_lst;

#ifdef __DEBUG__
  printf("make_keyword\n");
#endif

  new_lst = add_symbol(lst,text);

  return new_lst;
}

static string_t make_no_name(parser_t *parser){

#ifdef __DEBUG__
  printf("make_no_name\n");
#endif
  
  string_t str;
  char buf[256];
  sprintf(buf,"%s-%d",NO_NAME,parser->cnt);
  parser->cnt++;
  str = mem(strlen(buf) + 1);
  strcpy(str,buf);
  
  return str;
}
