;==============================================
;Coursework: Forthress, a Forth dialect
;
;Student - Smirnova Anastasia, P3211 
;ITMO University, 2019
; 
;macro: make a code mode active (and nice)
;==============================================

%define _lw 0
%macro native 3
    section .data
    wh_ %+ %2 : dq _lw
    db 0  ; to mark the name start
    db %1, 0
    db %3
    
    %define _lw wh_%+ %2 
    xt_ %+ %2 :  dq i_ %+ %2 
    section .text
    i_ %+ %2:
%endmacro

; it's enought for most of words to have only 2 params
%macro native 2
native %1, %2, 0
%endmacro

;-----------------------------------------------------
%macro colon 3
section .data 
    wh_ %+ %2 : dq _lw
    %define _lw wh_ %+ %2 
    db 0  ; to mark the name start
    str_ %+ %2:
    db %1, 0
    db %3
    
    xt_ %+ %2 : dq i_docol
%endmacro

%macro colon 2
colon %1, %2, 0
%endmacro

;-----------------------------------------------------
; lets create our own pop, push :)
%macro rpush 1
    sub rstack, 8
    mov qword [rstack], %1
%endmacro

%macro rpop 1
    mov %1, qword [rstack]
    add rstack, 8
%endmacro


%macro const 2
%defstr %%__cnst_str %1
native %%__cnst_str, %1 
    push qword %2
    jmp next
%endmacro


%macro branch 1 
dq xt_branch
dq %1
%endmacro

%macro branch0 1 
dq xt_branch0
dq %1 
%endmacro

