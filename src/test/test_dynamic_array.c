#include <stdio.h>
#include <stdlib.h>

#define N 10
int func();

int func(){

  int i;
  char *arr = malloc(sizeof(char) * N);
  char  sum;

  sum = 0;
  for(i = 0; i < N; i++){
	arr[i] = i + 1;
  }

  printf("[");
  for(i = 0; i < N; i++){
	printf(" %d ",arr[i]);
	sum = sum + arr[i];
  }
  printf("]\n");
  printf("%d\n",sum);
  if(sum == 55){
	printf("OK\n");
	return 0;
  } else {
	printf("NG\n");
	return 1;
  }
  
  free(arr);
  
  return 0;
}

int main(){

  int ret;
  ret = func();
  
  return ret;
}

