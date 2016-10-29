; Пример 1.38 из Лаврова, Силагадзе "Автоматическая обработка данных.
; Язык Лисп и его реализация."
; Поиск пути в лабиринте, вариант с использованием рекурсии.

(defun memb (x y) (cond
    ((null y) nil)
    ((eq x (car y)) t)
    (t (memb x (cdr y))) ))

(defun p (x s) (cond
    ((null s) (cons x nil))
    ((eq x (caar s)) (cons x (p (cdar s) (cdr s))))
    (t (p x (cdr s))) ))

(defun step (l a i s m j) (cond
    ((memb a j) (p a s))
    ((null l) (cond
        ((null j) nil)
        (t (step m a j s nil nil)) ))
    ((memb (cdar l) i) (cond
        ((memb (caar l) i)
            (step (cdr l) a i s m j))
        ((memb (caar l) j)
            (step (cdr l) a i s m j))
        (t (step (cdr l) a i
            (cons (car l) s) m
            (cons (caar l) j) )) ))
    ((memb (caar l) i) (cond 
        ((memb (cdar l) j)
            (step (cdr l) a i s m j))
        (t (step (cdr l) a i (cons (cons (cdar l) (caar l)) s)
            m (cons (cdar l) j) )) ))
    (t (step (cdr l) a i s (cons (car l) m) j)) ))

(defun path (l a b) (cond
    ((eq a b) (cons a nil))
    (t (step l a (cons b nil) nil nil nil)) ))

(defun lab () (path '((o . d) (p . q) (n . m)
        (g . d) (h . d) (h . e) (h . q)
        (d . c) (c . m) (c . k) (a . f)
        (b . e) (q . r) (m . l) (e . i) (i . r)
        (i . t) (r . s) (f . k) (f . j) (l . k) (l . u))
        't 'a))

(princ "Enter (lab) to start labyrinth search.")
(terpri)
