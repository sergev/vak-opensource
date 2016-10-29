; Пример 1.38 из Лаврова, Силагадзе "Автоматическая обработка данных.
; Язык Лисп и его реализация."
; Поиск пути в лабиринте, вариант с использованием PROG.

(defun memb (x y) (prog nil
  A (cond ((null y) (return nil))
          ((eq x (car y)) (return t)))
    (setq y (cdr y))
    (go A) ))

; Настоящий программист сможет написать фортрановскую
; программу на любом языке!

(defun path (l a b)
    (prog (i j s m x y z)
        (setq i (cons b nil))
        (cond ((eq a b) (return i)))
      A (cond ((null l) (go d)))
        (setq z (car l))
        (setq l (cdr l))
        (setq x (car z))
        (setq y (cdr z))
        (cond ((memb y i) (go C))
              ((memb x i) (go B)))
        (setq m (cons z m))
        (go A)
      B (setq z (cons y x))
        (setq y x)
        (setq x (car z))
      C (cond ((memb x j) (go A)))
        (setq s (cons z s))
        (cond ((eq x a) (go E)))
        (setq j (cons x j))
        (go A)
      D (cond ((null j) (return nil)))
        (setq l m)
        (setq i j)
        (setq m nil)
        (setq j nil)
        (go A)
      E (setq z nil)
      F (cond ((null s) (return (cons x z))))
        (setq y (car s))
        (setq s (cdr s))
        (cond ((eq x (car y)) (go G)))
        (go F)
      G (setq z (cons x z))
        (setq x (cdr y))
        (go F) ))

(defun lab () (path '((o . d) (p . q) (n . m)
        (g . d) (h . d) (h . e) (h . q)
        (d . c) (c . m) (c . k) (a . f)
        (b . e) (q . r) (m . l) (e . i) (i . r)
        (i . t) (r . s) (f . k) (f . j) (l . k) (l . u))
        't 'a))

(princ "Enter (lab) to start labyrinth search.")
(terpri)
