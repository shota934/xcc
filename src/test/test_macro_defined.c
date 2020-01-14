#define T
#define T1

#if defined T1 && defined T2
#define A 10
#else 
#define A 30
#endif

#if defined(T1) || defined(T2)
#define B 50
#else
#define B 100
#endif

int main(){
  return A + B;
}
