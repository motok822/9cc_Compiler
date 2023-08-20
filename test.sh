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

assert 0 '0;'
assert 42 '42;'
assert 8 '3+5;'
assert 47 '5+6*7;'
assert 15 '5*(9-6);'
assert 4 '(3+5)/2;'
assert 21 '-7*-3;'
assert 1 '5==(2+3);'
assert 1 '9>=(2*3+2);'
assert 1 '4!=3;'
assert 3 'a=3;'
assert 24 'a=4;b=5;a*b+a;'
echo OK