(define (square n) (* n n))

(define (smallest-divisor n)
  (find-divisor n 2))

(define (find-divisor n test-divisor)
  (cond ((> (square test-divisor) n) n)
        ((divides? test-divisor n) test-divisor)
        (else (find-divisor n (+ test-divisor 1)))))

(define (divides? a b)
  (= (remainder b a) 0))

(define (prime? n)
  (= n (smallest-divisor n)))

(define (expmod b e m)
  (cond ((= e 0) 1)
        ((even? e)
         (remainder (square (expmod b (/ e 2) m))
                    m))
        (else
         (remainder (* b (expmod b (- e 1) m))
                    m))))        

(define (fermat-test n)
  (define a (+ 2 (random (- n 2))))
  (= (expmod a n n) a))

(define (fast-prime? n times)
  (cond ((= times 0) t)
        ((fermat-test n)
         (fast-prime? n (- times 1)))
        (else nil)))

(define (t n)
  (display n)
  (if (prime? n)
    (display '-prime)
    (display '-compound))
  (newline))

(t 2)
(t 20)
(t 23)
(t 65)
(t 67)
