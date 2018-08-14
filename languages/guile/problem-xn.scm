#! /usr/bin/env guile -q -s
!#

(define (solve n x)
  (define d (remainder x 10))
  (if (equal? x 1)
    1
    (+ d (* 10 (solve n (+ (quotient x 10) (* d n)))))))

(display "For 2: ") (display (solve 2 2)) (newline)
(display "For 3: ") (display (solve 3 3)) (newline)
(display "For 4: ") (display (solve 4 4)) (newline)
(display "For 5: ") (display (solve 5 5)) (newline)
(display "For 6: ") (display (solve 6 6)) (newline)
(display "For 7: ") (display (solve 7 7)) (newline)
(display "For 8: ") (display (solve 8 8)) (newline)
(display "For 9: ") (display (solve 9 9)) (newline)
