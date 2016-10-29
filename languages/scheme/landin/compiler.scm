(require 'eval)

(define atom symbol?)
(define add +)
(define (eq a b) (cond
    ((and (eq? a 'nil) (eq? b '())) #t)
    ((and (eq? a '()) (eq? b 'nil)) #t)
    (else (eq? a b))))

(define complist (call-with-input-file "compiler.il" read))

(define compiler (eval complist))

(define (c port)
    (let ((e (read port)))
	(if (eof-object? e) '()
	    (begin
;	        (display e) (display " --> ")
        	(display (compiler e))
	        (newline)))))

;(call-with-input-file "compiler.il" c)
(c (current-input-port))
(quit)
