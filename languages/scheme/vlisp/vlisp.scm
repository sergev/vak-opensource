; Интерпретатор языка ЛИСП

(define nil '())
(define t 't)

; Счетчик для GENSYM
(define l-gensym-cnt 0)

; Бесконечный список (nil nil nil...)
(define nillist (cons nil nil))
(set-cdr! nillist nillist)

; Переключатель для реализации функций GO и RETURN 
; оператора PROG.  Функция GO устанавливает его в 'go,
; функция RETURN - в 'ret.
(define l-prog-switch 'step)

; Таблица атомов (имя - список свойств)
(define l-atom-table nil)

; Вычисление значения аргумента
(define (l-eval arg cx)
    (cond
        ((symbol? arg) (l-evatom arg cx))
        ((pair? arg) (l-apply (car arg) (cdr arg) cx))
        (else (l-error "Cannot evaluate: " arg))
)   )

; Вычисление значения атома
(define (l-evatom a1 cx)
    (let ((ap (l-proplist a1)))
	(cond
	    ((and ap (l-assoc 'apval ap)) => cdr)
	    ((l-assoc a1 cx) => cdr)
	    (else (l-error "Unbound variable:" a1))
)   )   )

; Поиск заданного свойства в списке
(define (l-assoc i u)
    (cond
        ((null? u) #f)
	((eq? (caar u) i) (car u))
	(else (l-assoc i (cdr u)))
)   )

; Выдача списка свойств атома
(define (l-proplist a)
    (let ((p (l-assoc a l-atom-table)))
	(cond
            ((not p) #f)
            ((pair? p) (cdr p))
            (else (l-error "Bad atom table pair"))
)   )   )

; Применение функции к списку аргументов
(define (l-apply a1 a2 cx)
    (cond
        ((symbol? a1)
	    (l-evfun a1 a2 cx))
	((not (pair? a1))
	    (l-error "Bad function"))
	((eq? (car a1) 'lambda)
	    (l-evlam (cdr a1) (l-evlis a2 cx) cx))
	((eq? (car a1) 'funarg)
	    (l-evfarg (cdr a1) a2 cx))
)   )

; Вычисление элементов списка
(define (l-evlis a1 cx)
    (l-list a1 cx) 
)

; Построение списка значений
(define (l-list a1 cx)
    (if (null? a1) nil
	(cons (l-eval (car a1) cx) (l-list (cdr a1) cx))
)   )

; Вычисление функции
(define (l-evfun a1 a2 cx)
    (let ((ap (l-proplist a1)))
        (cond
            ((and ap (l-assoc 'expr ap)) => 
                (lambda (p) (l-evlam (cddr p) (l-evlis a2 cx) cx)))
            ((and ap (l-assoc 'fexpr ap)) =>
                (lambda (p) (l-evlam (cddr p) a2 cx)))
            ((and ap (l-assoc 'subr ap)) =>
                (lambda (p) ((cdr p) (l-evlis a2 cx))))
	    ((and ap (l-assoc 'fsubr ap)) =>
                (lambda (p) ((cdr p) a2 cx)))
            ((and ap (l-assoc 'apval ap)) 
                (l-error "Invalid constant function"))
            ((l-assoc a1 cx) =>
                (lambda (p) (l-apply (cdr p) a2 cx)))
            (else (l-error "Unbound variable"))
)   )   )

; Вычисление лямбда-выражения
(define (l-evlam a1 a2 cx)
    (l-eval (cadr a1) (l-pairlis (car a1) a2 cx))
)

; Вычисление выражения FUNCTION
(define (l-evfarg a1 a2 cx)
    (l-evlam (car a1) (l-evlis a2 (cdr a1)) (cdr a1))
)

; Расширение контекста списком переменных и значений
(define (l-pairlis a1 a2 cx)
    (cond
	((null? a1) cx)
	((null? a2) (l-error "Bad pairlis args"))
	(else (l-pairlis (cdr a1) (cdr a2) (cons (cons (car a1) (car a2)) cx)))
)   )

; Изменение атрибута атома в таблице атомов
(define (l-put atom prop val)
    (let ((ap (l-proplist atom)))
        (if (not ap)
            (set! l-atom-table
                (cons (cons atom (cons (cons prop val) nil)) l-atom-table))
            (let ((p (l-assoc prop ap)))
                (if p
                    (set-cdr! p val)
                    (set-cdr! ap (cons (cons prop val) (cdr ap)))
)   )   )   )   )                    

; Функция SEXPR
(define (f-sexpr a1 cx)
    (let ((name (car a1))
          (val (cadr a1)))
        (cond
            ((not (symbol? name)) 
                (l-error "First argument of SEXPR is not symbol:" name))
            ((not (eq? (car val) 'lambda))
                (l-error "Second argument of SEXPR is not lambda:" (car val)))
            (else (l-put name 'expr val) name)
)   )   )

; Функция SFEXPR    
(define (f-sfexpr a1 cx)
    (let ((name (car a1))
          (val (cadr a1)))
        (cond
            ((not (symbol? name)) 
                (l-error "First argument of FEXPR is not symbol:" name))
            ((not (eq? (car val) 'lambda))
                (l-error "Second argument of FEXPR is not lambda:" (car val)))
            (else (l-put name 'fexpr val) name)
)   )   )

; Функция CSETQ    
(define (f-csetq a1 cx)
    (let ((name (car a1)))
        (if (not (symbol? name)) 
            (l-error "First argument of CSETQ is not symbol:" name)
	)
	(l-put name 'apval (l-eval (cadr a1) cx))
	name
)   )
    
; Функция QUOTE
(define (f-quote x cx)
    (if (not (pair? x))
        (l-error "Bad argument to QUOTE:" x)
        (car x)
)   )

; Функция CAR
(define (f-car x)
    (if (or (not (pair? x))
            (not (pair? (car x))))
        (l-error "Bad argument to CAR:" x)
        (caar x)
)   )

; Функция CDR
(define (f-cdr x)
    (if (or (not (pair? x))
            (not (pair? (car x))))
        (l-error "Bad argument to CDR:" x)
        (cdar x)
)   )

; Функция CONS
(define (f-cons x)
    (if (or (not (pair? x)) 
            (not (pair? (cdr x))))
        (l-error "Bad argument to CONS:" x)
        (cons (car x) (cadr x))
)   )

; Предикат ATOM
(define (f-atom x)
    (let ((arg (car x))) 
        (cond
            ((null? arg) t)
            ((symbol? arg) t)
            ((number? arg) t)
            (else nil)
)   )   )

; Предикат NUMBERP
(define (f-numberp x)
    (if (number? (car x)) t nil)
)

; Предикат EQ
(define (f-eq x)
    (let ((a (car x))
          (b (cadr x))) 
        (cond
            ((and (null? a) (null? b)) t)
            ((and (symbol? a) (eq? a b)) t)
            ((and (number? a) (eq? a b)) t)
            (else nil)
)   )   )

; Предикат NULL, он же NOT
(define (f-null x)
    (if (null? (car x)) t nil)
)

; Вычисление списка выражений,
; возвращает значение последнего выражения в списке.
(define (l-sequence x cx)
    (cond
        ((null? x) nil)
        ((not (pair? x)) (l-error "Bad sequence" x))
        ((null? (cdr x)) (l-eval (car x) cx))
        (else
	    (set! l-prog-switch 'step)			; для GO и RETURN
            (let ((rez (l-eval (car x) cx)))
                (if (not (eq? l-prog-switch 'step))
                    rez
                    (l-sequence (cdr x) cx)
)   )   )   )   )

; Функция COND
(define (f-cond x cx)
    (cond
        ((null? x) nil)
        ((not (pair? (car x))) (l-error "Bad COND" (car x)))
        ((eq? t (l-eval (caar x) cx)) (l-sequence (cdar x) cx))
        (else (f-cond (cdr x) cx))
)   )                    

; Создание списка меток для тела конструкции PROG
; rez - накапливающий параметр
(define (l-segment x rez)
    (cond
        ((null? x) rez)
        ((symbol? (car x)) (l-segment (cdr x) (cons x rez)))
        (else (l-segment (cdr x) rez))
)   )

; Переход на метку в теле PROG
(define (l-lassoc label lablist)
    (cond
        ((null? lablist) (l-error "Bad GO:" label))
        ((eq? label (caar lablist)) (car lablist))
        (else (l-lassoc label (cdr lablist)))
)   )

; Цикл функции PROG
(define (l-prog-loop stlist lablist extcx)
    (define rez nil)
    (cond
	((null? stlist) nil)
	((symbol? (car stlist))				; пропускаем метку
	    (l-prog-loop (cdr stlist) lablist extcx))
	(else
	    (set! l-prog-switch 'step)			; для GO и RETURN
	    (set! rez (l-eval (car stlist) extcx))
	    (case l-prog-switch
		((step) (l-prog-loop (cdr stlist) lablist extcx))
		((ret) rez)
		((go) (l-prog-loop (cdr (l-lassoc (car rez) lablist))
		        lablist extcx))
		(else (l-error "Bad l-prog-switch:" l-prog-switch))
)   )   )   )

; Функция PROG
(define (f-prog x cx)
    (if (or (null? x) (not (pair? x)))
        (l-error "Bad PROG" (car x)))
    (l-prog-loop (cdr x)			; список операторов
        (l-segment (cdr x) '())			; список меток
        (if (null? (car x)) cx
	    (l-pairlis (car x) nillist cx)))	; расширенный контекст
)

; Функция GO
(define (f-go label cx)
    (set! l-prog-switch 'go)
    (if (or (not (pair? label)) (not (symbol? (car label))))
        (l-error "Bad GO label:" label))
    label
)

; Функция RETURN
(define (f-return value)
    (set! l-prog-switch 'ret)
    (if (not (pair? value))
        (l-error "Bad RETURN value:" value))
    (car value)
)

; Функция SETQ
(define (f-setq a1 cx)
    (let ((name (car a1)))
        (if (not (symbol? name)) 
            (l-error "First argument of SETQ is not symbol:" name))
	(let ((ap (l-assoc name cx)))
	    (if (not ap) 
		(l-error "Unbound valiable in SETQ:" name))
            (let ((val (l-eval (cadr a1) cx)))
		(set-cdr! ap val)
		val
)   )   )   )
    
; Функция SET
(define (f-set x cx)
    (let ((name (l-eval (car x) cx))
          (val (l-eval (cadr x) cx)))
        (if (not (symbol? name)) 
            (l-error "First argument of SET is not symbol:" name))
	(let ((ap (l-assoc name cx)))
	    (if (not ap) 
		(l-error "Unbound valiable in SET:" name))
	    (set-cdr! ap val)
	    val
)   )   )
    
; Чтение списков - функция READ
(define (f-read x) (read))

; Печать списков - функция PRINT
(define (f-print x) (write (car x)) (newline) (car x))

; Генератор символов GENSYM
(define (f-gensym x)
    (set! l-gensym-cnt (+ 1 l-gensym-cnt))
    (string->symbol (string-append "G" (number->string l-gensym-cnt)))
) 

; Функция LOAD
(define (f-load x cx)
    (let ((filename (car x)))
        (if (not (string? filename)) 
            (l-error "Bad LOAD:" filename))
        (call-with-input-file filename flisp)
)   )
    
; Функция EVAL
(define (f-eval x cx)
    (if (null? x)
        (l-error "Bad EVAL"))
    (l-eval (l-eval (car x) cx) cx)
)

; Функция LIST
(define (f-list x cx)
    (if (null? x) nil
	(cons (l-eval (car x) cx) (f-list (cdr x) cx))
)   )

; Функция AND
(define (f-and x cx)
    (cond
        ((null? x) t)
	((null? (l-eval (car x) cx)) nil)
	(else (f-and (cdr x) cx))
)   )

; Функция OR
(define (f-or x cx)
    (cond
        ((null? x) nil)
	((null? (l-eval (car x) cx)) (f-or (cdr x) cx))
	(else t)
)   )

; Функция PLUS
(define (f-plus x)
    (if (null? x) (l-error "No arguments to PLUS"))
    (if (null? (cdr x)) (l-error "Not enough arguments to PLUS"))
    (+ (car x) (cadr x))
)

; Функция TIMES
(define (f-times x)
    (if (null? x) (l-error "No arguments to TIMES"))
    (if (null? (cdr x)) (l-error "Not enough arguments to TIMES"))
    (* (car x) (cadr x))
)

; Функция DIFFERENCE
(define (f-difference x)
    (if (null? x) (l-error "No arguments to DIFFERENCE"))
    (if (null? (cdr x)) (l-error "Not enough arguments to DIFFERENCE"))
    (- (car x) (cadr x))
)

; Функция QUOTIENT
(define (f-quotient x)
    (if (null? x) (l-error "No arguments to QUOTIENT"))
    (if (null? (cdr x)) (l-error "Not enough arguments to QUOTIENT"))
    (quotient (car x) (cadr x))
)

; Функция REMAINDER
(define (f-remainder x)
    (if (null? x) (l-error "No arguments to REMAINDER"))
    (if (null? (cdr x)) (l-error "Not enough arguments to REMAINDER"))
    (remainder (car x) (cadr x))
)

; Функция EXPT - X в степени Y
(define (f-expt x)
    (if (null? x) (l-error "No arguments to EXPT"))
    (if (null? (cdr x)) (l-error "Not enough arguments to EXPT"))
    (if (< (cadr x) 0) (l-error "Negative power in EXPT"))
    (round (expt (car x) (cadr x)))
)

; Функция MINUS
(define (f-minus x)
    (if (null? x) (l-error "No arguments to MINUS"))
    (- (car x))
)

; Функция ADD1
(define (f-add1 x)
    (if (null? x) (l-error "No arguments to ADD1"))
    (+ (car x) 1)
)

; Функция SUB1
(define (f-sub1 x)
    (if (null? x) (l-error "No arguments to SUB1"))
    (- (car x) 1)
)

; Функция MAX
(define (f-max x)
    (if (null? x) (l-error "No arguments to MAX"))
    (apply max x)
)

; Функция MIN
(define (f-min x)
    (if (null? x) (l-error "No arguments to MIN"))
    (apply min x)
)

; Функция MINUSP
(define (f-minusp x)
    (if (null? x) (l-error "No arguments to MINUSP"))
    (if (negative? (car x)) t nil)
)

; Функция ZEROP
(define (f-zerop x)
    (if (null? x) (l-error "No arguments to ZEROP"))
    (if (zero? (car x)) t nil)
)

; Функция ONEP
(define (f-onep x)
    (if (null? x) (l-error "No arguments to ONEP"))
    (if (= (car x) 1) t nil)
)

; Функция LESSP
(define (f-lessp x)
    (if (null? x) (l-error "No arguments to LESSP"))
    (if (null? (cdr x)) (l-error "Not enough arguments to LESSP"))
    (if (< (car x) (cadr x)) t nil)
)

; Функция GREATERP
(define (f-greaterp x)
    (if (null? x) (l-error "No arguments to GREATERP"))
    (if (null? (cdr x)) (l-error "Not enough arguments to GREATERP"))
    (if (> (car x) (cadr x)) t nil)
)

; Функция GREQP
(define (f-greqp x)
    (if (null? x) (l-error "No arguments to GREQP"))
    (if (null? (cdr x)) (l-error "Not enough arguments to GREQP"))
    (if (>= (car x) (cadr x)) t nil)
)

; Функция MAPLIST
(define (f-maplist x)
    (if (null? x) (l-error "No arguments to MAPLIST"))
    (if (null? (cdr x)) (l-error "Not enough arguments to MAPLIST"))
    (l-maplist (car x) (cadr x) '())
)

(define (l-maplist x f rez)
    (if (null? x) rez
        (l-maplist (cdr x) f (cons (f x) rez))
)   )

; Функция MAPCAR
(define (f-mapcar x)
    (if (null? x) (l-error "No arguments to MAPCAR"))
    (if (null? (cdr x)) (l-error "Not enough arguments to MAPCAR"))
    (l-mapcar (car x) (cadr x) '())
)

(define (l-mapcar x f rez)
    (if (null? x) rez
        (l-mapcar (cdr x) f (cons (f (car x)) rez))
)   )

; Функция MAP
(define (f-map x)
    (if (null? x) (l-error "No arguments to MAP"))
    (if (null? (cdr x)) (l-error "Not enough arguments to MAP"))
    (l-map (car x) (cadr x))
)

(define (l-map x f)
    (cond
        ((null? x) nil)
        ((not (pair? x)) x)
        (else (f x) (l-map (cdr x) f))
)   )

; Функция SELECTQ
(define (f-selectq x cx)
    (if (null? x) (l-error "No arguments to SELECTQ"))
    (if (or (null? (cdr x)) (null? (cddr x)))
        (l-error "Not enough arguments to SELECTQ"))
    (l-selectq (car x) (cadr x) (caddr x) cx)
)

(define (l-selectq e l e0 cx)
    (cond
        ((null? l) (l-eval e0 cx))
        ((and (pair? (car l)) (memv (l-eval e cx) (car l)))
            (l-eval (cadr l) cx))
        ((eq? (l-eval e cx) (car l)) (l-eval (cadr l) cx))
        (else (l-selectq e (cddr l) e0 cx))
)   )

; Таблица атомов (имя - список свойств)
; Есть пять стандартных свойств:
; apval - значение константы
; expr - функция
; fexpr - спец. функция
; subr - встроенная функция
; fsubr - встроенная спец. функция
(set! l-atom-table `(
    (nil	(apval . ,nil))
    (t		(apval . ,t))
    (quote	(fsubr . ,f-quote))
    (car	(subr  . ,f-car))
    (cdr	(subr  . ,f-cdr))
    (cons	(subr  . ,f-cons))
    (atom	(subr  . ,f-atom))
    (eq		(subr  . ,f-eq))
    (null	(subr  . ,f-null))
    (not	(subr  . ,f-null))
    (cond	(fsubr . ,f-cond))
    (sexpr	(fsubr . ,f-sexpr))
    (sfexpr	(fsubr . ,f-sfexpr))
    (csetq	(fsubr . ,f-csetq))
    (prog	(fsubr . ,f-prog))
    (go		(fsubr . ,f-go))
    (return	(subr  . ,f-return))
    (setq	(fsubr . ,f-setq))
    (set	(fsubr . ,f-set))
    (read	(subr  . ,f-read))
    (print	(subr  . ,f-print))
    (gensym	(subr  . ,f-gensym))
    (load	(fsubr . ,f-load))
    (eval	(fsubr . ,f-eval))
    (list	(fsubr . ,f-list))
    (and	(fsubr . ,f-and))
    (or		(fsubr . ,f-or))
    (numberp	(subr  . ,f-numberp))
    (plus	(subr  . ,f-plus))
    (times	(subr  . ,f-times))
    (difference	(subr  . ,f-difference))
    (minus	(subr  . ,f-minus))
    (quotient	(subr  . ,f-quotient))
    (remainder	(subr  . ,f-remainder))
    (expt	(subr  . ,f-expt))
    (add1	(subr  . ,f-add1))
    (sub1	(subr  . ,f-sub1))
    (max	(subr  . ,f-max))
    (min	(subr  . ,f-min))
    (lessp	(subr  . ,f-lessp))
    (greaterp	(subr  . ,f-greaterp))
    (greqp	(subr  . ,f-greqp))
    (minusp	(subr  . ,f-minusp))
    (zerop	(subr  . ,f-zerop))
    (onep	(subr  . ,f-onep))
    (maplist	(subr  . ,f-maplist))
    (mapcar	(subr  . ,f-mapcar))
    (map	(subr  . ,f-map))
    (selectq	(fsubr . ,f-selectq))
))

; Фатальная ошибка интерпретатора
(define (l-error . msg)
    (display "*** ")
    (display msg)
    (newline)
    (l-abort)
)

(define (lisp)
    (newline)
    (display "L> ")
    (let ((expr (read)))
        (if (not (eof-object? expr))
            (begin
                (write (l-eval expr nil))
                (lisp)
)   )   )   )

(define (flisp port)
    (let ((expr (read port)))
        (if (eof-object? expr)
            nil
            (begin
                (write (l-eval expr nil)) (newline)
                (flisp port)
)   )   )   )

(define l-abort abort)
(let ((again (call-with-current-continuation (lambda (ret)
        (letrec ((p (lambda () (ret p)))) p)))))
    (set! l-abort again)
    (display "VLisp Interpreter")
    (lisp)
    (again)
)
; (quit)
