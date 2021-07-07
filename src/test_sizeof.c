#include <stdio.h>

struct sample {
  int a;
  int b;
};

int main(){

  printf("%d\n",sizeof(struct sample));

  return 0;
}
