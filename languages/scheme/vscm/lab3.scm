; Пример 1.38 из Лаврова, Силагадзе "Автоматическая обработка данных.
; Язык Лисп и его реализация."
; Поиск пути в лабиринте методом перебора с возвратами,
; вариант с использованием рекурсии.

(define nil '())

(define (join m l) (cond
    ((null? m) l)
    (else (join (cdr m) (cons (car m) l) )) ))

(define (up l x b m r) (cond
    ((eq? x b) (cons b r))
    ((memq x r) (down l (car r) b m (cdr r)))
    (else (down (join m l) x b nil r)) ))

(define (down l x b m r) (cond
    ((null? l) (cond
        ((null? r) nil)
	(else (down m (car r) b nil (cdr r))) ))
    ((eq? x (caar l))
        (up (cdr l) (cdar l) b m (cons x r)))
    ((eq? x (cdar l))
        (up (cdr l) (caar l) b m (cons x r)))
    (else (down (cdr l) x b (cons (car l) m) r)) ))

(define (path l a b) (up l a b nil nil))

(define (lab) (path '((o . d) (p . q) (n . m)
        (g . d) (h . d) (h . e) (h . q)
        (d . c) (c . m) (c . k) (a . f)
        (b . e) (q . r) (m . l) (e . i) (i . r)
        (i . t) (r . s) (f . k) (f . j) (l . k) (l . u))
        't 'a))

(display (lab)) (newline)
