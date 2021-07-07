#include <stdio.h>
#include <string.h>

int main(){

  int a = sizeof( 2 < 3 );
  int ret = 0;
  
  if(a == 4){
	printf("OK\n");
  } else {
	printf("%d\n",a);
	ret = 1;
  }

  a = sizeof("Hello");
  if(strlen("Hello") == a){
	printf("OK\n");
  } else {
	printf("%d\n",a);
	ret = 2;
  }
  
  return ret;
}
