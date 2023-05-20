section .text
global main
extern scanf
extern printf
main:
mov r11, Buffer
push r11
push rbp
push rsp
mov rdi, Message
call printf
pop rsp
pop rbp
pop r11
add r11, 72
.block0
mov rax, 105
mov rbx, 95
sub rax, rbx
mov [r11 + 48], rax
mov rax, [r11 + 48]
mov [r11 + 8], rax
mov rax, r11
add rax, 8
push rax
pop rax
push r11
push rbp
push rsp
mov rdi, formatin
mov rsi, rax
pop rsp
pop rbp
pop r11
mov [r11 + 56], rax
mov rax, 0
mov [r11 + 24], rax
mov rax, [r11 + 8]
push rax
call faktorial
mov [r11 + 64], rax
mov rax, [r11 + 64]
mov [r11 + 24], rax
mov rax, [r11 + 24]
push rax
pop rax
push r11
push rbp
push rsp
mov rdi, formatout
pop rsp
pop rbp
pop r11
mov [r11 + 72], rax
mov rax, 0x3c
xor rdi, rdi
syscall
faktorial
add r11, 56
.block0
pop rbx
pop rax
mov [r11 + 8], rax
push rbx
mov rax, 1
mov [r11 + 16], rax
mov rax, [r11 + 8]
mov rbx, 1
sub rax, rbx
mov [r11 + 32], rax
cmp DWORD [r11 + 32], 0
jne .block1
jmp .block2
.block1
mov rax, [r11 + 8]
mov rbx, 1
sub rax, rbx
mov [r11 + 40], rax
mov rax, [r11 + 40]
push rax
call faktorial
mov [r11 + 48], rax
mov rax, [r11 + 48]
mov rbx, [r11 + 8]
mul rbx
mov [r11 + 56], rax
mov rax, [r11 + 56]
mov [r11 + 16], rax
jmp .block3
.block2
mov rax, 1
mov [r11 + 16], rax
.block3
mov rax, [r11 + 16]
sub r11, 56
ret
.block4
section .data
Buffer: times 4096 db 0x00
formatin: db "%d", 0x00
formatout: db "%d", 0x0a, 0x00
Message: db "HelloWorld!", 0x0a, 0x00section .text
global main
extern scanf
extern printf
main:
mov r11, Buffer
push r11
push rbp
push rsp
mov rdi, Message
call printf
pop rsp
pop rbp
pop r11
add r11, 72
.block0
mov rax, 105
mov rbx, 95
sub rax, rbx
mov [r11 + 48], rax
mov rax, [r11 + 48]
mov [r11 + 8], rax
mov rax, r11
add rax, 8
push rax
pop rax
push r11
push rbp
push rsp
mov rdi, formatin
mov rsi, rax
pop rsp
pop rbp
pop r11
mov [r11 + 56], rax
mov rax, 0
mov [r11 + 24], rax
mov rax, [r11 + 8]
push rax
call faktorial
mov [r11 + 64], rax
mov rax, [r11 + 64]
mov [r11 + 24], rax
mov rax, [r11 + 24]
push rax
pop rax
push r11
push rbp
push rsp
mov rdi, formatout
pop rsp
pop rbp
pop r11
mov [r11 + 72], rax
mov rax, 0x3c
xor rdi, rdi
syscall
faktorial
add r11, 56
.block0
pop rbx
pop rax
mov [r11 + 8], rax
push rbx
mov rax, 1
mov [r11 + 16], rax
mov rax, [r11 + 8]
mov rbx, 1
sub rax, rbx
mov [r11 + 32], rax
cmp DWORD [r11 + 32], 0
jne .block1
jmp .block2
.block1
mov rax, [r11 + 8]
mov rbx, 1
sub rax, rbx
mov [r11 + 40], rax
mov rax, [r11 + 40]
push rax
call faktorial
mov [r11 + 48], rax
mov rax, [r11 + 48]
mov rbx, [r11 + 8]
mul rbx
mov [r11 + 56], rax
mov rax, [r11 + 56]
mov [r11 + 16], rax
jmp .block3
.block2
mov rax, 1
mov [r11 + 16], rax
.block3
mov rax, [r11 + 16]
sub r11, 56
ret
.block4
section .data
Buffer: times 4096 db 0x00
formatin: db "%d", 0x00
formatout: db "%d", 0x0a, 0x00
Message: db "HelloWorld!", 0x0a, 0x00