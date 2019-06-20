;==============================================
;Coursework: Forthress, a Forth dialect
;
;Student - Smirnova Anastasia, P3211 
;ITMO University, 2019
; 
;base dictionary - the first part of dictionary
;==============================================

;	!!!	PART 2	!!!
;		TASK 1	

;--------------------------------------------
;--------------------------------------------
; 1 - STACK COMBINATORS (drop, dup, swap)

;--------------------------------------------
;delete one number from stack (-- a)
;--------------------------------------------
native "drop", drop
    add rsp, 8
    jmp next

;--------------------------------------------
; echange first and second numbers (a b -- b a)
;--------------------------------------------
native "swap", swap
    pop rax
    pop rdx
    push rax
    push rdx
    jmp next

;--------------------------------------------
;duplicate the number in the top of the stack (a -- a a)
;--------------------------------------------
native "dup", dup
    push qword [rsp]
    jmp next


;--------------------------------------------
;--------------------------------------------
; 2 - MATH (+ * / % - = < )

;--------------------------------------------
; add (a b -- [a+b])
;--------------------------------------------
native "+", plus
    pop rax
    add [rsp], rax
    jmp next

;--------------------------------------------
; mul (a b -- [a*b])
;--------------------------------------------
native "*", mul
    pop rax
    pop rdx
    imul rdx
    push rax
    jmp next

;--------------------------------------------
; div (a b -- [b/a]) 
;--------------------------------------------
native "/", div
    pop rcx
    pop rax
    cqo
    idiv rcx
    push rax
    jmp next

;--------------------------------------------
; mod (a b -- [b mod a] ~~ b%a) 
;--------------------------------------------
native "%", mod
    pop rcx
    pop rax
    cqo
    idiv rcx
    push rdx
    jmp next

	
;--------------------------------------------
; minus (a b -- [b-a]) 
;--------------------------------------------
native "-", minus
    pop rax
    sub [rsp], rax
    jmp next

;--------------------------------------------
; equality (a b -- [a==b])
;--------------------------------------------
native "=", equals
    pop rax
    pop rdx
    cmp rax, rdx
    sete al
    movzx rax, al
    push rax
    jmp next

;--------------------------------------------
; compare (a b -- [b<a])
;--------------------------------------------
native "<", lt
    pop rdx
    pop rax
    cmp rax, rdx
    setl al
    movzx rax, al
    push rax
    jmp next



;--------------------------------------------
;--------------------------------------------
; 3 - LOGIC (not, and, or, land, lor)

;--------------------------------------------
; ( a -- not(a) )
;--------------------------------------------
native "not", not
    pop rax
    test rax, rax
    setz al
    movzx rax, al
    push rax
    jmp next

;--------------------------------------------
; (a b -- [ a & b ]) !!! bitwise !!!
;--------------------------------------------
native "and", and
    pop rax
    and [rsp], rax
    jmp next

;--------------------------------------------
; (a b -- [ a | b ]) 
;--------------------------------------------
native "or", or
    pop rax
    or [rsp], rax
    jmp next

;--------------------------------------------
; (a b -- [ a && b ]) !!! logic !!!
;--------------------------------------------
native "land", land
    pop rax
    pop rdx
    test rax, rax
    jz .no
    push rdx
    jmp next
.no:
    push rax
    jmp next


;--------------------------------------------
; (a b -- [ a || b ]) !!! logic !!!
;--------------------------------------------
native "lor", lor
    pop rax
    pop rdx
    test rax, rax
    jnz .yes
    push rdx
    jmp next
.yes:
    push rax
    jmp next

;--------------------------------------------
;--------------------------------------------
; 4 - STACK (. ; .S)

;--------------------------------------------
; (a --) ; without print a 
;--------------------------------------------
native ".", dot
    pop rdi
    call print_int
    jmp next

;--------------------------------------------
; (a --) ; print a 
;--------------------------------------------
native ".S", show_stack
    mov rcx, rsp
    .loop:
        cmp rcx, [stack_base]
        jae next
        mov rdi, [rcx]
        push rcx
        call print_int
        call print_newline
        pop rcx
        add rcx, 8
        jmp .loop

section .data
    stack_base: dq 0

;--------------------------------------------
;--------------------------------------------
; 5 - WORK WITH RSTACK  (>r, r>, r@)

;--------------------------------------------
; >r - from data stack to r_stack 
;--------------------------------------------
native ">r", to_r
    pop rax
    rpush rax
    jmp next

;--------------------------------------------
; r> - from r_stack to data stack 
;--------------------------------------------
native "r>", from_r
    rpop rax
    push rax
    jmp next


;--------------------------------------------
; r@ - from r_stack to data stack; BUT save in r_stack
;--------------------------------------------
native "r@", r_fetch
    push qword [rstack]
    jmp next


;--------------------------------------------
;--------------------------------------------
; 6 - manage (docol, branch 0branch)


;--------------------------------------------
; docol ; implements colon-words
;--------------------------------------------
native "docol", docol
    rpush pc
    add w, 8
    mov pc, w
    jmp next

;--------------------------------------------
; branch
;--------------------------------------------
native "branch", branch
    mov pc, [pc]
    jmp next

;--------------------------------------------
; 0branch 
;--------------------------------------------
native "0branch", branch0
    pop rax
    test rax, rax
    jnz .skip
    mov pc, [pc]
    jmp next
    .skip:
    add pc, 8
    jmp next



;--------------------------------------------
;--------------------------------------------
; 7 - syscall ( call_num a1 a2 a3 a4 a5 a6 -- new_rax new_rdx )
native "syscall", syscall
    pop r9
    pop r8
    pop r10
    pop rdx
    pop rsi
    pop rdi
    pop rax
    syscall
    push rax
    push rdx
    jmp next




;--------------------------------------------
;--------------------------------------------
; 8 - WORK WITH MEMORY (@, !, c!, c@)

;--------------------------------------------
; (addr -- value) get the value from memory   
;--------------------------------------------
native "@", fetch
    pop rax
    push qword[rax]
    jmp next

;--------------------------------------------
; (val addr --) save the value using address 
;--------------------------------------------
native "!", write
    pop rax
    pop rdx
    mov [rax], rdx
    jmp next

;--------------------------------------------
; (char addr --) save the char (1 byte) using address 
;--------------------------------------------
native "c!", write_char
    pop rax
    pop rdx
    mov [rax], dl
    jmp next

;--------------------------------------------
; (addr -- char)  read char (1 byte) from memory  
;--------------------------------------------
native "c@", fetch_char
    pop rax
    movzx rax, byte [rax]
    push rax
    jmp next


;--------------------------------------------
;--------------------------------------------
; 9 - SERVICE - LIT


native "lit", lit
    push qword [pc]
    add pc, 8
    jmp next


