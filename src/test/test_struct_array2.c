#include <stdio.h>

#define N 70

typedef struct sample1 {
  int c;
  int d;
} sample1;

typedef struct sample {
  int a;
  int b;
  sample1 sam1[3];
} sample;

int func();

int func(){

  sample sams[2];
  int d;

  sams[0].a = 20;
  sams[0].sam1[2].d = 50;

  d = sams[0].a + sams[0].sam1[2].d;
  
  printf("%d\n",d);
  
  return d;
}

int main(){

  int d;
  int ret;

  ret = 0;
  d = func();
  if(d == N){
	ret = 0;
  } else {
	ret = 1;
  }

  return ret;
}
