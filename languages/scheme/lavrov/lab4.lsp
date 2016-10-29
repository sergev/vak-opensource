; Пример 1.38 из Лаврова, Силагадзе "Автоматическая обработка данных.
; Язык Лисп и его реализация."
; Поиск пути в лабиринте методом перебора с возвратами,
; вариант с использованием PROG.

(defun memb (x y) (prog nil
  A (cond ((null y) (return nil))
          ((eq x (car y)) (return t)))
    (setq y (cdr y))
    (go A) ))

; Настоящий программист сможет написать фортрановскую
; программу на любом языке!

(defun path (l a b) (prog (m r x)
   UP (cond ((eq a b) (return (cons b r)))
            ((memb a r) (go D1)))
 JOIN (cond ((null m) (go DOWN)))
      (setq l (cons (car m) l))
      (setq m (cdr m))
      (go JOIN)
   D1 (setq a (car r))
      (setq r (cdr r))
 DOWN (cond ((null l) (go D2)))
      (setq x (car l))
      (setq l (cdr l))
      (cond ((eq (car x) a) (go U1))
            ((eq (cdr x) a) (go U2)))
      (setq m (cons x m))
      (go DOWN)
   U1 (setq x (cdr x))
      (go U3)
   U2 (setq x (car x))
   U3 (setq r (cons a r))
      (setq a x)
      (go UP)
   D2 (cond ((null r) (return nil)))
      (setq l m)
      (setq m nil)
      (go D1) ))

(defun lab () (path '((o . d) (p . q) (n . m)
        (g . d) (h . d) (h . e) (h . q)
        (d . c) (c . m) (c . k) (a . f)
        (b . e) (q . r) (m . l) (e . i) (i . r)
        (i . t) (r . s) (f . k) (f . j) (l . k) (l . u))
        't 'a))

(princ "Enter (lab) to start labyrinth search.")
(terpri)
