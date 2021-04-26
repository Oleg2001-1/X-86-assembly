; Aleh Iotchanka
; Function for drawing(Setpixel)

section .text
global drawASM

drawASM:
    push    rbp
    mov     rbp, rsp
    mov     r9, rdx             ;we write the address of the text in rejestr nr 9
    mov     rdx, 0              ;rdx - mod_iter
    mov     r10, rcx            

draw_loop:
    mov     al, BYTE[r9]       
    cmp     al, 0
    jz      end                 ;if /0 then jump to end
    cmp     al, '1'
    jne     do_if_zero          ; if not '1' then jump
    mov     rdx, 0              ; resets of mod_iter
mod_loop:
    mov     rcx, 0x0007  ; set_mask
    and     rcx, r10
    mov     r8, 0x0080
    shr     r8, cl
    not     r8
    mov     rcx, r10    ; get_pixel
    shr     rcx, 3
    mov     rax, rdi  ; set_pixel
    add     rax, rcx
    and     QWORD[rax], r8
    inc     r10                ; mod_loop_end
    inc     rdx
    mov     rax, rsi
    cmp     rdx, rax
    jb      mod_loop

draw_loop_end:

    inc     r9
    jmp     draw_loop

do_if_zero:
    mov     rax, rsi
    add     r10, rax            ; we add mod_width to x
    inc     r9
    jmp     draw_loop

end:
    pop     rbp
    ret
