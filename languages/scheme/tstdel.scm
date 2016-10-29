(require 'promise)

(define (exprlist n)
    (if (eq? n 0) '()
        (cons n (make-promise (lambda () (exprlist (- n 1)))))
    )
)

(define (forcelist l)
    (if (eq? l '()) l
        (cons (car l) (forcelist (promise:force (cdr l))))
    )
)

(define (p n)
    (forcelist (exprlist n))
)
