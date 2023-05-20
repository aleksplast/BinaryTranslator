section .text

global _start

_start:
_Printstart:

Dspecif:
            push rax
            push rbx
            push rcx
            push rdx
            push rsi
            push rax
            mov al, 0xa
            mov byte [r13], al
            sub r13, 1
            pop rax

            xor rcx, rcx
            xor rdx, rdx

            cmp eax, 0
            jge .Next3
            mov byte [r13], '-'
            inc r13
            neg eax

            .Next3:
            xor dx, dx
            cmp ax, 2560
            jb .Less

            mov cl, 10
            xor ch, ch
            div cx
            add dl, '0'
            mov byte [r13], dl
            sub r13, 1

            jmp .Next3

            .Less: cmp ax, 0
            je .Done3

            mov cl, 10
            xor ch, ch
            div cl
            push rax
            xchg ah, al
            add al, '0'
            mov byte [r13], al
            sub r13, 1
            pop rax

            xor ah, ah
            jmp .Less

            .Done3:

            call PrintExtraBuff
            pop rsi
            pop rdx
            pop rcx
            pop rbx
            pop rax

            ret

PrintExtraBuff: push rax
            push rdi
            push rsi
            push rdx
            push r12

            mov rax, 0x01
            mov rdi, 1
            mov rsi, r13
            mov rdx, r12
            sub rdx, r13
            syscall

            pop r12
            mov r13, r12
            pop rdx
            pop rsi
            pop rdi
            pop rax

            ret
