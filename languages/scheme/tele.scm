(define N 128)

; Создание списка чисел (N N-1 ... 2 1)
(define L '())
(define (make-L k)
    (set! L (cons K L))
    (if (< k N) (make-L (+ k 1)))
)
(make-L 1)

(define V (make-vector (+ N 1)))
(vector-set! V 0 1)

; Вычисление вектора от индекса K до N.
(define (compute k)
    (vector-set! V k
	(apply max
	    (map (lambda (i) (lcm (vector-ref V (- k i)) i))
		(list-tail L (- N k))
    )   )   )
    (if (< k N) (compute (+ k 1)))
)
(compute 1)

(define log10 (log 10))

(define (print-vector k)
    (display k)
    (display " ")
    (display (/ (log (vector-ref V k)) log10))
    (newline)
    (if (< k N) (print-vector (+ k 1)))
)

(print-vector 0) (newline)
