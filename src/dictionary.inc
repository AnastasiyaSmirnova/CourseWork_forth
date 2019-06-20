;==============================================
;Coursework: Forthress, a Forth dialect
;
;Student - Smirnova Anastasia, P3211 
;ITMO University, 2019
; 
;dictionary - the optional part of dictionary
;==============================================


;read a word and save it in the data stack 
colon "'", tick, 1
	dq xt_inbuf
	dq xt_word
	dq xt_drop
	dq xt_inbuf
	dq xt_find, xt_dup
    branch0 .no_such_word ; see below ! it's not a msg ! 
	dq xt_cfa
    dq xt_state, xt_fetch
    branch0 .interpret
    dq xt_lit, xt_lit
    dq xt_comma
    dq xt_comma
.interpret:
	dq xt_exit
    .no_such_word:
    dq xt_inbuf, xt_prints
    dq xt_lit, msg_no_such_word, xt_prints
    dq xt_exit


; Initialization
native "init", init
    mov qword [state], 0
    mov rstack, rstack_start
    mov pc, interpreter_stub
    cmp qword [stack_base], 0
    je  .first
    mov rsp, [stack_base]
    jmp next
    .first:
    mov [stack_base], rsp
    jmp next


native "exit", exit
    rpop pc
    jmp next


; ( str -- header_addr )
native "find", find
    mov rsi, [last_word]

    ; if compiling, we have to skip the word being defined
    mov rax, [state]
    test rax, rax
    jz .loop
    mov rsi, [rsi]
.loop:
    mov rdi, [rsp]
    push rsi
    add rsi, 9
    call string_equals
    pop rsi
    test rax, rax
    jnz .found

    mov rsi, [rsi]
    test rsi, rsi
    jnz .loop
.not_found:
    mov qword [rsp], 0
    jmp next
.found:
    mov [rsp], rsi
    jmp next

; ( word_addr -- xt )
; Converts word header start address to the
; execution token
native "cfa", cfa
    pop rsi
    add rsi, 9
    .loop:
    mov al, [rsi]
    test al, al
    jz .end
    inc rsi
    jmp .loop

    .end:
    add rsi, 2
    push rsi
    jmp next


; ( c -- )
; Outputs a single character to stdout
native "emit", emit
    pop rdi
    call print_char
    jmp next

; ( addr -- len )
; Reads word from stdin and stores it starting at address
; Word length is pushed into stack
native "word", word
    pop rdi
    call read_word
    push rdx
    jmp next

; ( str -- len num )
; Parses an integer from string
native "number", number
    pop rdi
    call parse_int
    push rax
    push rdx
    jmp next

; null-terminated string length
colon "count", count
     dq xt_dup
 .loop:
     dq xt_dup
     dq xt_fetch_char
     branch0 .exit
     dq xt_lit, 1, xt_plus
     branch .loop
 .exit:
     dq xt_swap, xt_minus, xt_exit


; prints a certain amount of symbols from string
; ( str cnt -- )
colon "printc", printc
    dq xt_to_r
.loop:
    dq xt_r_fetch
    branch0 .exit
    dq xt_dup, xt_fetch_char, xt_emit
    dq xt_lit, 1, xt_plus
    dq xt_from_r, xt_lit, 1, xt_minus, xt_to_r
    branch .loop

    .exit:
    dq xt_from_r, xt_drop, xt_drop, xt_exit


; ( addr len -- )
; Prints a null-terminated string
colon "prints", prints
     dq xt_dup, xt_count, xt_printc, xt_exit

; Exits Forthress
colon "bye", bye
   dq xt_lit, 60
   dq xt_lit, 0
   dq xt_lit, 0
   dq xt_lit, 0
   dq xt_lit, 0
   dq xt_lit, 0
   dq xt_lit, 0
   dq xt_syscall



const inbuf, input_buf ; Address of the input buffer (is used by interpreter/compiler)
const mem, user_mem ; Address of user memory.
const last_word, last_word
const state, state ; The state cell stores either 1 (compilation mode) or 0 (interpretation mode)

const here, [here]
const in_fd, in_fd
const dp, dp

; ( xt -- )
native "execute", execute
    pop rax
    mov w, rax
    jmp [rax]


; ( x -- )
; Add x to the word being defined
native ",", comma
    mov rax, [here]
    pop qword [rax]
    add qword [here], 8
    jmp next

; ( c -- )
; Add a single byte to the word being defined
native "c,", char_comma
    mov rax, [here]
    pop rdx
    mov [rax], dl
    add qword[here], 1
    jmp next


; ( flags name --  )
native "create", create
    ; link
    mov rcx, [last_word]
    mov rsi, [here]
    mov [rsi], rcx
    mov [last_word], rsi
    add rsi, 8
    mov byte [rsi], 0
    inc rsi

    ; name
    pop rdi
    push rsi
    call string_copy
    pop rsi
    push rsi
    mov rdi, rsi
    call string_length
    pop rsi
    add rsi, rax
    ; flags
    inc rsi
    pop rax
    mov [rsi], al
    inc rsi

    mov [here], rsi
    jmp next

;--------------------------------------------
; CREATING NEW WORDS - forth (: .... ;)
; Read word from stdin and start defining it
colon ":", colon
    .restart:
    dq xt_inbuf, xt_word
    branch0 .restart
    dq xt_lit, 0, xt_inbuf, xt_create
    dq xt_lit, 1, xt_state, xt_write
    dq xt_lit, i_docol, xt_comma
    dq xt_exit

; End the current word definition
colon ";", semicolon, 1
    dq xt_lit, 0, xt_state, xt_write
    dq xt_lit, xt_exit, xt_comma
    dq xt_exit

section .rodata
interpreter_stub:
    dq xt_lit, 0    ; stdin
    dq xt_interpret_fd
    dq xt_bye

; interprets words from an open fd 
; (  fd -- )
colon "interpret-fd", interpret_fd
    dq xt_lit, in_fd, xt_fetch, xt_to_r
    dq xt_lit, in_fd
    dq xt_write
    .loop:
    dq xt_inbuf, xt_word
    branch0 .end_of_input

    dq xt_inbuf
    dq xt_interpret

    branch .loop

    .end_of_input:
    dq xt_from_r
    dq xt_lit, in_fd, xt_write
    dq xt_exit


;-------------------------------------------------------
; Forthress interpreter
colon "interpret", interpret
.start:
    ; ( inbuf )
    dq xt_dup, xt_find       ; find in dict

    ; ( inbuf dictaddr )
    dq xt_dup
    branch0 .number  ; if not found, try to parse as a number
    dq xt_cfa

    ; if compile
    dq xt_state, xt_fetch
    branch0 .interpreting

    ; if is immediate:
    dq xt_dup
    dq xt_lit, 1, xt_minus, xt_fetch_char, xt_not
    branch0 .interpreting

    ; compile ( inbuf xt )
    dq xt_swap, xt_drop
    dq xt_comma
    dq xt_exit

    .interpreting: ; ( inbuf xt )
    dq xt_swap, xt_drop
    dq xt_execute
    dq xt_exit

    .number: ; ( inbuf dictaddr )
    dq xt_drop, xt_dup
    dq xt_number ; ( inbuf num 0/1 )
    branch0 .no_word

   ; ( inbuf num )
    dq xt_swap, xt_drop
    dq xt_state, xt_fetch  ; if interpreting, that's it
    branch0 .exit

   ; ( num )
    dq xt_here, xt_lit, 8, xt_minus, xt_fetch

   ; ( num prev_xt )
    dq xt_dup
    dq xt_lit, xt_branch0
    dq xt_equals, xt_not
    branch0 .is_branch

    dq xt_dup
    dq xt_lit, xt_branch
    dq xt_equals, xt_not
    branch0 .is_branch

    dq xt_lit, xt_lit, xt_comma

.is_branch: ; ( num prev_xt )
    dq xt_drop
    dq xt_comma
    dq xt_exit

.no_word: ; ( inbuf num )
    dq xt_drop
    dq xt_prints
    dq xt_lit, msg_no_such_word, xt_prints
.exit:
    dq xt_exit
