/*
 *      Файл low.c - машинно-зависимая часть ГРАФОРа,
 *      связанная с графикой.
 *
 *      Функции:
 *
 *      subroutine begin (nx, ny)
 *      integer nx
 *      integer ny
 *
 *              - включение графического режима. Максимальные значения
 *              графических координат, которые предполагается использовать,
 *              заданы в nx и ny. Перо устанавливается в точку 0, 0.
 *
 *      subroutine end
 *
 *              - выключение графического режима.
 *
 *      subroutine plot (nx, ny, k)
 *      integer nx
 *      integer ny
 *      integer k
 *
 *              - перемещение пера в точку nx, ny. Если k==2, перемещение
 *              происходит с опущеным пером (отрисовка линии), иначе - с
 *              поднятым (изменение позиции).
 *
 *      subroutine setpen (n)
 *      integer n
 *
 *              - установка цвета пера с номером n. По умолчанию - 1.
 *
 *      subroutine setmtf (name, len)
 *      character name (len)
 *      integer len
 *
 *              - задание имена метафайла для вывода графика. Длина len имени
 *              name не должна превышать 14 символов.
 *
 *      subroutine setscr
 *
 *              - задание вывода графика на дисплей.
 *
 *      subroutine commnt (s, len)
 *      character s (len)
 *      integer len
 *
 *              - запись комментария s длиной len в метафайл.
 *
 *      subroutinе paint (nx, ny, n)
 *      integer nx
 *      integer ny
 *      integer n
 *
 *              - заливка обасти от точки nx, ny до границы цветом n.
 */

/*
 *      В настоящий момент поддерживаются два соглашения
 *      о связях между Си и фортраном:
 *
 *      1.      Для вызова фортран-функции из Си к имени нужно добавить
 *              подчерк. Порядок параметров - тот же. Параметры
 *              передаются по ссылке. Типы integer и real в фортране
 *              соответствует long и float в Си.
 *              Используется по умолчанию.
 *
 *      2.      Для вызова фортран-функции из Си ее нужно описать
 *              ключевым словом fortran. Порядок параметров - тот же.
 *              Параметры передаются по ссылке. Типы integer и real в
 *              фортране соответствует long и float в Си.
 *              Включается флагом -DMSF.
 */

# ifdef MSF
#       define  BEGIN   fortran Begin
#       define  END     fortran End
#       define  SETPEN  fortran Setpen
#       define  PLOT    fortran Plot
#       define  SETSCR  fortran Setscr
#       define  SETMTF  fortran Setmtf
#       define  PAINT   fortran Painta
#       define  COMMNT  fortran Commnt
#       define  GRINIT  Grinit
# else
#       define  BEGIN   begin_
#       define  END     end_
#       define  SETPEN  setpen_
#       define  PLOT    plot_
#       define  SETSCR  setscr_
#       define  SETMTF  setmtf_
#       define  PAINT   painta_
#       define  COMMNT  commnt_
#       define  GRINIT  grinit_
# endif

# ifdef MSF
extern fortran GRINIT ();
# endif

# define INTEGER long

static graflag;                         /* if graph regime is on */
static scrflag = 1;                     /* use screen (1) or metafile (0) */

static grinit () { GRINIT (); }         /* dummy call - load block data */

BEGIN (x, y)
INTEGER *x, *y;
{
	/* check if grafor is already opened */
	if (graflag)
		return;

	/* try to initialize graph library */
	if (! GOpen (scrflag, (int) *x, (int) *y))
		return;

	/* grafor is opened */
	graflag = 1;
}

END ()
{
	if (! graflag)
		return;
	GClose ();
	graflag = 0;
}

PLOT (x, y, k)
INTEGER *x, *y, *k;
{
	if (! graflag)
		return;
	if (*k == 2)
		GPlot ((int) *x, (int) *y);
	else
		GMove ((int) *x, (int) *y);
}

SETPEN (n)
INTEGER *n;
{
	GColor ((int) *n);
}

static strcopy (dest, dsz, src, ssz)
register char *dest, *src;
{
	register i;

	i = ssz < dsz ? ssz : dsz-1;
	while (--i >= 0)
		*dest++ = *src++;
	*dest = 0;
}

SETMTF (name, len)
char *name;
INTEGER *len;
{
	char buf [40];

	if (graflag)
		return;
	strcopy (buf, sizeof (buf), name, (int) *len);
	if (GFile (buf))
		scrflag = 0;
	else
		scrflag = 1;
}

SETSCR ()
{
	if (graflag)
		return;
	scrflag = 1;
}

COMMNT (s, len)
char *s;
INTEGER *len;
{
	char buf [100];

	if (! graflag)
		return;
	strcopy (buf, sizeof (buf), s, (int) *len);
	GComment (buf);
}

PAINT (n, x, y)
INTEGER *n, *x, *y;
{
	if (! graflag)
		return;
	GPaint ((int) *n, (int) *x, (int) *y);
}
