#!/usr/bin/env python3 -B -m hy

(defn fib [index]
    (if (< index 0)
        (raise (ValueError "Negative input to Fibonacci!")))
    (if (< index 2)
        (return 1))
    (defn fibrec [index iteration prev2 prev1]
        (setv sum (+ prev2 prev1))
        (setv iter1 (+ iteration 1))
        (if (= iter1 index)
            (return sum))
        (fibrec index iter1 prev1 sum))
    (fibrec index 1 0 1))

(print "(fib 10) =" (fib 10))
(print "(fib 100) =" (fib 100))
(print "(fib 200) =" (fib 200))
(print "(fib 300) =" (fib 300))
(print "(fib 400) =" (fib 400))
(print "(fib 500) =" (fib 500))
(print "(fib 600) =" (fib 600))
(print "(fib 700) =" (fib 700))
(print "(fib 800) =" (fib 800))
(print "(fib 900) =" (fib 900))
;(print "(fib 1000) =" (fib 1000))
