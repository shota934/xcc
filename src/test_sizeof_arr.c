#include <stdio.h>

int main(){

  int arr[10];
  int a = sizeof(arr) / sizeof(arr[0]);
  int ret;
  
  if(a == 10){
	printf("OK\n");
	ret = 0;
  } else {
	printf("NG\n");
	ret = 1;
  }
  
  return ret;
}
