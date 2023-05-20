section .text

global _start

_start:

;--------------------------------------
; Entry: RDI = number to printf
; Exit: NONE
; Expects: R13, R12 is the write buffer ptr
; Destroys: RAX, RBX, RCX, RDX, RSI, RDI
;--------------------------------------

_Printstart:

Dspecif:
            push rax
            push rbx
            push rcx            ; saving registers from syscall
            push rdx
            push rsi
            push rax
            mov al, 0xa         ; adding \n to the end
            mov byte [r13], al  ; moving it to the buffer
            sub r13, 1          ; next symbol
            pop rax

            xor rcx, rcx        ; rcx = 0 for later use in calculation
            xor rdx, rdx        ; rdx = 0 for later use in calculation

            call CnvStrToBuf    ; convert int to str, ptr in r13

            call PrintExtraBuff ; printing
            pop rsi
            pop rdx
            pop rcx             ; restore registers
            pop rbx
            pop rax

            ret

;--------------------------------------
; Entry: RDI = destination pointer
; Exit: RDI = input num
; Expects: R14 is the write buffer ptr
; Destroys: RAX, RCX, R8
;--------------------------------------

PrintExtraBuff:
            push rax
            push rdi
            push rsi
            push rdx            ; saving registers from syscall
            push r12

            mov rax, 0x01       ; syscall write
            mov rdi, 1          ; stdout descriptor
            mov rsi, r13        ; r13 is the write buffer ptr
            mov rdx, r12
            sub rdx, r13        ; calculating strlen
            syscall

            pop r12             ; restore write buffer ptr
            mov r13, r12        ; r13 = r12 for use in later printfs
            pop rdx
            pop rsi             ; restoring registers
            pop rdi
            pop rax

            ret


;--------------------------------------
; Entry: RDI = destination pointer
; Exit: RDI = input num
; Expects: R14 is the write buffer ptr
; Destroys: RAX, RCX, R8
;--------------------------------------

CnvStrToBuf:
            cmp eax, 0          ; check for nagative
            jge .Next3
            mov byte [r13], '-' ; placing - in write buffer
            inc r13
            neg eax             ; converting num to positive

            .Next3:
            xor dx, dx          ; dx for div use
            cmp ax, 2560        ; two variants of div
            jb .Less

            mov cl, 10          ; 10 in cl for div
            xor ch, ch          ; ch = 0
            div cx
            add dl, '0'         ; transformating symbol
            mov byte [r13], dl  ; place it in the buffer
            sub r13, 1          ; next symbol

            jmp .Next3

            .Less: cmp ax, 0    ; if conversion is over
            je .Done3

            mov cl, 10          ; 10 in cl for div
            xor ch, ch          ; ch = 0
            div cl              ; transformating symbol
            push rax            ; save rax
            xchg ah, al         ; change al and ah for print
            add al, '0'         ; coverting symbol
            mov byte [r13], al  ; storing to string
            sub r13, 1          ; next symbol
            pop rax             ; restore rax

            xor ah, ah          ; destroy bigger number
            jmp .Less

            .Done3:
