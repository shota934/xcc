#include <stdio.h>

#define N 66

int main(){

  int i;
  int sum;
  int ret;

  i = 0;
  sum = 0;
  do {
	i++;
	sum += i;
  } while(i <= 10);
  
  printf("%d\n",sum);
  if(sum == N){
	ret = 0;
  } else {
	ret = 1;
  }
  
  return ret;
}
