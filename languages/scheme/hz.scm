(define (print v)			; print v
  (display v)
  (newline))

(define the-empty-stream '())		; the-empty-stream

(define empty-stream? null?)		; empty-stream? stream

(define head car)			; head stream

(define (tail stream)			; tail stream
  (force (cdr stream)))

(define (map-stream proc stream)	; map-stream proc stream
  (if (empty-stream? stream)
      the-empty-stream
      (cons (proc (head stream))
	    (delay (map-stream proc (tail stream))))))

(define (for-each-stream proc stream)	; for-each-stream proc stream
  (if (empty-stream? stream)
      'done
      (begin (proc (head stream))
	     (for-each-stream proc (tail stream)))))

(define (enumerate-interval low high)	; enumerate-interval low high
  (if (> low high)
      the-empty-stream
      (cons low (delay (enumerate-interval (+ low 1) high)))))

(define (print-stream s)		; print-stream s
  (for-each-stream print s))

(define (singleton s)			; singleton s
  (cons s (delay the-empty-stream)))

(define (filter pred stream)		; filter pred stream
  (cond ((empty-stream? stream) the-empty-stream)
	((pred (head stream))
	 (cons (head stream)
	       (delay (filter pred (tail stream)))))
	(else (filter pred (tail stream)))))

(define (select-range min max stream)	; select-range min max stream
  (cond ((empty-stream? stream) the-empty-stream)
	((< max (head stream)) the-empty-stream)
	((<= min (head stream))
	 (cons (head stream)
	       (delay (select-range min max (tail stream)))))
	(else (select-range min max (tail stream)))))

(define (divisible? x y) (= (remainder x y) 0))

(define (scale-stream c stream)
  (map-stream (lambda (x) (* x c)) stream))

(define (add-streams a b)
  (cond ((empty-stream? a) b)
	((empty-stream? b) a)
	(else
	 (cons (+ (head a) (head b))
	       (delay (add-streams (tail a) (tail b)))))))

(define ones (cons 1 (delay ones)))

(define integers (cons 1 (delay (add-streams ones integers))))

(define (select-divisible x stream)
  (filter (lambda (v) (divisible? v x)) stream))

;;(print-stream (enumerate-interval 1 6))
;;(print-stream (scale-stream 3 (enumerate-interval 1 6)))

(print-stream
 (select-range 10000000 30000000
	       (select-divisible 64000
				 (scale-stream (* 76800 8)
					       integers))))

(quit)