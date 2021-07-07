
struct sample {
  int a;
  int b;
};

int func(struct sample *p){
  return p->a + p->b;
}

int main(){

  struct sample sam;
  sam.a = 20;
  sam.b = 50;
  
  return func(&sam);
}
