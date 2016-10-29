#include "scm.h"

#define MAXARG 64       /* максимальное количество аргументов функции map */

static long divide (long a, long b)
{
	if (a > 0)
		if (b > 0)      return a / b;
		else            return - a / (-b);
	else    if (b > 0)      return - (-a) / b;
		else            return (-a) / (-b);
}

static long gcd (long a, long b)
{
	long tmp;

	if (a < 0)
		a = -a;
	if (b < 0)
		b = -b;
	if (b > a)
		tmp = a, a = b, b = tmp;
	for (;;) {
		if (b == 0)
			return a;
		if (b == 1)
			return b;
		b = a % (tmp = b);
		a = tmp;
	}
}

static long lcm (long x, long y)
{
	long r;

	if (x == 0 || y == 0)
		return (0);
	r = x * y / gcd (x, y);
	return (r<0 ? -r : r);
}

static long ipow (long x, long p)
{
	long r;

	if (p == 0)
		return (1);
	if (p < 0 || x == 0)
		return (0);
	r = 1;
	for (;;) {
		if (p & 1)
			r *= x;
		if (! (p >>= 1))
			return (r);
		x *= x;
	}
}

#define CXR(func,op) LISP func (LISP a, LISP ctx) { \
	if (! istype (a, TPAIR) || ! istype (a = car (a), TPAIR)) \
	return (NIL); return (op (a)); }

CXR (Fcar, car)
CXR (Fcdr, cdr)

#define CXXR(func,op1,op2) LISP func (LISP a, LISP ctx) { \
	if (! istype (a, TPAIR) || ! istype (a = car (a), TPAIR) || \
	! istype (a = op1 (a), TPAIR)) return (NIL); return (op2 (a)); }

CXXR (Fcaar, car, car)
CXXR (Fcadr, cdr, car)
CXXR (Fcdar, car, cdr)
CXXR (Fcddr, cdr, cdr)

#define CXXXR(func,op1,op2,op3) LISP func (LISP a, LISP ctx) { \
	if (! istype (a, TPAIR) || ! istype (a = car (a), TPAIR) || \
	! istype (a = op1 (a), TPAIR) || ! istype (a = op2 (a), TPAIR)) \
	return (NIL); return (op3 (a)); }

CXXXR (Fcaaar, car, car, car)
CXXXR (Fcaadr, cdr, car, car)
CXXXR (Fcadar, car, cdr, car)
CXXXR (Fcaddr, cdr, cdr, car)
CXXXR (Fcdaar, car, car, cdr)
CXXXR (Fcdadr, cdr, car, cdr)
CXXXR (Fcddar, car, cdr, cdr)
CXXXR (Fcdddr, cdr, cdr, cdr)

#define CXXXXR(func,op1,op2,op3,op4) LISP func (LISP a, LISP ctx) { \
	if (! istype (a, TPAIR) || ! istype (a = car (a), TPAIR) || \
	! istype (a = op1 (a), TPAIR) || ! istype (a = op2 (a), TPAIR) || \
	! istype (a = op3 (a), TPAIR)) return (NIL); return (op4 (a)); }

CXXXXR (Fcaaaar, car, car, car, car)
CXXXXR (Fcaaadr, cdr, car, car, car)
CXXXXR (Fcaadar, car, cdr, car, car)
CXXXXR (Fcaaddr, cdr, cdr, car, car)
CXXXXR (Fcadaar, car, car, cdr, car)
CXXXXR (Fcadadr, cdr, car, cdr, car)
CXXXXR (Fcaddar, car, cdr, cdr, car)
CXXXXR (Fcadddr, cdr, cdr, cdr, car)
CXXXXR (Fcdaaar, car, car, car, cdr)
CXXXXR (Fcdaadr, cdr, car, car, cdr)
CXXXXR (Fcdadar, car, cdr, car, cdr)
CXXXXR (Fcdaddr, cdr, cdr, car, cdr)
CXXXXR (Fcddaar, car, car, cdr, cdr)
CXXXXR (Fcddadr, cdr, car, cdr, cdr)
CXXXXR (Fcdddar, car, cdr, cdr, cdr)
CXXXXR (Fcddddr, cdr, cdr, cdr, cdr)

/*
 * Создать новую пару из двух элементов.
 * (cons a (b)) ==> (a b)
 */
LISP Fcons (LISP arg, LISP ctx)
{
	LISP a;
	if (! istype (arg, TPAIR))
		return (NIL);
	a = car (arg);
	if (! istype (arg = cdr (arg), TPAIR))
		return (NIL);
	return (cons (a, car (arg)));
}

/*
 * Заменить голову списка.
 * (set-car! pair obj) ==> pair
 */
LISP Fsetcar (LISP arg, LISP ctx)
{
	LISP a;
	if (! istype (arg, TPAIR))
		return (NIL);
	a = car (arg);
	if (istype (arg = cdr (arg), TPAIR) && istype (a, TPAIR))
		setcar (a, car (arg));
	return (a);
}

/*
 * Заменить хвост списка.
 * (set-cdr! pair obj) ==> pair
 */
LISP Fsetcdr (LISP arg, LISP ctx)
{
	LISP a;
	if (! istype (arg, TPAIR))
		return (NIL);
	a = car (arg);
	if (istype (arg = cdr (arg), TPAIR) && istype (a, TPAIR))
		setcdr (a, car (arg));
	return (a);
}

/*
 * Вычислить логическое отрицание.
 * Выдает #t если аргумент равен #f или пустому списку.
 * (not (a b c)) ==> #f
 */
LISP Fnot (LISP arg, LISP ctx)
{
	return (istype (arg, TPAIR) && car (arg) == NIL ? T : NIL);
}

/*
 * Сравнить элементы списка.
 * (eqv? a b) ==> #f
 */
LISP Feqv (LISP arg, LISP ctx)
{
	LISP a;
	if (! istype (arg, TPAIR))
		return (T);
	a = car (arg);
	while (istype (arg = cdr (arg), TPAIR))
		if (! eqv (a, car (arg)))
			return (NIL);
	return (T);
}

/*
 * Сравнить элементы списка рекурсивно.
 * (equal? a b) ==> #f
 */
LISP Fequal (LISP arg, LISP ctx)
{
	LISP a;
	if (! istype (arg, TPAIR))
		return (T);
	a = car (arg);
	while (istype (arg = cdr (arg), TPAIR))
		if (! equal (a, car (arg)))
			return (NIL);
	return (T);
}

/*
 * Вычислить элементы списка.
 * (eval a [toplevel]) ==> aval
 * Если toplevel не равен NIL, вычисление происходит
 * в контексте верхнего уровня, и оператор define
 * будет добавлять новые переменные.
 */
LISP Feval (LISP a, LISP ctx)
{
	if (! istype (a, TPAIR))
		return (NIL);
	return (eval (car (a), ctx == TOPLEVEL || cdr (a) != NIL ? 0 : &ctx));
}

LISP Fabs (LISP arg, LISP ctx)
{
	long sum = 0;

	while (istype (arg, TPAIR)) {
		LISP b = car (arg);
		if (istype (b, TINTEGER)) {
			long v = numval (b);
			if (v > 0)
				sum += v;
			else
				sum -= v;
		}
		arg = cdr (arg);
	}
	return (number (sum));
}

LISP Fadd (LISP arg, LISP ctx)
{
	long sum = 0;

	while (istype (arg, TPAIR)) {
		LISP v = car (arg);
		if (istype (v, TINTEGER))
			sum += numval (v);
		arg = cdr (arg);
	}
	return (number (sum));
}

LISP Fmul (LISP arg, LISP ctx)
{
	long prod = 1;

	while (istype (arg, TPAIR)) {
		LISP v = car (arg);
		if (istype (v, TINTEGER))
			prod *= numval (v);
		arg = cdr (arg);
	}
	return (number (prod));
}

LISP Fsub (LISP arg, LISP ctx)
{
	long val;

	if (! istype (arg, TPAIR))
		return (ZERO);
	val = numval (car (arg));
	if (! istype (arg = cdr (arg), TPAIR))
		return (number (-val));
	do {
		LISP v = car (arg);
		if (istype (v, TINTEGER))
			val -= numval (v);
	} while (istype (arg = cdr (arg), TPAIR));
	return (number (val));
}

LISP Fdiv (LISP arg, LISP ctx)
{
	long val, p;

	if (! istype (arg, TPAIR))
		return (ZERO);
	val = numval (car (arg));
	if (! istype (arg = cdr (arg), TPAIR))
		return (ZERO);
	do {
		LISP v = car (arg);
		if (istype (v, TINTEGER) && (p = numval (v)))
			val /= p;
	} while (istype (arg = cdr (arg), TPAIR));
	return (number (val));
}

LISP Feq (LISP arg, LISP ctx)
{
	long val;

	if (! istype (arg, TPAIR))
		return (T);
	val = numval (car (arg));
	while (istype (arg, TPAIR)) {
		LISP v = car (arg);
		long nval = numval (v);
		if (! (istype (v, TINTEGER) && val == nval))
			return (NIL);
		arg = cdr (arg);
	}
	return (T);
}

#define DEFLOG(func,op) LISP func (LISP a, LISP ctx) { \
	long v, nv; if (! istype (a, TPAIR)) return (T); v = numval (car (a)); \
	while (istype (a = cdr (a), TPAIR)) { LISP b = car (a); \
	if (! istype (b, TINTEGER) || ! (v op (nv = numval (b)))) return (NIL); \
	v = nv; } return (T); }

DEFLOG (Flt, <);
DEFLOG (Fgt, >);
DEFLOG (Fle, <=);
DEFLOG (Fge, >=);

LISP Fifboolean (LISP arg, LISP ctx)
{
	while (istype (arg, TPAIR)) {
		LISP a = car (arg);
		if (a != NIL && ! istype (a, TBOOL))
			return (NIL);
		arg = cdr (arg);
	}
	return (T);
}

LISP Fifsymbol (LISP arg, LISP ctx)
{
	while (istype (arg, TPAIR)) {
		LISP a = car (arg);
		if (! istype (a, TSYMBOL))
			return (NIL);
		arg = cdr (arg);
	}
	return (T);
}

LISP Fifpair (LISP arg, LISP ctx)
{
	while (istype (arg, TPAIR)) {
		LISP a = car (arg);
		if (! istype (a, TPAIR))
			return (NIL);
		arg = cdr (arg);
	}
	return (T);
}

#define DEFNUMLOG(func,test) LISP func (LISP arg, LISP ctx) { \
	while (istype (arg, TPAIR)) { LISP a = car (arg); \
		if (! (istype (a, TINTEGER) test)) return (NIL); \
		arg = cdr (arg); } return (T); }

DEFNUMLOG (Fifnumber, )
DEFNUMLOG (Fifzero, && numval (a) == 0)
DEFNUMLOG (Fifpositive, && numval (a) > 0)
DEFNUMLOG (Fifnegative, && numval (a) < 0)
DEFNUMLOG (Fifodd, && (numval (a) & 1))
DEFNUMLOG (Fifeven, && ! (numval (a) & 1))

LISP Fmax (LISP arg, LISP ctx)
{
	long val;

	if (! istype (arg, TPAIR))
		return (ZERO);
	val = numval (car (arg));
	while (istype (arg = cdr (arg), TPAIR)) {
		LISP b = car (arg);
		if (istype (b, TINTEGER)) {
			long v = numval (b);
			if (v > val)
				v = val;
		}
	}
	return (number (val));
}

LISP Fmin (LISP arg, LISP ctx)
{
	long val;

	if (! istype (arg, TPAIR))
		return (ZERO);
	val = numval (car (arg));
	while (istype (arg = cdr (arg), TPAIR)) {
		LISP b = car (arg);
		if (istype (b, TINTEGER)) {
			long v = numval (b);
			if (v < val)
				v = val;
		}
	}
	return (number (val));
}

LISP Fgcd (LISP arg, LISP ctx)
{
	long val = 0;
	while (istype (arg, TPAIR)) {
		LISP b = car (arg);
		if (istype (b, TINTEGER))
			val = gcd (val, numval (b));
		arg = cdr (arg);
	}
	return (number (val));
}

LISP Flcm (LISP arg, LISP ctx)
{
	long val = 1;
	while (istype (arg, TPAIR)) {
		LISP b = car (arg);
		if (istype (b, TINTEGER))
			val = lcm (val, numval (b));
		arg = cdr (arg);
	}
	return (number (val));
}

LISP Fquotient (LISP arg, LISP ctx)
{
	long a, b;

	if (! istype (arg, TPAIR) || ! istype (car (arg), TINTEGER))
		return (ZERO);
	a = numval (car (arg));
	if (! istype (arg = cdr (arg), TPAIR) ||
	    ! istype (car (arg), TINTEGER) || (b = numval (car (arg))) == 0)
		return (ZERO);
	return (number (divide (a, b)));
}

LISP Fremainder (LISP arg, LISP ctx)
{
	long a, b;

	if (! istype (arg, TPAIR) || ! istype (car (arg), TINTEGER))
		return (ZERO);
	a = numval (car (arg));
	if (! istype (arg = cdr (arg), TPAIR) ||
	    ! istype (car (arg), TINTEGER) || (b = numval (car (arg))) == 0)
		return (ZERO);
	return (number (a - divide (a, b) * b));
}

LISP Fmodulo (LISP arg, LISP ctx)
{
	long a, b, r;

	if (! istype (arg, TPAIR) || ! istype (car (arg), TINTEGER))
		return (ZERO);
	a = numval (car (arg));
	if (! istype (arg = cdr (arg), TPAIR) ||
	    ! istype (car (arg), TINTEGER) || (b = numval (car (arg))) == 0)
		return (ZERO);
	r = a - divide (a, b) * b;
	if ((b>0 && r<0) || (b<0 && r>0))
		r += b;
	return (number (r));
}

LISP Fexpt (LISP arg, LISP ctx)
{
	long r = 1;

	if (istype (arg, TPAIR) && istype (car (arg), TINTEGER)) {
		long a = numval (car (arg));
		if (istype (arg = cdr (arg), TPAIR) &&
		    istype (car (arg), TINTEGER))
			r = ipow (a, numval (car (arg)));
	}
	return (number (r));
}

LISP Fifnull (LISP arg, LISP ctx)
{
	while (istype (arg, TPAIR)) {
		LISP a = car (arg);
		if (a != NIL)
			return (NIL);
		arg = cdr (arg);
	}
	return (T);
}

LISP Fifprocedure (LISP arg, LISP ctx)
{
	while (istype (arg, TPAIR)) {
		LISP a = car (arg);
		if (! istype (a, THARDW) && ! istype (a, TCLOSURE))
			return (NIL);
		arg = cdr (arg);
	}
	return (T);
}

LISP Fiflist (LISP arg, LISP ctx)
{
	while (istype (arg, TPAIR)) {
		LISP a = car (arg);
		while (istype (a, TPAIR))
			a = cdr (a);
		if (a != NIL)
			return (NIL);
		arg = cdr (arg);
	}
	return (T);
}

LISP Fread (LISP arg, LISP ctx)
{
	return (getexpr ());
}

LISP Fwrite (LISP arg, LISP ctx)
{
	int first = 1;
	while (istype (arg, TPAIR)) {
		if (first)
			first = 0;
		else
			putc (' ', stdout);
		putexpr (car (arg), stdout);
		arg = cdr (arg);
	}
	return (NIL);
}

LISP Fnewline (LISP arg, LISP ctx)
{
	putc ('\n', stdout);
	return (NIL);
}

/*
 * Выдать текущий контекст.
 */
LISP Fcontext (LISP arg, LISP ctx)
{
	return (ctx==TOPLEVEL ? NIL : ctx);
}

/*
 * Выдать контекст верхнего уровня.
 */
LISP Fgcontext (LISP arg, LISP ctx)
{
	return (ENV);
}

LISP Flist (LISP arg, LISP ctx)
{
	return (copy (arg, 0));
}

LISP Fappend (LISP arg, LISP ctx)
{
	LISP val = NIL, tail;
	while (istype (arg, TPAIR)) {
		if (val == NIL)
			val = copy (car (arg), &tail);
		else {
			LISP newtail;
			setcdr (tail, copy (car (arg), &newtail));
			tail = newtail;
		}
		arg = cdr (arg);
	}
	return (val);
}

LISP Freverse (LISP arg, LISP ctx)
{
	LISP val = NIL;
	if (! istype (arg, TPAIR))
		return (NIL);
	arg = car (arg);
	while (istype (arg, TPAIR)) {
		val = cons (car (arg), val);
		arg = cdr (arg);
	}
	return (val);
}

LISP Flength (LISP arg, LISP ctx)
{
	long len = 0;
	if (! istype (arg, TPAIR))
		return (ZERO);
	for (arg=car(arg); istype (arg, TPAIR); arg=cdr(arg))
		++len;
	return (number (len));
}

LISP Flisttail (LISP arg, LISP ctx)
{
	LISP l;
	int k;
	if (! istype (arg, TPAIR))
		return (NIL);
	l = car (arg);
	if (! istype (arg = cdr (arg), TPAIR) ||
	    ! istype (arg = car (arg), TINTEGER))
		return (l);
	for (k=numval(arg); k>0; --k) {
		if (! istype (l, TPAIR))
			return (NIL);
		l = cdr (l);
	}
	return (l);
}

LISP Flistref (LISP arg, LISP ctx)
{
	LISP l;
	int k;
	if (! istype (arg, TPAIR))
		return (NIL);
	l = car (arg);
	if (! istype (l = car (arg), TPAIR))
		return (NIL);
	if (! istype (arg = cdr (arg), TPAIR) ||
	    ! istype (arg = car (arg), TINTEGER))
		return (car (l));
	for (k=numval(arg); k>0; --k) {
		l = cdr (l);
		if (! istype (l, TPAIR))
			return (NIL);
	}
	return (car (l));
}

LISP member (LISP o, LISP arg, int (*compare) (LISP, LISP))
{
	while (istype (arg, TPAIR)) {
		if ((*compare) (o, car (arg)))
			return (arg);
		arg = cdr (arg);
	}
	return (NIL);
}

LISP assoc (LISP o, LISP arg, int (*compare) (LISP, LISP))
{
	LISP p;
	while (istype (arg, TPAIR)) {
		if (istype (p = car (arg), TPAIR) && (*compare) (o, car (p)))
			return (p);
		arg = cdr (arg);
	}
	return (NIL);
}

LISP Fmemv (LISP arg, LISP ctx)
{
	LISP o;
	if (! istype (arg, TPAIR))
		return (NIL);
	o = car (arg);
	if (! istype (arg = cdr (arg), TPAIR))
		return (NIL);
	return (member (o, car (arg), eqv));
}

LISP Fmember (LISP arg, LISP ctx)
{
	LISP o;
	if (! istype (arg, TPAIR))
		return (NIL);
	o = car (arg);
	if (! istype (arg = cdr (arg), TPAIR))
		return (NIL);
	return (member (o, car (arg), equal));
}

LISP Fassv (LISP arg, LISP ctx)
{
	LISP o;
	if (! istype (arg, TPAIR))
		return (NIL);
	o = car (arg);
	if (! istype (arg = cdr (arg), TPAIR))
		return (NIL);
	return (assoc (o, car (arg), eqv));
}

LISP Fassoc (LISP arg, LISP ctx)
{
	LISP o;
	if (! istype (arg, TPAIR))
		return (NIL);
	o = car (arg);
	if (! istype (arg = cdr (arg), TPAIR))
		return (NIL);
	return (assoc (o, car (arg), equal));
}

LISP Fapply (LISP arg, LISP ctx)
{
	LISP func;
	if (! istype (arg, TPAIR))
		return (NIL);
	func = car (arg);
	arg = istype (arg = cdr (arg), TPAIR) ? car (arg) : NIL;
	return (evalfunc (func, arg, ctx));
}

static LISP makearg (int n, LISP *vect)
{
	LISP arg = NIL, v;
	int i;

	for (i=n-1; i>=0; --i) {
		if (istype (vect[i], TPAIR)) {
			v = car (vect[i]);
			vect[i] = cdr (vect[i]);
		} else
			v = NIL;
		arg = cons (v, arg);
	}
	return (arg);
}

LISP Fmap (LISP arg, LISP ctx)
{
	LISP func, vect[MAXARG], tail, val;
	int n;

	if (! istype (arg, TPAIR))
		return (NIL);
	func = car (arg);
	if (! istype (arg = cdr (arg), TPAIR))
		return (NIL);

	/* составляем массив списков аргументов */
	for (n=0; n<MAXARG && istype (arg, TPAIR); arg=cdr(arg))
		if (istype (vect[n] = car (arg), TPAIR))
			++n;
	if (! n)
		return (NIL);

	tail = val = cons (NIL, NIL);
	for (;;) {
		setcar (tail, evalfunc (func, makearg (n, vect), ctx));
		if (! istype (vect[0], TPAIR))
			break;
		setcdr (tail, cons (NIL, NIL));
		tail = cdr (tail);
	}
	return (val);
}

LISP Fforeach (LISP arg, LISP ctx)
{
	LISP func, vect[MAXARG];
	int n;

	if (! istype (arg, TPAIR))
		return (NIL);
	func = car (arg);
	if (! istype (arg = cdr (arg), TPAIR))
		return (NIL);

	/* составляем массив списков аргументов */
	for (n=0; n<MAXARG && istype (arg, TPAIR); arg=cdr(arg))
		if (istype (vect[n] = car (arg), TPAIR))
			++n;
	if (! n)
		return (NIL);

	while (istype (vect[0], TPAIR))
		evalfunc (func, makearg (n, vect), ctx);
	return (NIL);
}

LISP Fifstring (LISP arg, LISP ctx)
{
	while (istype (arg, TPAIR)) {
		LISP a = car (arg);
		if (! istype (a, TSTRING))
			return (NIL);
		arg = cdr (arg);
	}
	return (T);
}

LISP Fmakestring (LISP arg, LISP ctx)
{
	LISP s = string (0, "");
	int k, f = ' ';

	if (!istype (arg, TPAIR) || !istype (car (arg), TINTEGER))
		return (s);
	k = numval (car (arg));
	if (istype (arg = cdr (arg), TPAIR) &&
	    istype (arg = car (arg), TCHAR))
		f = charval (arg);
	if (k > 0) {
		void *array = malloc (k);
		if (! array)
			fatal ("no memory for string");
		memset (array, f, k);
		mem[s].as.string.length = k;
		mem[s].as.string.array = array;
	}
	return (s);
}

LISP Fstring (LISP arg, LISP ctx)
{
	LISP a, s = string (0, "");
	int k = 0;

	for (a=arg; istype (a, TPAIR) && istype (car (a), TCHAR); a=cdr(a))
		++k;
	if (k > 0) {
		void *array = malloc (k);
		char *p = array;
		if (! array)
			fatal ("no memory for string");
		for (a=arg; istype (a, TPAIR) && istype (car (a), TCHAR); a=cdr(a))
			*p++ = charval (car (a));
		mem[s].as.string.length = k;
		mem[s].as.string.array = array;
	}
	return (s);
}

LISP Fstringlength (LISP arg, LISP ctx)
{
	if (! istype (arg, TPAIR) || ! istype (arg = car (arg), TSTRING))
		return (ZERO);
	return (number (mem[arg].as.string.length));
}

functab stdfunc [] = {
	{ "car",                    Fcar		},
	{ "cdr",                    Fcdr		},

	{ "caar",                   Fcaar		},
	{ "cadr",                   Fcadr		},
	{ "cdar",                   Fcdar		},
	{ "cddr",                   Fcddr		},

	{ "caaar",                  Fcaaar		},
	{ "caadr",                  Fcaadr		},
	{ "cadar",                  Fcadar		},
	{ "caddr",                  Fcaddr		},
	{ "cdaar",                  Fcdaar		},
	{ "cdadr",                  Fcdadr		},
	{ "cddar",                  Fcddar		},
	{ "cdddr",                  Fcdddr		},

	{ "caaaar",                 Fcaaaar		},
	{ "caaadr",                 Fcaaadr		},
	{ "caadar",                 Fcaadar		},
	{ "caaddr",                 Fcaaddr		},
	{ "cadaar",                 Fcadaar		},
	{ "cadadr",                 Fcadadr		},
	{ "caddar",                 Fcaddar		},
	{ "cadddr",                 Fcadddr		},
	{ "cdaaar",                 Fcdaaar		},
	{ "cdaadr",                 Fcdaadr		},
	{ "cdadar",                 Fcdadar		},
	{ "cdaddr",                 Fcdaddr		},
	{ "cddaar",                 Fcddaar		},
	{ "cddadr",                 Fcddadr		},
	{ "cdddar",                 Fcdddar		},
	{ "cddddr",                 Fcddddr		},

	{ "cons",                   Fcons		},
	{ "set-car!",               Fsetcar		},
	{ "set-cdr!",               Fsetcdr		},
	{ "not",                    Fnot		},
	{ "current-environment",    Fcontext		},
	{ "global-environment",     Fgcontext		},
	{ "eq?",                    Feqv		},
	{ "eqv?",                   Feqv		},
	{ "equal?",                 Fequal		},
	{ "eval",                   Feval		},
	{ "boolean?",               Fifboolean		},
	{ "symbol?",                Fifsymbol		},
	{ "pair?",                  Fifpair		},
	{ "procedure?",             Fifprocedure	},
	{ "null?",                  Fifnull		},
	{ "list?",                  Fiflist		},
	{ "read",                   Fread		},
	{ "write",                  Fwrite		},
	{ "display",                Fwrite		},
	{ "newline",                Fnewline		},

	{ "number?",                Fifnumber		},
	{ "integer?",               Fifnumber		},
	{ "zero?",                  Fifzero		},
	{ "positive?",              Fifpositive		},
	{ "negative?",              Fifnegative		},
	{ "odd?",                   Fifodd		},
	{ "even?",                  Fifeven		},
	{ "+",                      Fadd		},
	{ "-",                      Fsub		},
	{ "*",                      Fmul		},
	{ "/",                      Fdiv		},
	{ "<",                      Flt			},
	{ ">",                      Fgt			},
	{ "<=",                     Fle			},
	{ ">=",                     Fge			},
	{ "=",                      Feq			},
	{ "max",                    Fmax		},
	{ "min",                    Fmin		},
	{ "abs",                    Fabs		},
	{ "gcd",                    Fgcd		},
	{ "lcm",                    Flcm		},
	{ "quotient",               Fquotient		},
	{ "remainder",              Fremainder		},
	{ "modulo",                 Fmodulo		},
	{ "expt",                   Fexpt		},

	{ "list",                   Flist		},
	{ "length",                 Flength		},
	{ "append",                 Fappend		},
	{ "reverse",                Freverse		},
	{ "list-tail",              Flisttail		},
	{ "list-ref",               Flistref		},

	{ "memq",                   Fmemv		},
	{ "memv",                   Fmemv		},
	{ "member",                 Fmember		},
	{ "assq",                   Fassv		},
	{ "assv",                   Fassv		},
	{ "assoc",                  Fassoc		},
	{ "apply",                  Fapply		},
	{ "for-each",               Fforeach		},
	{ "map",                    Fmap		},

	{ "string?",                Fifstring		},
	{ "make-string",            Fmakestring		},
	{ "string",                 Fstring		},
	{ "string-length",          Fstringlength	},
#if 0
string-ref
string-set!

string=?
string-ci=?
string<=?
string<?
string>=?
string>?
string-ci<=?
string-ci<?
string-ci>=?
string-ci>?

substring
string-append
string->list
list->string
string-copy
string-fill!

number->string
string->number
string->symbol
symbol->string

vector?
make-vector
vector
vector-length
vector-ref
vector-set!
vector->list
list->vector
vector-fill!

char->integer
char-alphabetic?
char-ci<=?
char-ci<?
char-ci=?
char-ci>=?
char-ci>?
char-downcase
char-lower-case?
char-numeric?
char-ready
char-ready?
char-upcase
char-upper-case?
char-whitespace?
char<=?
char<?
char=?
char>=?
char>?
char?
integer->char
#endif
	{ 0, 0 },
};
