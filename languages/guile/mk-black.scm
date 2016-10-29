#!/opt/local/bin/guile -q -s
!#
(use-modules (ice-9 rdelim))

; Чтение строчек "Received:" из указанного файла.
(define (read-lines-received port)

  ; Возвращает список строк файла, с восстановлением длинных
  ; заголовков. Заведомо ненужные строки заменяются пустыми.
  (define (read-lines list-of-lines)
    (define line (read-line port))
    (cond
      ((eof-object? line)
	(reverse list-of-lines))
      ((and (char-whitespace? (string-ref line 0))
	    (not (null? list-of-lines)))
	(read-lines (cons (string-append (car list-of-lines) line)
			 (cdr list-of-lines))))
      ((and (>= (string-length line) 9)
	    (string-ci=? (substring line 0 9) "Received:"))
	(read-lines (cons line list-of-lines)))
      (else
	(read-lines (cons "" list-of-lines)))))

  ; Удаление ненужных строк, оставляем только "Received:"
  (define (filter-received in out)
    (cond
      ((null? in)
        (reverse out))
      ((and (>= (string-length (car in)) 9)
	    (string-ci=? (substring (car in) 0 9) "Received:"))
	(filter-received (cdr in) (cons (car in) out)))
      (else
	(filter-received (cdr in) out))))

  (filter-received (read-lines '()) '()))

(define data (call-with-input-file "pmn-pmn" read-lines-received))

(define (print-line line)
  (display line)
  (newline))
(for-each print-line data)
