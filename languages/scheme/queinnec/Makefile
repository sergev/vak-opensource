# Makefile
# Makefile,v 1.3 1995/03/07 02:20:48 baird Exp
# Makefile for tests.
# These tests have been written by C. Queinnec, see the README file.

OSCHEME=../kernel/oskernel -path ../kernel/lib -path ../contrib 

test.int :: 
	-rm /tmp/oscheme.results 
	echo "(load \"tester.scm\") 			\
(define (eval e) 					\
  (codeblock:execute (sys:compile (sys:expand e))))	\
(define t #t)						\
(define f #f)						\
(define nil '())					\
(define a 'wait)					\
(define foo 'wait)					\
(define fact 'wait)					\
(define primes 'wait)					\
(suite-test \"scheme.tst\"				\
  \"?? \" \"== \" #t					\
  (lambda (read check err)				\
    (lambda () 						\
      (error:case (check (eval (read)))                 \
         (#t (lambda error (err error))))))             \
  equal? )" | ${OSCHEME} 2>&1 | tee /tmp/oscheme.results 
	grep -i DONE /tmp/oscheme.results

test.int :: 
	-rm /tmp/oscheme.results 
	echo "(load \"tester.scm\") 			\
(define (eval e) 					\
  (codeblock:execute (sys:compile (sys:expand e))))	\
(define t #t)						\
(define f #f)						\
(define nil '())					\
(define a 'wait)					\
(define foo 'wait)					\
(define fact 'wait)					\
(define primes 'wait)					\
(suite-test \"other-tests.scm\"				\
  \"?? \" \"== \" #t					\
  (lambda (read check err)				\
    (lambda () 						\
      (error:case (check (eval (read)))                 \
         (#t (lambda error (err error))))))             \
  equal? )" | ${OSCHEME} 2>&1 | tee /tmp/oscheme.results 
	grep -i DONE /tmp/oscheme.results

