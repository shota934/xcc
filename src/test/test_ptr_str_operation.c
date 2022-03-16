#define DEBUG

#define EXPECTED_VALUE "ello Wrold"

#ifdef DEBUG
#include <stdio.h>
#include <string.h>
#endif

int main(){

  char *ptr = "Hello Wrold";
  void *p = (void *)ptr;
  char *p1 = (char *)p + 1;

#ifdef DEBUG 
  printf("%s\n",p1);
#endif

  if(!strcmp(EXPECTED_VALUE,p1)){
	printf("OK\n");
	return 0;
  }
  printf("NG\n");
  
  return 1;
}
