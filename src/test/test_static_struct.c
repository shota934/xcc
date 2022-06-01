
struct sample{
  int a;
  int b;
};

int main(){

  static struct sample sam;
  sam.b = 10;
  sam.a = 2;

  int a = sam.a + sam.b;
  if(a == 12){
	return 0;
  } else {
	return 1;
  }
}
