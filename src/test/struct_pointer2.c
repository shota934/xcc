
struct sample {
  int a;
  int b;
};

int func(struct sample *sam){
  return sam->b;
}

int main(){

  struct sample sam;
  
  sam.a = 100;
  sam.b = 500;

  return func(&sam);
}
