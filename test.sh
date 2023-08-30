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

assert 0 'main(){0;}'
assert 42 'main(){42;}'
assert 8 'main(){3+5;}'
assert 47 'main(){5+6*7;}'
assert 15 'main(){5*(9-6);}'
assert 4 'main(){(3+5)/2;}'
assert 21 'main(){-7*-3;}'
assert 1 'main(){5==(2+3);}'
assert 1 'main(){9>=(2*3+2);}'
assert 1 'main(){4!=3;}'
assert 3 'main(){a=3;}'
assert 24 'main(){apple=4;ball=5;apple*ball+apple;}'
assert 56 'main(){a=56;return a;}'
assert 2 'main(){if(2==2) a=2;}'
assert 5 'main(){
  a = 4;  
  if(a==3){
    a=4;
  }else {
    a=5;
  }
}
'
assert 5 '
main(){
  a = 0;
  while(a <= 4){
    a = a + 2;
    a = a - 1;
    a = a + 1;
    a = a - 1;
  }
}
'
assert 5 '
main(){
  for(i = 0;i <= 4;i = i+1){
    i = i + 1;
    i = i - 1;
  }
}'
assert 6 '
main(){
  {
    x = 2;
    y = 4;
    return x + y;
  }
}'

assert 11 '

add(){
  return 2+9;
}
main(){
  x = 2;
  add();
}
'
echo OK