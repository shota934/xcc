
static int a = 40 + 2;

int main(){

  static int b = 30;

  a = a + b;
  
  return a;
}
