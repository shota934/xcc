#include <stdio.h>

int func(int arr[5]);
int func(int arr[5]){
  return arr[2] + arr[4];
}

int main(){

  int a[5];
  int s;
  int ret;

  a[2] = 20;
  a[4] = 2000;

  s = func(a);
  printf("%d\n",s);
  if(s == 2020){
	printf("OK\n");
	ret = 1;
  } else {
	printf("NG\n");
	ret = 0;
  }
  
  return ret;
}
