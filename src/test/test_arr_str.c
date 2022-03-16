#include <stdio.h>
#include <string.h>

int main(){

  char arr[6] = "Hello";
  int ret;

  if(strcmp(arr,"Hello") == 0){
	ret = 0;
  } else {
	ret = 1;
  }

  return ret;
}
