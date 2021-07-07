#include <stdio.h>

enum sample {
	 AA,
	 BB,
	 CC
};

int main(){

  enum sample sam;

  sam = CC;

  printf("%d\n",BB);
  printf("%d\n",sam);
  if(sam == 2){
	printf("OK");
	return 1;
  }
  
  return 0;
}
