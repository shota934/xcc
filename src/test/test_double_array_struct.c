#include <stdio.h>

typedef struct sample {
  int a;
  int b;
} sample;

int main(){

  int ret;
  sample sams[2][2];
  
  sams[1][1].a = 20;
  sams[0][1].b = 30;

  ret = 0;
  sams[0][0].b = sams[1][1].a + sams[0][1].b;
  if (sams[0][0].b == 50){
	printf("OK\n");
	ret = 0;
  } else {
	printf("NG\n");
  }
  
  return ret;
}
