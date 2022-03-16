#include <stdio.h>

int func(int arr[],int num){

  int sum;
  int i;
  for(i = 0; i < num; i++){
	sum += arr[i];
  }
  
  return sum;
}

int main(){

  int arr[ ] = {1,2,3,4,5,6,7,8,9,10};
  int sum;
  int ret;

  sum = func(arr,sizeof(arr) / sizeof(arr[0]));
  ret = 0;
  printf("%d\n",sum);
  if(sum == 55){
	ret = 0;
  } else {
	ret = 1;
  }

  return ret;
}

