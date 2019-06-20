( ==================== Task 1 ==================== )
( Check is number even )
: is_even 2 % if ." no" else ." yes" then ; 


( ==================== Task 2 ==================== )
( Check is number prime , main word - check, example: 5 check => prime )

: inc 1 + ; 
: dec 1 - ; 

: copy inc swap dup rot dup rot swap ;  ( a b -- a b+1 a b+1 )
: result if ." prime" else ." no prime" then ; 
: is_prime 1 repeat copy % not until = result ; 

: check dup 1 > if is_prime else ." incorrect" then ; 


( ==================== Task 3 ==================== )
( rewrite last task - store result using allot, main word - store )

: result_2 if 1 else 0 then ; ( 1 - is prime, 0 - no prime )
: is_prime_2 1 repeat copy % not until = result_2 ; 
: check_2 dup 1 > if is_prime_2 else 2 then ; ( 2 - incorrect input number )

: get_addres 1 allot ; 
: save_addres dup rot rot ; ( val add -- add val add )

: store check_2 get_addres save_addres ! ; ( get result 0/1/2 , get addres , save )
: info dup 2 = if ." incorrect value, it must be more then 1" else if ." is prime" else ." isn't prime" then then ;  
: check_store store @ info ; 


( ==================== Task 4 ==================== )
( 2 pointers => store in heap using heap-alloc, c!, c@ and concatenation strings, test the result )
( command magic do all, test input values are saved by command task3 )

: save_pointers swap dup rot dup rot ; ( a b -- a b b a )
: count_length count swap count + ;  ( ... -- a b length )

: get_new_pointer  heap-alloc ; ( .... -- a b pointer )

: save_first_string  rot ( b p a )
	dup count 0 for ( b p a )
		dup r@ + c@ rot ( b a sym p )
		dup r@ + rot ( b a p p+i sym ) swap c! ( b a p ) swap 
	endfor ; ( b p a )

: save_p count ( b p a_length ) swap dup rot ( b p p a_l ) + ( b p p_second ) rot ; ( p p_second b )
: save_second_string save_p 
	dup count 0 for 
		dup r@ + c@ ( p p_sec b sym ) rot ( p b sym p_sec )
		dup r@ + ( p b sym p_sec p_sec+i ) rot swap c! ( p b p_sec ) swap 
	endfor ; ( p p_sec b )


: magic save_pointers count_length get_new_pointer save_first_string save_second_string ( p p_sec b ) rot prints ; 
: task3 m" abcd" m" efgh" magic ; 

( ==================== Task 5 ==================== )
( it isn't a useful word; finds the multiplication of prime numbers smaller then entered )
: mul_prime 1 swap 2 for ( 1 n 2 -- 1 ) r@ is_prime_2 if r@ * then endfor ; 

( We start with get_mul, find a prime number i, where n%i = 0 then ) 
( while (n%i == 0 () ( n/i ; after we check if n/i == 1, if it is true )
( we finished , else we find next good prime number again )


: copy_2 swap dup rot dup rot swap ; ( n i -- n i n i )

: test_finish ( finish condition ) ( res i n/i ) dup 1 = 
	if ( is n/i == 1  ) 1 1 ( finish, res i n/i 1 1 )
	else swap ( res n/i i ) copy_2 0 ( we continue )
	then ;   

: del ( res n i n%i ) 
	if ( n%i != 0 ) ( res n i ) rot swap dup rot * ( n i res*i ) rot rot 0 1 ( new_result n i 0 1 ) 
	else dup rot swap ( res i n i ) / test_finish ( finish: res i n/i 1 1 || continue: res n/i i n/i i 0 ) 
	then ;

: dev_prime ( result n i n i ) 
	repeat % ( res n i n%i ) del until ( res n i 0/1 , where 0 means: find next prime number, 1 - it is finish ) ; 


: is_dev ( res n i ) copy_2 % 
	if ( n%i != 0, we need next prime ) 0 
	else 1 ( it is good number ) 
	then ; ( res n i ) 


: get_next_prime ( res n i ) 
	repeat 
		inc dup is_prime_2 
			if ( is prime ) is_dev 
			else 0 
			then 
	until ; ( res n good_i )


( input value - n )		
: get_mul 1 swap ( 1 - start value of result -- 1 as result, n ) 
	1 ( i = 1 - start value, it will be increment ) ( result n i )
	repeat  
		get_next_prime ( res n good_i ) copy_2 dev_prime 
	until ( res i n/i ) drop * .S ;
