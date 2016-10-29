#!/opt/local/bin/guile \
-e main -s
!#
(define (main args)
  (map (lambda (arg) (display arg) (display " "))
    (cdr args))
  (newline))
