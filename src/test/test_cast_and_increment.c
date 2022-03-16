#include <stdio.h>
#include <string.h>

#define EXPECTED_VALUE "ello"

int main(){

  char *ptr = "hello";
  void *p = (void *)ptr;

  printf("%s\n",(char *)p++);
  if(!strcmp(p,EXPECTED_VALUE)){
	return 0;
  }

  return 1;
}
