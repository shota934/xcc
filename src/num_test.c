#include <stdio.h>

enum sample {
			 AA = 0,
			 BB = AA + 1,
};

int main(){

  printf("%d\n",AA);
  printf("%d\n",BB);
  
  return 0;
}
