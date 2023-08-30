.intel_syntax noprefix
.globl main
add:
  push rbp
  mov rbp, rsp
  sub rsp, 24
  push 2
  push 9
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  pop rax
  mov rsp, rbp
  pop rbp
  ret
main:
  push rbp
  mov rbp, rsp
  sub rsp, 24
  mov rax, rbp
  sub rax, 24
  push rax
  push 2
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  call add
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
