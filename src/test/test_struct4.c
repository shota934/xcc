


struct sample{
  int a;
  int b;
  double c;
  double d;
};

int func(struct sample sam){
  return sam.a + sam.b;
}

int main(){

  struct sample sam;
  
  sam.a = 20;
  sam.b = 30;
  
  return func(sam);
}


