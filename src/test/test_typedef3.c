
struct sample {
  int a;
  int b;
};

typedef struct sample  sample_t;

int func(int a,int b);
int func(int a,int b){

  sample_t sam;
  int c;

  sam.a = a;
  sam.b = b;
  
  c = sam.a + sam.b;
  
  return c;
}

int main(){

  int c;
  
  c = func(10,20);
  
  return c;
}
