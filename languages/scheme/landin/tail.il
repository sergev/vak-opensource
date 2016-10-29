(letrec 
    (
        (tail (lambda (x) 
            (if (atom x) x 
            (if (eq (cdr x) (quote nil)) (car x) 
            (tail (cdr x))))
        ))
    )
    tail
)
