

int func(int arr[3],int n){

  int a;
  
  arr[0] = 10;
  a = arr[0] + n;
  
  return a;
}

int main(){

  int arr[3];

  return func(arr,3);
}
