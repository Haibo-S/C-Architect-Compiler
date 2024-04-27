; procedure height(): computes height of a binary tree
;
; parameter: $1 (base index), $4 (current index)
; MEM[$4] -> A[index] current Node (this is kinda useless)
; MEM[$4+4] -> left child index 
; MEM[$4+8] -> right child index
;
; return value: $3 -> return value (height of tree)
;
; base case: if(MEM[$4+4] == -1 && MEM[$4+8] == -1) return 0;
;
; inductive case: 
; if(MEM[$4+4] != -1 && MEM[$4+8] == -1) return height($1, $1 + 4*MEM[$4+4]);
; else if(MEM[$4+4] == -1 && MEM[$4+8] != -1) return height($1, $1 + 4*MEM[$4+4]);
; else return max(height($1, $1 + 4*MEM[$4+4]), height($1, $1 + 4*MEM[$4+4]));

add $4, $1, $0                  ; initalizes $4 with start address $1

height:
    ; save registers
    sw $1, -4($30)              ; param 1 
    sw $4, -8($30)              ; param 2

    sw $5, -12($30)             ; temp
    sw $6, -16($30)             ; temp
    sw $7, -20($30)             ; temp
    sw $8, -24($30)             ; temp
    sw $9, -28($30)             ; temp
    sw $10, -32($30)            ; temp
    sw $11, -36($30)            ; temp 
    sw $12, -40($30)            ; temp
    sw $13, -44($30)            ; temp 
    sw $14, -48($30)            ; temp
    sw $15, -52($30)            ; temp

    sw $31, -56($30)            ; return 

    lis $31
    .word 56
    sub $30, $30, $31

    ; procedure body
    lis $5
    .word -1
    lis $6
    .word 4
    lis $7
    .word 8
    lis $8
    .word 1

    lw $9, 4($4)
    lw $10, 8($4)
    add $11, $0, $0
    add $12, $0, $0
    lis $13
    .word height
    bne $9, $5, process1
    beq $0, $0, middle

process1:
    mult $9, $6
    mflo $4
    add $4, $1, $4
    jalr $13
    add $11, $3, $0

middle:
    bne $10, $5, process2
    beq $0, $0, process

process2:
    mult $10, $6
    mflo $4
    add $4, $1, $4
    jalr $13
    add $12, $3, $0

process:
    add $3, $8, $0
    lis $14
    .word -2
    add $15, $9, $10
    beq $15, $14, end
    slt $14, $11, $12       ; $14 = 1 if $11 < $12 else $14 = 0
    beq $14, $8, max1
    add $3, $11, $0
    add $3, $3, $8
    beq $0, $0, end


max1:
    add $3, $12, $0
    add $3, $3, $8

end:
    lis $31
    .word 56
    add $30, $30, $31

    lw $1, -4($30)              ; param 1 
    lw $4, -8($30)              ; param 2

    lw $5, -12($30)             ; temp
    lw $6, -16($30)             ; temp
    lw $7, -20($30)             ; temp
    lw $8, -24($30)             ; temp
    lw $9, -28($30)             ; temp
    lw $10, -32($30)            ; temp
    lw $11, -36($30)            ; temp 
    lw $12, -40($30)            ; temp
    lw $13, -44($30)            ; temp 
    lw $14, -48($30)            ; temp
    lw $15, -52($30)            ; temp

    lw $31, -56($30)            ; return 

    jr $31


