; Автоморфными называются числа, которые содержатся
; в последних разрядах их квадрата, например:
;     5^2 == 25
;    25^2 == 625
; Программа вычисляет автоморфные числа.

(define (autolist max)		; строим список автоморфных от 0 до max
    (cond 
	((< max 0) '())		; если меньше 0, то пустой список
	((automorf? max) 	; если автоморфно, добавляем в список
	    (cons max (autolist (- max 1))))
	(else 			; иначе рекурсия с уменьшением max
	    (autolist (- max 1))) 
    )
)

(define (automorf? n)		; проверяем число на автоморфность
    (listhead? 
	(reverse (string->list (number->string n)))
	(reverse (string->list (number->string (* n n))))
    )
)

(define (listhead? a b)		; проверяем, что a есть подсписок b
    (cond
        ((null? a) #t)
	((null? b) #f)
	((equal? (car a) (car b)) 
	    (listhead? (cdr a) (cdr b)))
	(else #f)
    )
)

(define (t1) (reverse (autolist 100)))
