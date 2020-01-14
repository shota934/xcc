
typedef struct sample {
  int a;
  int b;
} sample_t;

int main(){
  
  int a;
  sample_t sam;
  sam.a = 20;
  a = sam.a;
  
  return a;
}
