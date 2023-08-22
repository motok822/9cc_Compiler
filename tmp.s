.intel_syntax noprefix
.globl main
main:
push rbp
mov rbp, rsp
sub rsp, 48
mov rax, rbp
sub rax, 8
push rax
push 0
pop rdi
pop rax
mov [rax], rdi
push rdi
.Lbegin0:
mov rax, rbp
sub rax, 8
push rax
pop rax
mov rax, [rax]
push rax
push 4
pop rdi
pop rax
cmp rax, rdi
setle al
movzb rax, al
push rax
pop rax
cmp rax, 0
je .Lend0
mov rax, rbp
sub rax, 8
push rax
mov rax, rbp
sub rax, 8
push rax
pop rax
mov rax, [rax]
push rax
pop rdi
pop rax
mov [rax], rdi
push rdi
mov rax, rbp
sub rax, 8
push rax
mov rax, rbp
sub rax, 8
push rax
pop rax
mov rax, [rax]
push rax
push 1
pop rdi
pop rax
add rax, rdi
push rax
pop rdi
pop rax
mov [rax], rdi
push rdi
jmp .Lbegin0
.Lend0:
pop rax
mov rsp, rbp
pop rbp
ret
