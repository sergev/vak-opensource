; Вычисление таблицы простых чисел.
; Алгоритм - решето Эратосфена.

(define (primelist max)		; вычисляет список простых чисел 1..max
    (do (
    	    (vec (make-vector (+ max 1) #t))
	    (i 2 (+ i 1))
	)
	((>= i (/ max 2)) 
	    (log->num 0 (vector->list vec)))
	(do ((k (+ i i) (+ k i)))
	    ((> k max)) 
	    (vector-set! vec k #f)
	)
    )
)

(define (log->num n l)		; преобразование логических констант в числа
    (cond 
	((null? l) '())
	((car l) (cons n (log->num (+ n 1) (cdr l))))
	(else (log->num (+ n 1) (cdr l)))
    )
)

(define (t2) (primelist 300))
