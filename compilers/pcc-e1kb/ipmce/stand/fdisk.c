# include "vollab.h"
# include "svsb.h"

# define K              1024
# define W              ((int) sizeof (int))
# define VLABELSZ       9               /* размер метки тома в блоках */

char line [80];
char unitname [] = "hd(0)";
int buf [9*K/W];
int unit;
struct vl_fixed *lbl;
struct pt_entry *pt;
struct btt_entry *btt;

yes ()
{
	char rep [80];

	printf ("? (y, n, д, н) [н] ");
	gets (rep);
	return (rep[0]=='y' || rep[0]=='Y' || rep[0]=='д' || rep[0]=='Д');
}

main ()
{
	printf (MSG ("\nFDISK %s    %s\n", "\nРЕДАКТОР РАЗДЕЛОВ %s    %s\n"),
		BOOTVERSION, MSG (LCOPYRIGHT, RCOPYRIGHT));
	printf (MSG ("\nUsing this program you may overwrite information on your hard disk.\n",
		"\nПри работе с этой программой Вы можете затереть информацию на диске.\n"));
	printf (MSG ("Do you want to continue",
		"Продолжать"));
	if (! yes ())
		return;
	getunit ();
	prlabel ();
	for (;;) switch (getmenu ()) {
	case 1:                 /* Выдать список разделов */
		prcatalog ();
		continue;
	case 2:                 /* Выдать список свободных областей */
		prfreeareas ();
		continue;
	case 3:                 /* Выдать список перекрывающихся разделов */
		proverlap ();
		continue;
	case 4:                 /* Выдать таблицу дорожек замены */
		prbadtracks ();
		continue;
	case 5:                 /* Выдать метку тома */
		prlabel ();
		continue;
	case 6:                 /* Создать раздел */
		if (createpart ())
			putlabel ();
		continue;
	case 7:                 /* Удалить раздел */
		if (deletepart ())
			putlabel ();
		continue;
	case 8:                 /* Переименовать раздел */
		if (renamepart ())
			putlabel ();
		continue;
	case 9:                 /* Перейти на другое устройство */
		getunit ();
		prlabel ();
		continue;
	default:
		return;
	}
}

getunit ()
{
	do {
		printf (MSG ("\nEnter unit number: (0-7) [0] ",
			"\nВведите номер устройства: (0-7) [0] "));
		gets (line);
		unit = atoi (line) & 7;
		unitname [3] = '0' + unit;
	} while (! getlabel ());
}

getmenu ()
{
	int n;
loop:
	printf (MSG ("\n\t1. Display Partition Table\n",
		"\n\t1. Выдать список разделов\n"));
	printf (MSG ("\t2. Display Free Areas\n",
		"\t2. Выдать список свободных областей\n"));
	printf (MSG ("\t3. Display Overlap Partitions\n",
		"\t3. Выдать список перекрывающихся разделов\n"));
	printf (MSG ("\t4. Display Bad Track Table\n",
		"\t4. Выдать таблицу дорожек замены\n"));
	printf (MSG ("\t5. Display Volume Label\n",
		"\t5. Выдать метку тома\n"));
	printf (MSG ("\t6. Create Partition\n",
		"\t6. Создать раздел\n"));
	printf (MSG ("\t7. Delete Partition\n",
		"\t7. Удалить раздел\n"));
	printf (MSG ("\t8. Rename Partition\n",
		"\t8. Переименовать раздел\n"));
	printf (MSG ("\t9. Change Unit Number\n",
		"\t9. Перейти на другое устройство\n"));
	printf (MSG ("\t0. Quit\n", "\t0. Выход\n"));
	printf (MSG ("\nEnter your choice: ",
		"\nВведите номер команды: "));
	gets (line);
	if (! strcmp (line, "0"))
		return (0);
	n = atoi (line);
	if (n<1 || n>9)
		goto loop;
	return (n);
}

getlabel ()
{
	int i, n;

	i = open (unitname, 0);
	if (i < 0)
		return (0);
	if (! rd (i, buf, K)) {
error:          close (i);
		return (0);
	}
	if (buf [0] != VL_MAGIC) {
		printf (MSG ("bad volume magic %w\n", "неверный ключ %w\n"), buf[0]);
		goto error;
	}
	for (n=1; n<VLABELSZ; ++n)
		if (! rd (i, buf+K/W*n, K))
			goto error;
	close (i);
	lbl = (struct vl_fixed *) buf;
	btt = (struct btt_entry *) (buf + 6);
	pt = (struct pt_entry *) (buf + lbl->vl_ptntab);
	sortparts ();
	return (1);
}

rd (i, buf, sz)
int *buf;
{
	if (read (i, (char *) buf, sz) != sz) {
		printf (MSG ("bad block size on read\n",
			"плохой размер блока при чтении\n"));
		return (0);
	}
	return (1);
}

prlabel ()
{
	register struct vl_fixed *l;
	char volname [9], *n;

	l = lbl;
	n = volname;
	*n++ = l->vl_name >> 56;
	*n++ = l->vl_name >> 48;
	*n++ = l->vl_name >> 40;
	*n++ = l->vl_name >> 32;
	*n++ = l->vl_name >> 24;
	*n++ = l->vl_name >> 16;
	*n++ = l->vl_name >> 8;
	*n++ = l->vl_name;
	*n = 0;
	printf (MSG ("\nVolume '%s' model %d block length %d label length %d\n",
		"\nТом '%s' модель %d длина блока %d длина метки %d\n"),
		volname, l->vl_model, l->vl_blksize, l->vl_vlsize);
	printf (MSG ("Formatted %xh on %d label %d.%d label copy %d\n",
		"Размечен %xh на %d метка %d.%d копия метки %d\n"),
		l->vl_fmttime, l->vl_systemid, l->vl_version,
		l->vl_revision, l->vl_vlcopy);
	printf (MSG ("Blocks %d reserved tracks %d beginning from %d\n",
		"Блоков %d запасных дорожек %d начиная с %d\n"),
		l->vl_size, l->vl_nreserved, l->vl_restrack);
	printf (MSG ("Reserved partitions %d catalog from %d\n",
		"Максимальное число разделов %d каталог с %d\n"),
		l->vl_maxptn, l->vl_ptntab);
}

prcatalog ()
{
	register struct pt_entry *p;
	register struct vl_fixed *l;
	int n;

	l = lbl;
	p = pt;
	printf (MSG ("\nNumber\tStart\tLength\tVersion\tName\n",
		"\nНомер\tНачало\tДлина\tВерсия\tИмя\n"));
	for (n=0; p->pt_name1; ++n) {
		printf ("%d)\t", n);
		prpart (p++);
	}
}

proverlap ()
{
	register struct pt_entry *p, *s;
	int n;

	s = pt+1;
	if (! s->pt_name1)
		return;
	p = s+1;
	printf (MSG ("\nNumber\tStart\tLength\tVersion\tName\n",
		"\nНомер\tНачало\tДлина\tВерсия\tИмя\n"));
	n = 1;
	for (++p; p->pt_name1; ++s, ++p, ++n)
		if (s->pt_offset+s->pt_size > p->pt_offset) {
			printf ("\n%d)\t", n);
			prpart (s);
			printf ("%d)\t", n+1);
			prpart (p);
		}
}

prpart (p)
register struct pt_entry *p;
{
	char partname [13], *n;

	n = partname;
	*n++ = p->pt_name1 >> 56;
	*n++ = p->pt_name1 >> 48;
	*n++ = p->pt_name1 >> 40;
	*n++ = p->pt_name1 >> 32;
	*n++ = p->pt_name1 >> 24;
	*n++ = p->pt_name1 >> 16;
	*n++ = p->pt_name1 >> 8;
	*n++ = p->pt_name1;
	*n++ = p->pt_name2 >> 24;
	*n++ = p->pt_name2 >> 16;
	*n++ = p->pt_name2 >> 8;
	*n++ = p->pt_name2;
	*n = 0;
	printf ("%x\t%x\t<%d>\t", p->pt_offset, p->pt_size,
		p->pt_version);
	prname (partname);
	putchar ('\n');
}

prbadtracks ()
{
	register struct btt_entry *b;
	int n;

	b = btt;
	printf (MSG ("\nNumber\tBad\tExtra\n", "\nНомер\tПлохая\tЗамена\n"));
	for (n=0; *(int*)b; ++n, ++b)
		printf ("%d)\t%d.%d\t%d.%d\n", n, b->btt_badcyl,
			b->btt_badhead, b->btt_newcyl, b->btt_newhead);
}

prfreeareas ()
{
	register struct pt_entry *p, *s;
	register struct vl_fixed *l;
	int n;

	l = lbl;
	p = pt+1;
	printf (MSG ("\nNumber\tStart\tLength\n",
		"\nНомер\tНачало\tДлина\n"));
	n = 0;
	if (! p->pt_name1) {
		prfree (n, l->vl_vlsize, l->vl_vlcopy);
		return;
	}
	for (s=p++; p->pt_name1; ++s, ++p)
		if (prfree (n, s->pt_offset+s->pt_size, p->pt_offset))
			++n;
	prfree (n, s->pt_offset+s->pt_size, l->vl_vlcopy);
}

prfree (n, b, e)
{
	if (b < e) {
		printf ("%d)\t%x\t%x\n", n, b, e-b);
		return (1);
	}
	return (0);
}

prname (s)
char *s;
{
	int c, bos;

	bos = 1;
	for (;;) {
		if (c = *s++)
			bos = 0;
		else if (! bos)
			return;
		if (c>=' ' && c<='~' || c>=0300 && c<=0376) {
			putchar (c);
			continue;
		}
		putchar ('\\');
		putchar ('0' + (c>>6&3));
		putchar ('0' + (c>>3&7));
		putchar ('0' + (c&7));
	}
}

struct pt_entry *find (s)
char *s;
{
	register struct pt_entry *p;
	register struct vl_fixed *l;
	struct pt_entry n;

	l = lbl;
	p = pt;
	storename (s, (int *) &n);
	for (; p->pt_name1; ++p)
		if (p->pt_name1==n.pt_name1 && p->pt_name2==n.pt_name2)
			return (p);
	return (0);
}

delete (p)
register struct pt_entry *p;
{
	for (; p->pt_name1; ++p)
		p[0] = p[1];
}

struct pt_entry *create ()
{
	register struct pt_entry *p;

	for (p=pt+1; p->pt_name1; ++p);
	return (p);
}

createpart ()
{
	char name [80], buf [80];
	register struct pt_entry *p;
	int off, size, ver;

	/* ввести новое имя и проверить, что оно не существует */
	for (;;) {
		printf (MSG ("\nName: ", "\nИмя: "));
		gets (name);
		if (! name [0])
			return (0);
		decode (name);
		if (! find (name))
			break;
		printf (MSG ("Such partition already exists\n",
			"Такой раздел уже существует\n"));
	}
	/* ввести начало */
	printf (MSG ("\nBegin: ", "\nНачало: "));
	gets (buf);
	if (! buf [0])
		return (0);
	off = atox (buf);

	/* ввести длину */
	printf (MSG ("\nLength: ", "\nДлина: "));
	gets (buf);
	if (! buf [0])
		return (0);
	size = atox (buf);

	/* ввести версию */
	printf (MSG ("\nVersion: [1] ", "\nВерсия: [1] "));
	gets (buf);
	ver = buf[0] ? atoi (buf) : 1;

	printf (MSG ("\nCreate '", "\nСоздать '"));
	prname (name);
	printf (MSG ("', begin %x, length %x, version %d",
		"', начало %x, длина %x, версия %d"),
		off, size, ver);
	if (! yes ())
		return (0);
	if (! (p = create ())) {
		printf (MSG ("Partition table full\n",
			"Нет места в таблице разделов\n"));
		return (0);
	}
	storename (name, (int *) p);
	p->pt_offset = off;
	p->pt_size = size;
	p->pt_version = ver;
	printf (MSG ("\nPartition '", "\nРаздел '"));
	prname (name);
	printf (MSG ("' created.\n", "' создан.\n"));
	return (1);
}

deletepart ()
{
	char name [80];
	register struct pt_entry *p;

	/* ввести имя и проверить, что оно существует и не нулевой раздел */
	for (;;) {
		printf (MSG ("\nName: ", "\nИмя: "));
		gets (name);
		if (! name [0])
			return (0);
		decode (name);
		if ((p = find (name)) && p != pt)
			break;
		if (p == pt)
			printf (MSG ("You cannot delete zero partition\n",
				"Нельзя удалять нулевой раздел\n"));
		else
			printf (MSG ("No such partition\n",
				"Нет такого раздела\n"));
	}
	printf (MSG ("\nDelete '", "\nУдалить '"));
	prname (name);
	putchar ('\'');
	if (! yes ())
		return (0);

	delete (p);
	printf (MSG ("\nPartition '", "\nРаздел '"));
	prname (name);
	printf (MSG ("' deleted.\n", "' удален.\n"));
	return (1);
}

renamepart ()
{
	char oldname [80], newname [80];
	register struct pt_entry *p;

	/* ввести старое имя и проверить, что оно существует */
	for (;;) {
		printf (MSG ("\nOld name: ", "\nСтарое имя: "));
		gets (oldname);
		if (! oldname [0])
			return (0);
		decode (oldname);
		if (p = find (oldname))
			break;
		printf (MSG ("No such partition\n", "Нет такого раздела\n"));
	}
	/* ввести новое имя и проверить, что оно не существует */
	for (;;) {
		printf (MSG ("New name: ", "Новое имя: "));
		gets (newname);
		if (! newname [0])
			return (0);
		decode (newname);
		if (! find (newname))
			break;
		printf (MSG ("Such partition already exists\n",
			"Такой раздел уже существует\n"));
	}
	printf (MSG ("\nRename '", "\nПереименовать '"));
	prname (oldname);
	printf (MSG ("' to '", "' в '"));
	prname (newname);
	putchar ('\'');
	if (! yes ())
		return (0);

	storename (newname, (int *) p);
	printf (MSG ("\nPartition '", "\nРаздел '"));
	prname (oldname);
	printf (MSG ("' renamed to '", "' переименован в '"));
	prname (newname);
	printf ("'\n");
	return (1);
}

storename (s, p)
char *s;
int *p;
{
	int n, b;

	p[0] = 0;
	p[1] = p[1] << 32 >> 32;
	b = 1;
	for (n=0; n<8 && (b || *s); ++s, ++n) {
		if (*s)
			b = 0;
		p[0] |= *s << (56-8*n);
	}
	for (n=0; n<4 && *s; ++s, ++n)
		p[1] |= *s << (56-8*n);
}

putlabel ()
{
	int i, n;

	i = open (unitname, 1);
	if (i < 0)
		return (0);
	sortparts ();
	for (n=0; n<VLABELSZ; ++n)
		write (i, buf+K/W*n, K);
	close (i);
	printf (MSG ("Partition table written.\n",
		"Каталог разделов записан на диск.\n"));
	return (1);
}

comppart (a, b)
register struct pt_entry *a, *b;
{
	if (a->pt_offset < b->pt_offset)
		return (-1);
	if (a->pt_offset > b->pt_offset)
		return (1);
	if (a->pt_size < b->pt_size)
		return (-1);
	if (a->pt_size > b->pt_size)
		return (1);
	return (0);
}

sortparts ()
{
	register struct pt_entry *p;
	int n;

	/* compute number of partitions */
	for (p=pt+1; p->pt_name1; ++p);
	if (n = (p-pt) - 1)
		qsort ((char *) (pt+1), n, sizeof (*p), comppart);
}

atox (s)
char *s;
{
	int n, c, d;

	for (n=0; c= *s; ++s) {
		if ((d = hexdig (c)) < 0)
			break;
		n = n<<4 | d;
	}
	return (n);
}

hexdig (c)
{
	switch (c) {
	case '0': case 'o': case 'O': case 'о': case 'О': return (0);
	case '1':       return (1);
	case '2':       return (2);
	case '3':       return (3);
	case '4':       return (4);
	case '5':       return (5);
	case '6':       return (6);
	case '7':       return (7);
	case '8':       return (8);
	case '9':       return (9);
	case 'a': case 'A': case 'а': case 'А': return (10);
	case 'b': case 'B': case 'б': case 'Б': case 'в': case 'В': return (11);
	case 'c': case 'C': case 'ц': case 'Ц': case 'с': case 'С': return (12);
	case 'd': case 'D': case 'д': case 'Д': return (13);
	case 'e': case 'E': case 'е': case 'Е': return (14);
	case 'f': case 'F': case 'ф': case 'Ф': return (15);
	}
	return (-1);
}
