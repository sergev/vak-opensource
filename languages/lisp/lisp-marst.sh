#!/bin/sh
#
# Solve the 3 missionaries and 3 cannibals crossing a river puzzle.
# Use a simple LISP 1.5 interpreter written in ALGOL 60.
#
algc lisp.alg && ./lisp << END_OF_INPUT

t nil quote cond lambda define car cdr cons equal atom numberp lessp greaterp add1 sub1 add minus times div

define ((
    (crossriver (lambda ( ) (complete (cons (i) nil))))

    (complete
        (lambda (path)
            (cond ((equal (car path) (f)) (cons path nil))
                  (t (try path (fullmoveset)))
    )   )   )

    (try
        (lambda (path moveset)
            (cond ((null moveset) nil)
                ((feasible (car moveset) (car path))
                    (append (try1 path (result (car moveset) (car path)))
                            (try path (cdr moveset))))
                    (t (try path (cdr moveset)))
    )   )   )

    (try1
        (lambda (path newstate)
            (cond ((not (admissible newstate)) nil)
                  ((member newstate path) nil)
                  (t (complete (cons newstate path)))
    )   )   )

    (i (lambda ( ) (quote ((c c c) (m m m) ( ) ( ) left))))

    (f (lambda ( ) (quote ((c c c) (m m m) ( ) ( ) right))))

    (fullmoveset
        (lambda ( )
            (quote (((c c) ( )) ((c) (m)) (( ) (m m)) ((c) ( )) (( ) (m))))
    )   )

    (feasible
        (lambda (move state)
            (cond ((smaller (car state) (car move)) nil)
                  ((smaller (cadr state) (cadr move)) nil)
                  (t t)
    )   )   )

    (admissible
        (lambda (state)
            (cond ((null (cadr state)) t)
                  ((null (cadddr state)) t)
                  (t (ofequallength (car state) (cadr state)))
    )   )   )

    (result
        (lambda (move state)
            (list (inc (caddr state) (car move))
                (inc (cadddr state) (cadr move))
                (dec (car state) (car move))
                (dec (cadr state) (cadr move))
                (other (caddddr state))
    )   )   )

    (other
        (lambda (riverside)
            (cond ((equal riverside (quote left)) (quote right))
            (t (quote left))
    )   )   )

    (list
        (lambda (a b c d e)
            (cons a (cons b (cons c (cons d (cons e nil)))))
    )   )

    (smaller
        (lambda (x y)
        (cond ((null y) nil)
            ((null x) t)
            (t (smaller (cdr x) (cdr y)))
    )   )   )

    (inc
        (lambda (x y)
            (cond ((null y) x)
                  (t (inc (cons (car y) x) (cdr y)))
    )   )   )
    (dec
        (lambda (x y)
            (cond ((null y) x)
                  (t (dec (cdr x) (cdr y)))
    )   )   )

    (ofequallength
        (lambda (x y)
            (cond ((null x) (null y))
                  ((null y) nil)
                  (t (ofequallength (cdr x) (cdr y)))
     )   )   )

    (null (lambda (x) (equal x nil)))

    (append
        (lambda (x y)
            (cond ((null x) y)
                  (t (cons (car x) (append (cdr x) y)))
    )   )   )

    (not (lambda (x) (equal x nil)))

    (member
        (lambda (x y)
            (cond ((null y) nil)
                  ((equal x (car y)) t)
                  (t (member x (cdr y)))
    )   )   )

    (cadr (lambda (x) (car (cdr x))))

    (caddr (lambda (x) (car (cdr (cdr x)))))

    (cadddr (lambda (x) (car (cdr (cdr (cdr x))))))

    (caddddr (lambda (x) (car (cdr (cdr (cdr (cdr x)))))))
))

crossriver ()

END_OF_INPUT
