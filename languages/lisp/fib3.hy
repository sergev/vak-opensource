#!/usr/bin/env python3 -B -m hy

(defn fib [index]
    (if (< index 0)
        (raise (ValueError "Negative input to Fibonacci!")))
    (if (< index 2)
        (return index))
    (setv prev2 0)
    (setv prev1 1)
    (for [iteration (range 1 index)]
        (setv sum (+ prev2 prev1))
        (setv prev2 prev1)
        (setv prev1 sum))
    sum)

(assert (= (fib 0) 0))
(assert (= (fib 1) 1))
(assert (= (fib 2) 1))
(assert (= (fib 3) 2))
(print "(fib 10) =" (fib 10))
(print "(fib 100) =" (fib 100))
(print "(fib 200) =" (fib 200))
(print "(fib 300) =" (fib 300))
(print "(fib 400) =" (fib 400))
(print "(fib 500) =" (fib 500))
(print "(fib 1000) =" (fib 1000))
(print "(fib 5000) =" (fib 5000))
(print "(fib 100000) =" (fib 100000))
;(print "(fib 1000000) =" (fib 1000000))
