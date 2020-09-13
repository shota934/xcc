struct list_t {
  int num;
  struct list_t *next;
};

struct list_t *make();

struct list_t *make(){
  
  struct list_t *p;

  p = malloc(sizeof(struct list_t));
  
  return p;
}

int main(){

  struct list_t *q;

  q = make();
  q->num = 200;
  
  return q->num;
}


