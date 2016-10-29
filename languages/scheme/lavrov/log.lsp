; Пример из приложения к Лаврову, Силагадзе "Автоматическая обработка данных.
; Язык Лисп и его реализация."
; Проверка общезначности формулы исчисления высказываний.

; Формула - это либо логическая константа (T или NIL), либо
; переменная (любой атом, отличный от T и NIL), либо список,
; первый элемент которого - знак логической операции (NOT, AND,
; OR, IMP или EQU), а следующие один (для NOT) или два
; элемента - формулы, являющиеся операндами данной операции.
; Например:
;     (equ (not (imp x y)) (and x (not y)))
; По определению формула общезначима, если она истинна при
; любых значениях переменных.

; Пытаемся построить контрпример - набор значений переменных,
; при котором данная формула принимает значение "ложь".

(defun prove (l r s) (cond
    (l (prove1 (car l) (cdr l) r s))
    (r (prove2 (car r) (cdr r) s))
    (s (prove (caar s) (cdar s) (cdr s)))
    (t t) ))

(defun prove1 (f l r s) (cond
    ((atom f) (subt f l r s))
    (t (prove3 (car f) (cdr f) l r s)) ))

(defun prove2 (f r s) (cond
    ((atom f) (subv nil f r nil nil s))
    (t (prove4 (car f) (cdr f) r s)) ))

(defun prove3 (op a l r s) (cond
    ((eq op 'not)
        (prove l (cons (car a) r) s))
    (t (prove5 op (car a) (cadr a) l r s)) ))

(defun prove4 (op a r s) (cond
    ((eq op 'not) (prove a r s))
    (t (prove6 op (car a) (cadr a) r s)) ))

(defun prove5 (op f g l r s) (cond
    ((eq op 'and) (prove1 f (cons g l) r s))
    ((eq op 'or) (prove1 f l r (cons (cons (cons (g l) r) s))))
    ((eq op 'imp) (prove1 g l r (cons (cons l (cons f r)) s)))
    (t (prove1 f (cons g l) r (cons (cons l (cons f (cons g r)))))) ))

(defun prove6 (op f g r s) (cond
    ((eq op 'or) (prove2 f (cons g r) s))
    ((eq op 'and) (prove2 f r (cons (cons nil (cons g r)) s)))
    ((eq op 'imp) (prove1 f nil (cons g r) s))
    (t (prove1 f nil (cons g r) (cons (cons (list g) (cons g r)) s))) ))

(defun subt (x l r s) (cond
    ((null l) (subv t x r nil nil s))
    (t (subt x (cdr l) (cons (list 'not (car l)) r) s)) ))

(defun subv (v x r lx lf s) (cond
    (r (subv1 v x (sub v x (car r)) (cdr r) lx lf s))
    ((null lf) nil)
    (lx (subv nil (car lx) lf (cdr lx) nil s))
    (t (prove2 (car lf) (cdr lf) s)) ))

(defun subv1 (v x f r lx lf s) (cond
    ((null f) (subv v x r lx lf s))
    ((eq f t) (cond (s (prove (caar s) (cdar s) (cdr s)))
                    (t t) ))
    ((atom f) (subv v x r (cons f lx) lf s))
    (t (subv v x r lx (cons f lf) s)) ))

(defun sub (v x f) (cond
    ((eq x f) v)
    ((atom f) f)
    (t (suba v x (car f) (sub v x (cadr f)) (cddr f))) ))

(defun suba (v x op f a) (cond
    ((eq op 'not) (neg f))
    (t (subb op f (sub v x (car a)))) ))

(defun neg (f) (cond
    ((eq f t) nil)
    ((eq f nil) t)
    (t (list 'not f)) ))

(defun subb (op f g) (cond
    ((eq f t) (cond
        ((or (eq g t) (eq op 'or)) t)
        ((null g) nil)
        (t g) ))
    ((null f) (cond
        ((eq op 'or) g)
        ((eq op 'and) nil)
        ((eq op 'imp) t)
        (t (neg g)) ))
    ((eq g t) (cond
        ((eq op 'or) t)
        ((eq op 'imp) t)
        (t f) ))
    ((null g) (cond
        ((eq op 'or) f)
        ((eq op 'and) nil)
        (t (list 'not f)) ))
    (t (list op f g)) ))

(defun check (f)
    (prove2 f nil nil))

(defun run ()
    (check ' (equ (not (imp x y)) (and x (not y))) ))
