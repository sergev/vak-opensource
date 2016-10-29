(letrec (
        (calc (lambda (x y)
            (let (
                    (x2 (mul x x))
                    (y2 (mul y y))
                )
                (add x2 y2))))
    )
    calc
)
