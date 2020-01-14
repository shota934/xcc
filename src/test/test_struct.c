
struct sample {
  int a;
  int b;
};

int func(struct sample sammp1){
  return sammp1.a;
}

int main(){

  struct sample sammp1;
  
  sammp1.a = 10;
  sammp1.b = 20;

  return func(sammp1);
}
