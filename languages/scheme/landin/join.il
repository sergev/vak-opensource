(letrec (
        (join (lambda (x y) 
            (if (eq x (quote nil))
                y
            (if (eq y (quote nil))
	        x
	    (cons (car x) (join (cdr x) y))))))
    )
    join
)
