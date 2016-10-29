; Между некоторыми цифрами 1, 2, 3, 4, 5, 6, 7, 8, 9,
; записанными именно в таком порядке, вставляются
; знаки +, -.  Полученное выражение вычисляется,
; например 12+34-5-6+78+9 = 122.
; Найти все числа, которые можно вычислить таким способом.

(require 'promise)

(define (decr v)	; вычитание единицы из троичного числа
    (cond
	((null? v) '())
        ((eq? (car v) 2) (cons 1 (cdr v)))
	((eq? (car v) 1) (cons 0 (cdr v)))
	(else (cons 2 (decr (cdr v))))
    )
)

(define (prev v)	; вычисление с задержкой след. троичного числа
    (if (equal? v '(0 0 0 0)) '()
	(let ((p (decr v)))
	    (cons p (make-promise (lambda () (prev p))))
	)
    )
)

(define (exprlist)	; вычисление списка троичных чисел
    (cons '(2 2 2 2 2 2 2 2) (make-promise (lambda () (prev '(2 2 2 2 2 2 2 2)))))
)

(define (evallist l)
    (if (null? l) '()
        (cons (evalexpr (concatexpr (reverse (convert (car l) (length (car l))))))
            (evallist (promise:force (cdr l))))
    )
)

(define (convert l n)
    (cond
	((eq? n 0) '(1))
	((eq? (car l) 0) (cons (+ n 1) (convert (cdr l) (- n 1)))) 
	((eq? (car l) 1) (cons (+ n 1) (cons '+ (convert (cdr l) (- n 1))))) 
	((eq? (car l) 2) (cons (+ n 1) (cons '- (convert (cdr l) (- n 1))))) 
	(else (error "bad convert"))
    )
)

(define (t)
    (sort (evallist (exprlist)))
)

(define (concatexpr ex)
    (cond
	((null? (cdr ex)) ex)
	((number? (cadr ex))
	    (concatexpr (cons (+ (* 10 (car ex)) (cadr ex)) (cddr ex))))
	(else (cons (car ex) (cons (cadr ex) (concatexpr (cddr ex)))))
    )
)

(define (evalexpr ex)
    (cond
	((null? (cdr ex)) (car ex))
	((eq? '+ (cadr ex)) (+ (car ex) (evalexpr (cddr ex))))
	((eq? '- (cadr ex)) (- (car ex) (evalexpr (cddr ex))))
    )
)

(define (split1 l)
    (cond
	((null? l) '())
	(else (cons (car l) (split2 (cdr l))))
    )
)

(define (split2 l)
    (cond 
	((null? l) '())
	(else (split1 (cdr l)))
    )
)	

(define (sort l)
    (cond 
        ((null? l) '())
	((null? (cdr l)) l)
	(else (merge (sort (split1 l)) (sort (split2 l))))
    )
)

(define (merge a b)
    (cond
	((null? a) b)
	((null? b) a)
	((eq? (car a) (car b)) (cons (car a) (merge (cdr a) (cdr b)))) 
	((< (car a) (car b)) (cons (car a) (merge (cdr a) b))) 
	((> (car a) (car b)) (cons (car b) (merge a (cdr b)))) 
	(else (error "bad merge"))
    )
)
