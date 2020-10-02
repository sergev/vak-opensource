#!/usr/bin/env python3 -B -m hy

(defn fib [n]
  (if (< n 2)
    n
    (+ (fib (- n 1)) (fib (- n 2)))))

(print "(fib 10) =" (fib 10))
;(print "(fib 100) =" (fib 100))
