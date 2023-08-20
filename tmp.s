.intel_syntax noprefix
.globl main
main:
push 4
push 3
pop rdi
pop rax
  cmp rax, rdi
  setne al
  movzb rax, al
push rax
pop rax
ret
