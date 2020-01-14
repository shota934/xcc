#define N 100
#define A 1
#define B 2

int func(int a,int b);

int func(int a,int b){

  int c;

  c = a + b * N;

  return c;
}

int main(){

  int c;
  
  c = func(A,B);

  return c;
}
