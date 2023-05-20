section .text

global _start

_start:
_scanfstart:
            push r11
            push r12
            push r13
            push r8
            push rax
            push r14
            push rdi
            xor rax, rax
            xor rdi, rdi
            mov rsi, r14
            mov rdx, 20
            syscall
            pop rdi
            pop r14
            pop rax
            pop r8
            pop r13
            pop r12
            pop r11

            push r14
            push rbx
            push rax
            call strlen
            xor rax, rax
            xor rdx, rdx
            xor r8, r8
            xor rcx, rcx
            call ReadBuffer

            mov [rdi], r8
            pop rax
            pop rbx
            pop r14
            ret

strlen:     xor rbx, rbx
            push r14

.Next:      mov eax, [r14]
            mov dl, al
            xor eax, eax
            mov al, dl
            cmp eax, 0
            sub eax, '0'
            cmp eax, 0
            jl .Done1
            cmp eax, 9
            jg .Done1

            add r14, 1
            add rbx, 1
            jmp .Next

.Done1:     sub rbx, 1
            pop r14
            ret


ReadBuffer:
.Next:      mov eax, [r14]
            mov dl, al
            xor eax, eax
            mov al, dl
            cmp eax, 0
            sub eax, '0'
            cmp eax, 0
            jl .Done
            cmp eax, 9
            jg .Done
            mov ch, 10

            push rbx
.pow:       cmp rbx, 0
            je .Don
            mul ch
            sub rbx, 1
            jmp .pow

.Don:       pop rbx
            sub rbx, 1
            add r8, rax
            add r14, 1
            jmp .Next
            .Done:

            ret
