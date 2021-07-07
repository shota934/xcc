#include <stdio.h>

#define N 10
#define DEBUG

int sum(int n){

#ifdef DEBUG
  printf("sum\n");
#endif

  if(n <= 0){
	return 0;
  } else {
	return n + sum(n - 1);
  }
}

int main(){

  int s;

  printf("%d\n",sum(N));

  return sum(N);
}
