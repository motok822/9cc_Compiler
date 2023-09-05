#!/bin/bash
assert() {
  expected="$1"
  input="$2"

  ./9cc "$input" > tmp.s
  cc -o tmp tmp.s
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

assert 0 'int main(){0;}'
assert 42 'int main(){42;}'
assert 8 'int main(){3+5;}'
assert 47 'int main(){5+6*7;}'
assert 15 'int main(){5*(9-6);}'
assert 4 'int main(){(3+5)/2;}'
assert 21 'int main(){-7*-3;}'
assert 1 'int main(){5==(2+3);}'
assert 1 'int main(){9>=(2*3+2);}'
assert 1 'int main(){4!=3;}'
assert 3 'int main(){int a=3;}'
assert 24 'int main(){int apple=4;int ball=5;apple*ball+apple;}'
assert 56 'int main(){int a=56;return a;}'
assert 2 'int main(){if(2==2) int a=2;}'
assert 5 '
int main(){
  int a = 4;  
  if(a==3){
    a=4;
  }else {
    a=5;
  }
}
'
assert 5 '
int main(){
  int a = 0;
  while(a <= 4){
    a = a + 2;
    a = a - 1;
    a = a + 1;
    a = a - 1;
  }
}
'
assert 5 '
int main(){
  for(int i = 0;i <= 4;i = i+1){
    i = i + 1;
    i = i - 1;
  }
}'
assert 6 '
int main(){
  {
    int x = 2;
    int y = 4;
    return x + y;
  }
}'

assert 11 '
int add(){
  return 2+9;
}
int main(){
  int x = 2;
  add();
}
'

assert 12 '
int add(int x, int y){
  if(x <= y){
    return x+y;
  }else{
    return x*y;
  }
}
int main(){
  add(2, 10);
}'

assert 8 '
int fib(int i){
  if(i<=1){
    return i;
  }
  return fib(i-1) + fib(i-2);
}
int main(){
  fib(6);
}
'
assert 5 '
int main(){
  int a = 5;
  int x = &a;
  return *x;
}
'
echo OK