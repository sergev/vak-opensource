#!/usr/local/bin/guile \
--no-auto-compile -s
!#
(define (fibo n)
  (letrec ((fibof
            (lambda (n a b)
             (if (<= n 2)
                 b
                 (fibof (- n 1) b (+ a b))))))

    (fibof n 1 1)))

(display "(fibo 100000) = ")
(display (number->string (fibo 100000)))
(newline)
