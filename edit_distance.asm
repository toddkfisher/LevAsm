        ROWS equ 8
        COLS equ 8



section .bss
edit: resb ROWS*COLS



section .text
global edit_distance


;; uint8_t string_length(char *rdi)
string_length:
        mov rax, 0
        push rdi
.loop:
        mov r15b, byte [rdi]
        cmp r15b, 0
        je .endloop
        inc rax
        inc rdi
        jmp .loop
.endloop:
        pop rdi
        ret


;; uint8_t edit_distance(char *a (rdi), char *b (rsi))
edit_distance:
;; Locals:
;;              C VARIABLE              REGISTER
;;              ----------              --------
;; uint8_t      len_a = strlen(a);      (r8)
;; uint8_t      len_b = strlen(b);      (r9)
;; uint8_t      *rc;                    (r10)
;; uint8_t      *backidx;               (r11)
;; uint8_t      idr_min;                (r12)
;; uint8_t      n;                      (r13)
;; uint8_t      row;                    (r14)
;; uint8_t      col;                    (r15)

;; len_a = strlen(a)
        call string_length
        mov r8, rax
;; len_b = strlen(b)
        push rdi
        mov rdi, rsi
        call string_length
        mov r9, rax
        pop rdi
;; edit[0][col] = col
        lea r10, [edit]
        mov r15, 0
        mov rax, COLS
.loop0:
        cmp r15, rax
        je .end_loop0
        mov byte [r10], r15b
        inc r15
        inc r10
        jmp .loop0
.end_loop0:
;; edit[row][0] = row;
        mov r14, 0
        lea r10, [edit]
        mov rax, ROWS
.loop1:
        cmp r14, rax
        je .end_loop1
        mov byte [r10], r14b
        inc r14
;; r10 = addr of next row of edit
        add r10, COLS
        jmp .loop1
.end_loop1:
;; rc = edit + COLS + 1 (== edit[1][1])
        lea r10, [edit]
        add r10, COLS
        inc r10
;; row = 1
        mov r14, 1
.loop2:                         ; outer do loop
;; col = 1
        mov r15, 1
.loop3:                         ; inner do loop
;; backindex: row, col - 1
;; backidx = rc - 1  (okay since col >= 1)
        mov r11, r10
        dec r11
;; idr_min = *backidx + 1
        movsx r12, byte [r11]
        inc r12
;; backindex: row - 1, col
;; backidx = rc - COLS
        mov r11, r10
        mov rax, COLS
        sub r11, rax
;; n = *backidx + 1
        movsx r13, byte [r11]
        inc r13
;; if (n < idr_min)
        cmp r13, r12
        jge .if0
;; idr_min = n
        mov r12, r13
.if0:
;; backindex: row - 1, col - 1
;; n = *rc[backindex] (== edit[row - 1][col - 1])
        dec r11
        movsx r13, byte [r11]
;; if (a[row - 1] == b[row - 1])
        movsx rax, byte [rdi + r14 - 1]
        movsx rbx, byte [rsi + r15 - 1]
        cmp rax, rbx
        jne .if1
;; if (n < idr_min)
        cmp r13, r12
        jge .if2
;; idr_min = n
        mov r12, r13
.if2:
        jmp .if3
.if1:
;; n += 1 (== edit[row - 1][col - 1] + 1)
        inc r13
;; if (n < idr_min)
        cmp r13, r12
        jge .if3
;; idr_min = n
        mov r12, r13
.if3:
;; *rc = idr_min
        mov byte [r10], r12b
;; col += 1
        inc r15
;; rc += 1
        inc r10
;; col <= len_b ?
        cmp r15, r9
        jle .loop3
;; row += 1
        inc r14
;; rc = edit + row*COLS + 1
        mov rax, COLS
        mul r14
        lea r10, [edit + rax + 1]
;; row <= len_a ?
        cmp r14, r8
        jle .loop2
;; return *(edit + len_a*COLS + len_b)
        mov rax, COLS
        mul r8
        add rax, r9
        movsx rax, byte [edit + rax]
        ret
