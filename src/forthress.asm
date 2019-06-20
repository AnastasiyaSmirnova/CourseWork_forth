;==============================================
;Coursework: Forthress, a Forth dialect
;
;Student - Smirnova Anastasia, P3211 
;ITMO University, 2019
; 
;main file (start and next - programm)
;==============================================

global _start
%include "macro.inc" 
%include "util.inc"

;----------------------------------------------
; define registers (there are 2 stacks here)
%define pc r15
%define w r14
%define rstack r13

;----------------------------------------------
section .text          

%include "base_dictionary.inc"   ; functional words as plus, mul, etc (part 1) 
%include "dictionary.inc"   ;  optinal part (working process)

;----------------------------------------------
section .bss

resq 1023             
rstack_start: resq 1   

input_buf: resb 1024   
user_dict:  resq 65536 
user_mem: resq 65536   

state: resq 1          ; changes to 1 if compiling, 0 by default

;----------------------------------------------
section .data 

last_word: dq _lw      ; pointer to the last word in the dictionary 
here: dq user_dict     
dp: dq user_mem        

;----------------------------------------------
section .rodata

msg_no_such_word: db ": no such word", 10, 0 ; error message

;----------------------------------------------
section .text
; next code is used every time, after any other programm 
next:                  
    mov w, pc
    add pc, 8
    mov w, [w]
    jmp [w]

_start: 
    jmp i_init

