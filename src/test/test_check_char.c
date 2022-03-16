#include <stdio.h>

int isAlpha(char ch);
int test();

int isAlpha(char ch){

  if(('A' <= ch) && (ch <= 'Z')){
	return 1;
  } else {
	return 0;
  }
}

int test(){

  if(isAlpha('B')){
	return 0;
  }
  
  return 1;
}

int main(){
  
  if(!test()){
	printf("OK\n");
	return 0;
  } else {
	printf("NG\n");
	return 1;
  }
}
