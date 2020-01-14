
struct sample {
  int a;
  int b;
};

int func(struct sample *sam){

  sam->b = 200;
  
  return 0;
}

int main(){

  struct sample sam;
  
  sam.a = 100;
  func(&sam);
  
  return sam.b;
}
