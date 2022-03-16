#include <stdio.h>

void swap(int i,int j,int arr[]);
void sort(int arr[],int n);
void print_arr(int arr[],int n);
int test(int arr[],int n);

void swap(int i,int j,int arr[]){
  
  int tmp;
  tmp = arr[i];
  arr[i] = arr[j];
  arr[j] = tmp;
	
  return;
}

void sort(int arr[],int n){

  int i;
  int j;
  int k;
  int min;

  for(i = 0; i < n - 1; i++){
	min = arr[i];
	k = i;
	for(j = i + 1; j < n; j++){
	  if(min > arr[j]){
		min = arr[j];
		k = j;
	  }
	}
	if(k != i){
	  swap(i,k,arr);
	}
  }
  
  return;
}

void print_arr(int arr[],int n){

  int i;
  printf("[");
  for(i = 0; i < n; i++){
	printf(" %d ",arr[i]);
  }
  printf("]\n");
  
  return;
}


int test(int arr[],int n){

  int i;

  for(i = 0; i < n - 1; i++){
	if(arr[i] > arr[i + 1]){
	  return 1;
	}
  }
  
  return 0;
}

int main(){

  int arr[] = {10,9,8,7,6,5,4,3,2,1};
  print_arr(arr,sizeof(arr) / sizeof(arr[0]));
  sort(arr,sizeof(arr) / sizeof(arr[0]));
  print_arr(arr,sizeof(arr) / sizeof(arr[0]));

  int ret = test(arr,sizeof(arr) / sizeof(arr[0]));
  if(ret){
	printf("NG\n");
	return 1;
  }

  printf("Passed\n");
  
  return 0;
}
