#include <stdlib.h>

int main(){

  char *p = malloc(sizeof(char) * 4);

  free(p);
  p = NULL;

  return 0;
}

  
