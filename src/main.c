// 
//
//  Copyright 2019 Shota Hisai. Released under the MIT license.
//
//
#include <stdio.h>
#include "unistd.h"
#include "parser.h"
#include "list.h"
#include "dump.h"
#include "file.h"
#include "gen.h"
#include "env.h"
#include "mem.h"
#include "optinfo.h"
#include "stack.h"
#include "cpp.h"
#include "com.h"
#include "srcinfo.h"

static void compile(opt_info_t *optinfo,string_t name);
static void cgen(list_t *ast,char *name);
static void show_version();
static void show_help();
static void parse_opt(opt_info_t *optinfo,int argc,char *argv[]);
static list_t *parse_src(compile_info_t *com,parser_t *parser);
static file_t *create_outfile(char *name);
static string_t make_outputfile(char *name,char *extend);

static void compile(opt_info_t *optinfo,string_t name){

  parser_t *parser;
  opt_info_t opt_into;
  gen_info_t *ei;
  list_t *ast;
  file_t *file;
  stack_ty *stack;
  lexer_t *lexer;
  compile_info_t *com;
  
  com = create_compile_info();
  file = file_create();
  FILE_SET_NAME(file,name);
  FILE_SET_MODE(file,FILE_OPEN_MODE);
  if(!file_open(file)){
    printf("Not found source file.\n");
    return;
  }

  stack = COM_GET_STACK(com);
  parser = parser_create();
  lexer = create_lexer();
  PARSER_SET_LEX(parser,lexer);
  parser_set_src(parser,file_read_as_string(file));
  LEXER_SET_NAME(lexer,file);
  
  COM_SET_SRC_INFO(com,create_source_info(file,NULL));
  cpreprocess(com,PARSER_GET_LEX(parser));
  if(IS_ONLY_PREPROCESS(optinfo)){
    dump_token_sequences(com);
    return;
  }
  
  ast = parse_src(com,parser);
  if(IS_DUMPING_AST(optinfo)){
    dump_ast(ast);
    return;
  }

  if(IS_ONLY_GENASSEMBLER(optinfo)){
    char *output_name = make_outputfile(name,OUTPUT_FILE_EXTENSION);
    cgen(ast,output_name);
    return;
  }
  
  if(IS_ONLY_GEN_OBJFILE(optinfo)){
    return;
  }
  
  cgen(ast,DEFAULT_OUTPUT_FILE);
  
  return;
}

static void cgen(list_t *ast,char *name){

  gen_info_t *ei;
  env_t *env;
  file_t *output_file;
  
  ei = create_gen_info();
  env = make_env();
  output_file = create_outfile(name);
  GEN_INF_SET_FILE(ei,output_file);
  file_open(output_file);
  gen(ei,env,ast,TRUE);
  file_close(output_file);
  fre(output_file);
  
  return;
}

static void show_version(){

  #ifdef __DEBUG__
  printf("show_version\n");
  #endif

  return;
}

static void show_help(){

  #ifdef __DEBUG__
  printf("show_help\n");
  #endif

  fprintf(stdout,"Options:\n");
  fprintf(stdout,"\t-d\t\tDump ast\n");
  fprintf(stdout,"\t-E\t\tOnly run preprocess\n");
  fprintf(stdout,"\t-S\t\nOnly run compiling\n");
  fprintf(stdout,"\t-v\t\tPrint version\n");

  exit(1);

  return;
}

static void parse_opt(opt_info_t *optinfo,int argc,char *argv[]){

  int opt;
  #ifdef __DEBUG__
  printf("show_help\n");
  #endif
  
  opt = 0;
  while((opt = getopt(argc,argv,"dhvcSoE:")) != -1){
    switch(opt){
    case 'v':
      show_version();
      break;
    case 'd':
      SET_DUMPING_AST(optinfo,TRUE);
      break;
    case 'c':
      SET_ONLY_GEN_OBJFILE(optinfo,TRUE);
      break;
    case 'S':
      SET_ONLY_GENASSEMBLER(optinfo,TRUE);
      break;
    case 'E':
      SET_ONLY_PREPROCESS(optinfo,TRUE);
      break;
	case 'h':
	  show_help();
	  break;
    default:
      break;
    }
  }
  
  return;
}

static list_t *parse_src(compile_info_t *com,parser_t *parser){
  
  list_t *ast;
  stack_ty *stack;
  source_info_t *src_info;
  lexer_t *lexer;
  file_t *file;
  list_t *p;
  
  ast = make_null();
  p = COM_GET_SRC_INFO_LST(com);
  while(TRUE){
    if(IS_NULL_LIST(p)){
      break;
    }
    
    src_info = car(p);
    file = SOURCE_INFO_GET_FILE(src_info);
    load_lexinfo(src_info,PARSER_GET_LEX(parser));
    ast = concat(ast,parser_parse(parser));
    file_close(SOURCE_INFO_GET_FILE(src_info));
    p = cdr(p);
  }
  
  return ast;
}

static string_t make_outputfile(char *name,char *extend){

  char *output_name;
  int len;

  len = (int)STRLEN(name);
  output_name = mem(sizeof(len) + NULL_CHAR);
  strcpy(output_name,name);
  *(output_name + len - 1) = *extend;
  
  return output_name;
}

static file_t *create_outfile(char *name){

  file_t *file;
  char *output_name;
  int len;

  len = (int)STRLEN(name);
  file = file_create();
  FILE_SET_NAME(file,name);
  FILE_SET_MODE(file,FILE_WRITE_MODE);
  
  return file;
}

int main(int argc,char *argv[]){
  
  char *input;
  opt_info_t optinfo;

  if(argc < 2){
    return -1;
  } else if(argc <= 2){
    input = argv[1];
  } else {
    input = argv[2];
  }
  
  init_optinfo(&optinfo);
  parse_opt(&optinfo,argc,argv);
  compile(&optinfo,input);

  return 0;
}

