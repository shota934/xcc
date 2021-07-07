#include <stdio.h>

int main(){

  int arr[5];
  int n = 5;
  int i;
  int sum;
  int ret;
  
  arr[0] = 20;
  arr[1] = 400;
  arr[2] = 300;
  arr[3] = 100;
  arr[4] = 500;

  sum = 0;
  for(i = 0; i < n; i++){
	sum += arr[i];
  }

  printf("%d\n",sum);
  if(1320 == sum){
	printf("OK\n");
	ret = 1;
  } else {
	printf("NG\n");
	ret = 0;
  }
  
  return ret;
}
