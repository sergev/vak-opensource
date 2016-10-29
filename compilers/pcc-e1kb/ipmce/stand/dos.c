/*
 * Обмен с файловой системой DOS.
 * ------------------------------
 *
 *      Адрес 22h - заявка на обмен.
 *      Адрес 23h - ответ.
 *
 * Логика обмена со стороны PC.
 *
 *      1) Периодически опрашивать ячейку 22h, пока
 *      не появится заявка.
 *
 *      2) Выполнить заявку.
 *
 *      3) Записать ответ в ячейку 23h.
 *
 *      4) Обнулить заявку в ячейке 22h.
 *
 * Логика обмена со стороны Э-Б.
 *
 *      1) Установить заявку в ячейку 22h.
 *
 *      2) Ждать 0 в ячейке 22h.
 *
 *      3) Прочитать ответ в ячейке 23h.
 *
 * Формат заявки.
 *
 * -------------------------------------------------
 * |        адрес         |   длина   |  код |  оп |
 * -------------------------------------------------
 *  64                  33 32       17 16   9 8   1
 *
 * Формат ответа.
 *
 * -------------------------------------------------
 * |     длина файла      | длина обм |  тип |  ош |
 * -------------------------------------------------
 *  64                  33 32       17 16   9 8   1
 *
 * Набор операций.
 *
 *      1) открытие файла
 *
 *              char    op;             DOPEN
 *              char    mode;           r=0, w=1, r/w=2
 *              short   length;         длина имени
 *              long    addr;           адрес имени
 *
 *         возврат:
 *
 *              char    err;            1 если файл не открыт
 *              char    fd;             дескриптор
 *
 *      2) чтение/запись
 *
 *              char    op;             DREAD/DWRITE
 *              char    fd;             дескриптор файла
 *              short   length;         длина блока обмена
 *              long    addr;           адрес буфера
 *
 *         возврат:
 *
 *              char    err;            1 если ошибка обмена
 *              short   length;         длина обмена
 *
 *      3) закрытие файла
 *
 *              char    op;             DCLOSE
 *              char    fd;             дескриптор файла
 *
 *         возврат:
 *
 *              char    err;            1 если файл не закравается
 *
 *      4) перемещение (lseek)
 *
 *              char    op;             DSEEK
 *              char    fd;             дескриптор файла
 *              short   pos;            0=beg, 1=end, 2=place
 *              long    offset;
 *
 *         возврат:
 *
 *              char    err;            1 если ошибка
 *
 *      5) открытие каталога
 *
 *              char    op;             DOPENDIR
 *              short   length;         длина имени
 *              long    addr;           адрес имени
 *
 *         возврат:
 *
 *              char    err;            1 если файл не открыт
 *              char    fd;             дескриптор каталога
 *
 *      6) чтение каталога
 *
 *              char    op;             DREADDIR
 *              char    fd;             дескриптор каталога
 *              long    addr;           4 слова для имени
 *
 *         возврат:
 *
 *              char    err;            1 если ошибка обмена
 *              char    neof;           0 если конец каталога
 *
 *      7) запрос информации о файле
 *
 *              char    op;             DSTAT
 *              char    fd;             дескриптор каталога
 *              short   length;         длина имени
 *              long    addr;           адрес имени
 *
 *         возврат:
 *
 *              char    err;            1 если ошибка
 *              char    type;           тип 0-файл, 1-каталог, >1-спец.
 *              long    length;         длина файла
 */

# include "dosio.h"
# include "svsb.h"

/* # define DEBUG */

dosopen (name, mode)
char *name;
{
	struct dosio io;
	struct dosreply reply;
	char nambuf [80];

	strcpy (nambuf, name);
	io.op = DOPEN;
	io.fd = mode==2 ? 2 : mode==1 ? 1 : 0;
	io.length = strlen (nambuf);
	io.addr = (int) nambuf;
# ifdef DEBUG
	printf ("***** dosopen '%s' mode 0x%x\n", nambuf, mode);
# endif /* DEBUG */
	doswait (&io, &reply);
	return (reply.err ? -1 : reply.type);
}

dosclose (fd)
{
	struct dosio io;
	struct dosreply reply;

	io.op = DCLOSE;
	io.fd = fd;
	io.length = 0;
	io.addr = 0;
	doswait (&io, &reply);
	return (reply.err ? -1 : 0);
}

dosread (fd, buf, len)
char *buf;
{
	struct dosio io;
	struct dosreply reply;

	io.op = DREAD;
	io.fd = fd;
	io.length = len;
	io.addr = (int) buf;
	doswait (&io, &reply);
	return (reply.err ? -1 : reply.length);
}

doswrite (fd, buf, len)
char *buf;
{
	struct dosio io;
	struct dosreply reply;

	io.op = DWRITE;
	io.fd = fd;
	io.length = len;
	io.addr = (int) buf;
	doswait (&io, &reply);
	return (reply.err ? -1 : reply.length);
}

doslseek (fd, offset, pos)
{
	struct dosio io;
	struct dosreply reply;

	io.op = DSEEK;
	io.fd = fd;
	io.length = pos;
	io.addr = offset;
	doswait (&io, &reply);
	return (reply.err ? -1 : 0);
}

dosopendir (name)
char *name;
{
	struct dosio io;
	struct dosreply reply;
	char nambuf [80];

	strcpy (nambuf, name);
	io.op = DOPENDIR;
	io.fd = 0;
	io.length = strlen (nambuf);
	io.addr = (int) nambuf;
	doswait (&io, &reply);
	return (reply.err ? -1 : reply.type);
}

dosreaddir (fd, buf)
char *buf;
{
	struct dosio io;
	struct dosreply reply;

	io.op = DREADDIR;
	io.fd = fd;
	io.length = 32;
	io.addr = (int) buf;
	doswait (&io, &reply);
	return (reply.err ? -1 : reply.type);
}

dosstat (fd, name, ptype, plen)
char *name;
int *ptype, *plen;
{
	struct dosio io;
	struct dosreply reply;

	io.op = DSTAT;
	io.fd = fd;
	io.length = strlen (name);
	io.addr = (int) name;
	doswait (&io, &reply);
	*ptype = reply.type;
	*plen = reply.addr;
	return (reply.err ? -1 : 0);
}

static doswait (o, r)
register struct dosio *o;
register struct dosreply *r;
{
	int u;

	u = o->op & 0xff | (o->fd & 0xff) << 8 |
		(o->length & 0xffff) << 16 | o->addr << 32;

# ifdef DEBUG
	printf ("***** dosio %w\n", u);
# endif /* DEBUG */

	_dosout = u;
	_flush_ ();                     /* выталкивание БРЗ */
	while (_dosout)
		idle (1);
# ifdef DEBUG
	printf ("***** reply %w\n", _dosin);
# endif /* DEBUG */

	r->err = _dosin & 0xff;
	r->type = _dosin >> 8 & 0xff;
	r->length = _dosin >> 16 & 0xffff;
	r->addr = _dosin >> 32 & 0x7fffffff;
}
