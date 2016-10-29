(sexpr memb (lambda (x y) (prog ()
    L (cond ((null y) (return nil))
            ((eq x (car y)) (return t)))
    (setq y (cdr y)) (go L))))
