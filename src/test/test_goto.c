
int main(){

  int i = 0;
  int ret;
  
  ret = 1;
 l0:
  if(i < 2){
	i = 3;
	ret = 0;
  } else {
	return ret;
  }
  goto l0;

  return ret;
}
