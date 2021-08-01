#include <stdio.h>

#define N 3

struct sample {
  int a;
  int b;
};

int main(){

  struct sample sam = {1,2};
  int a;

  a = sam.a + sam.b;
  if(a == N){
	
  } else {
	return 1;
  }

  if(sam.a + sam.b == 0){
	
  } else {
	return 2;
  }
  
  return 0;
}
