#include <stdio.h>
#include <stdarg.h>

#define EXPECTED 5

int sum(int num,...){
  
  va_list ap;
  int sum1 = 0;
  int i;

  
  va_start(ap,num);
  for(i = 0; i < num; i++){
	sum1 += va_arg(ap,int);
  }
  va_end(ap);
  
  return sum1;
}

int main(){

  int ans;

  ans = sum(2,2,3);
  printf("ans : %d\n",ans);
  if(ans == EXPECTED){
	return 0;
  } else {
	return 1;
  }
}
