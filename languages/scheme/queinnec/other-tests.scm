;;; other-tests.scm,v 1.1.1.1 1995/02/27 15:15:03 baird Exp
;;; Some tests and examples for the compiler. These tests use macros
;;; such as let, letrec etc.

;;; letrec
(letrec ((a (+ a 1)))
  0 )
   ***

;; OScheme is really sensible to functional lambdas:
(define (identity x) x)
---
(let ((x 1)) (let ((y 0) (z 2)) (identity z)))
2

;;; Named let
(let loop ((n 4))
  (if (= n 0) '() (cons n (loop (- n 1)))) )
   (4 3 2 1)

;; Testing allocation coalescing

(let () 1)
1
(let ((a 2)) (let ((b 3)) (cons a b)))
(2 . 3)
(let ((a 2)) (let ((b (+ 1 a))) (cons a b)))
(2 . 3)
;(let ((a 1)(b 2)) (let ((b 3)(c 4)) (cons a (cons b (cons c '())))))
(let ((a 1)(b 2)) (let ((b 3)(c 4)) (list a b c)))
(1 3 4)
;(let ((a 1)(b 2)) (let ((b (+ a b))(c 4)) (cons a (cons b (cons c '())))))
(let ((a 1)(b 2)) (let ((b (+ a b))(c 4)) (list a b c)))
(1 3 4)
(let ((a 1)) (cons a 
                   (cons (let ((a (+ a a))) a)
                         '() ) ))
(1 2)
;; allocation can move through sequence
(let ((a 1)) (begin (set! a (+ a a))
                    (let ((b (+ a a))) b) ))
4
;; allocation can move through assignment
(let ((x 0))
  (let ((a 1)) (set! x (let ((b (+ a a))) b)))
  x )
2

;;; Testing mutability
(let ((x 1))
  (set! x 2)
  x )
2

;;; Testing 
(let () 
  (define (fact n)
    (if (= n 1) 1
        (* n (fact (- n 1))) ) )
  (fact 5) )
   120
(let ()
  (define (fib n)
    (if (<= n 2) 1
        (+ (fib (- n 1)) (fib (- n 2))) ) )
  (fib 6) )
   8
(let ()
  (define (accumulate binary-op initial items)
    (if (null? items)
        initial
        (letrec ((loop (lambda (value items)
                         (if (null? items)
                             value
                             (loop (binary-op value (car items))
                                   (cdr items) ) ) )))
          (loop (car items) (cdr items)) ) ) )
  (accumulate + 0 '(1 2 3 4)) )
   10
(let ()
  (define (meta-fact f)
    (lambda (n)
      (if (= n 1) 
          1
          (* n (f (- n 1))) ) ) )
  (define (fix f)
    (let ((d (lambda (x) (f (lambda (y) ((x x) y))))))
      (d d) ) )
  ((fix meta-fact) 5) )
   120


;;; A huge call/cc test (from scm4a2, 1991 Aubrey Jaffer)
;;; This tests full conformance of call-with-current-continuation.  It
;;; is a separate test because some schemes do not support call/cc
;;; other than escape procedures.  I am indebted to
;;; raja@copper.ucs.indiana.edu (Raja Sooriamurthi) for fixing this
;;; code.  The function leaf-eq? compares the leaves of 2 arbitrary
;;; trees constructed of conses.  
(let ()
  (define (not b)
    (if b #f #t) )
  (define (for-each fun list)
    (if (pair? list)
        (begin (fun (car list))
               (for-each fun (cdr list)) )
        '() ) )
  (define (next-leaf-generator obj eot)
    (letrec ((return #f)
             (cont (lambda (x)
                     (recur obj)
                     (set! cont (lambda (x) (return eot)))
                     (cont #f) ))
             (recur (lambda (obj)
                      (if (pair? obj)
                          (for-each recur obj)
                          (call/cc
                           (lambda (c)
                             (set! cont c)
                             (return obj))) ) )) )
      (lambda () (call/cc
                  (lambda (ret) (set! return ret) (cont #f)))) ))
  (define (leaf-eq? x y)
    (let* ((eot (list 'eot))
           (xf (next-leaf-generator x eot))
           (yf (next-leaf-generator y eot)) )
      (let loop ((x (xf))
                 (y (yf)) )
        ;(basicwrite (list 'compare x y)) ; DEBUG
        (cond ((not (eq? x y)) #f)
              ((eq? eot x) #t)
              (else (loop (xf) (yf))) ) ) ) )
  (and (leaf-eq? '(a (b (c))) '((a) b c))
       (not (leaf-eq? '(a (b (c))) '((a) b c d))) ) )
   #t
