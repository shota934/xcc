#include <stdio.h>

int main(){

  int i;
  int j;
  int sum;
  int arr[3][2] = {{1,2},{3,4},{5,6}};

  sum = 0;
  for(i = 0; i < 3; i++){
	for(j = 0; j < 2; j++){
	  printf("%d\n",arr[i][j]);
	  sum += arr[i][j];
	}
  }

  if(sum == 21){
	return 0;
  } else {
	return 1;
  }
}
