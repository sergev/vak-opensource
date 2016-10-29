; Пример 1.38 из Лаврова, Силагадзе "Автоматическая обработка данных.
; Язык Лисп и его реализация."
; Поиск пути в лабиринте, вариант с использованием рекурсии.

(define nil '())

(define (p x s) (cond
    ((null? s) (cons x nil))
    ((eq? x (caar s)) (cons x (p (cdar s) (cdr s))))
    (else (p x (cdr s))) ))

(define (step l a i s m j) (cond
    ((memq a j) (p a s))
    ((null? l) (cond
        ((null? j) nil)
        (else (step m a j s nil nil)) ))
    ((memq (cdar l) i) (cond
        ((memq (caar l) i)
            (step (cdr l) a i s m j))
        ((memq (caar l) j)
            (step (cdr l) a i s m j))
        (else (step (cdr l) a i
            (cons (car l) s) m
            (cons (caar l) j) )) ))
    ((memq (caar l) i) (cond 
        ((memq (cdar l) j)
            (step (cdr l) a i s m j))
        (else (step (cdr l) a i (cons (cons (cdar l) (caar l)) s)
            m (cons (cdar l) j) )) ))
    (else (step (cdr l) a i s (cons (car l) m) j)) ))

(define (path l a b) (cond
    ((eq? a b) (cons a nil))
    (else (step l a (cons b nil) nil nil nil)) ))

(define (lab) (path '((o . d) (p . q) (n . m)
        (g . d) (h . d) (h . e) (h . q)
        (d . c) (c . m) (c . k) (a . f)
        (b . e) (q . r) (m . l) (e . i) (i . r)
        (i . t) (r . s) (f . k) (f . j) (l . k) (l . u))
        't 'a))

(display (lab)) (newline)
