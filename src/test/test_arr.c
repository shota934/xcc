#include <stdio.h>

int print_array(int arr[5],int n);
int print_array(int arr[5],int n){

  int i;
  printf("[");
  for(i = 0; i < n; i++){
	printf("%d ",arr[i]);
  }
  printf("]\n");
  
  return 0;
}

int main(){

  int i;
  int arr[5];

  arr[0] = 1;
  arr[1] = 2;
  arr[2] = 3;
  arr[3] = 4;
  arr[4] = 5;

  print_array(arr,5);
  
  return 0;
}
