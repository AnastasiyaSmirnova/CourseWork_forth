: IMMEDIATE  last_word @ cfa 1 - dup c@ 1 or swap c! ;

: rot >r swap r> swap ;
: -rot swap >r swap  r> ;

: over >r dup r> swap ;
: 2dup over over ;

: <> = not ;
: <= 2dup < -rot =  lor ;
: > <= not ;
: >= < not ; 

: cell% 8 ;
: cells cell% * ;
: KB 1024 * ;
: MB KB KB  ;

: allot dp @ swap over + dp ! ;

: begin here ; IMMEDIATE
: again ' branch , , ; IMMEDIATE

: if ' 0branch , here 0  , ; IMMEDIATE
: else ' branch , here 0 , swap here swap !  ; IMMEDIATE
: then here swap ! ; IMMEDIATE
: endif ' then execute ; IMMEDIATE

: repeat here ; IMMEDIATE
: until  ' 0branch , , ; IMMEDIATE


: for 
     	' swap , ' >r , ' >r , 
	here  ' r> , ' r> , 
     	' 2dup , ' >r , ' >r , ' < ,  
     	' 0branch ,  
	here    0 , 
     	swap 
; IMMEDIATE

: endfor 
     	' r> , 
     	' lit , 1 , ' + ,  ' >r , 
   	' branch , ,  here swap ! ' r> , 
     ' drop , ' r> , ' drop ,  
;  IMMEDIATE

: do  ' swap , ' >r , ' >r ,  here ; IMMEDIATE

: loop 
        ' r> , 
        ' lit , 1 , 
        ' + , 
        ' dup ,     
        ' r@ , 
        ' < , 
        ' not , 
        '  swap , 
        ' >r , 
        ' 0branch , ,
        ' r> , 
        ' drop ,
        ' r> , 
        ' drop ,
 ;  IMMEDIATE


: sys-read-no 0 ;
: sys-write-no 1 ;

: sys-read  >r >r >r sys-read-no r> r> r> 0 0 0  syscall drop ;
: sys-write >r >r >r sys-write-no r> r> r> 0 0 0  syscall drop ;

: readc@ in_fd @ swap 1 sys-read ;
: readc inbuf readc@ drop inbuf c@ ;

: ( repeat readc 41 - not until ; IMMEDIATE

( This is comments. It's necessary for part 1. )


: 2drop drop drop ;
: 2over >r >r dup r> swap r> swap ;
: case 0 ; IMMEDIATE
: of ' over , ' = , ' if execute ' drop , ; IMMEDIATE
: endof ' else execute ; IMMEDIATE
: endcase ' drop , dup if repeat ' then execute dup not until drop then  ; IMMEDIATE


( num from to -- 1/0)
: in-range rot swap over >= -rot <= land ;

( 1 if we are compiling )
: compiling state @ ;

: compnumber compiling if ' lit , , then ;

( -- input character's code )
: .' readc compnumber ; IMMEDIATE

: readce readc dup .' \ = if
         readc dup .' n = if
           drop drop 10
         else
           drop drop 0
         then
then
;

: cr 10 emit ;
: QUOTE 34 ;

: _"
  compiling if
    ' branch , here 0 , here
    repeat
readc dup 34 =
if
  drop
  0 c, ( null terminator )
  ( label_to_link string_start )
  swap
  ( string_start label_to_link )
  here swap !
  ( string_start )
  ' lit , , 1
else c, 0
then
    until
  else
    repeat
readce dup 34 = if drop 1 else emit 0 then
    until 
  then ; IMMEDIATE

: " compiling if
      ' branch , here 0 , here
      repeat
readce dup 34 =
if
  drop
  0 c, ( null terminator )
  ( label_to_link string_start )
  swap
  ( string_start label_to_link )
  here swap !
  ( string_start )
  ' lit , , 1
else c, 0
then
      until
    else
      repeat
readce dup 34 = if drop 1 else emit 0 then
      until 
    then ; IMMEDIATE

: ." ' " execute compiling if ' prints , then ; IMMEDIATE

: include
    inbuf word drop
    inbuf file-open-append >r
    ( place descriptor on top of data stack and interpret it )
    r@ interpret-fd
    r@ file-close
    r> drop ;

( Next commands are necessary for part 1 - work with strings; count hash )
include string.frt
include hash.frt 

." Forthress - Forth dialect; student Smirnova Anastasia > (c) Igor Zhirkov 2017-2018 " cr

