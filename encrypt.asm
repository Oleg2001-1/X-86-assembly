; Aleh Iotchanka
; Encrypt of a string of decimal characters to binary form for further drawing

section .data
;binary representation of barcode digits
    cipher:    db  '0001101001100100100110111101010001101100010101111011101101101110001011', 70
			     ;0001101     0
			     ;0011001     1
			     ;0010011     2
			     ;0111101     3
			     ;0100011     4
			     ;0110001     5
			     ;0101111     6
			     ;0111011     7
			     ;0110111     8
			     ;0001011     9
section .text
global encryptASM

;Przekodowanie
encryptASM:
    push    rbp
    mov     rbp, rsp
    add     rsi, 3              ; moves three sings beyond the start of binary
    mov     r9, 0

encrypt_loop:
    mov     rax, 0
    mov     al, BYTE[rdi]      
    cmp     al, 0
    je      end                 ; if /0 jump to end
    cmp     r9, 4               ; checks if its the middle ground arleady
    jne     encrypt_loop_cd
    add     rsi, 5
    mov     r11b, 1

encrypt_loop_cd:
    inc     rdi
    sub     al, '0'             ; changes char into int
    mov     r8b, al
    shl     al, 3
    sub     al, r8b
    mov     rcx, cipher
    add     rcx, rax            ; gets the start of the correct number from cipher
    mov     r8b, 0

loop_7:
    mov     al, BYTE[rcx]
    cmp     r11b, 1             ; checks the r11b flag if it should be C arleady
    jne     encrypt_A
    cmp     al, '0'
    je      change_to_one

change_to_zero:
    sub     al, 2       

change_to_one:
    add     al, 1

encrypt_A:
    mov     BYTE[rsi], al
    inc     rcx
    inc     rsi
    inc     r8b
    cmp     r8b, 7
    jb      loop_7
    inc     r9
    jmp     encrypt_loop
    
end:
    pop     rbp
    ret
