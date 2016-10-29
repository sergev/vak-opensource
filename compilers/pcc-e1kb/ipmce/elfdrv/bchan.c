/*
 * Канальный драйвер БК интерфейса ЭЛЬФ.
 * Используется драйверами устройств более высокого уровня.
 *
 * По каналу ходят пакеты, состоящие из заголовка и тела (данных).
 * Длина пакета всегда кратна 8 байтам (слову).
 * Заголовок имеет длину 1 слово.
 * В заголовке имеется поле, содержащее длину сопутствующих данных
 * в словах.
 *
 * Данные могут отрываться от заголовка и идти отдельным пакетом.
 * Если пакет содержит данные, то его длина должна быть равна длине,
 * указанной в заголовке, плюс 1 слово (длина заголовка).
 * Если пакет не содержит данных и имеет ненулевое поле длины,
 * то данные должны следовать за ним отдельным пакетом (без заголовка),
 * и иметь длину, указанную в предшествующем заголовке.
 *
 * Канал делится на 256 логических подканалов. В каждом заголовке
 * указывается номер подканала, к которому относится пакет.
 * Подканал с номером 0 зарезервирован и применяется
 * драйвером для управления каналом.
 *
 * Заголовок пакета имеет следующую структуру:
 *
 *    0     1     2     3     4     5     6     7 - порядковый номер
 * -------------------------------------------------  байта в канале
 * | нпк | ком | дл.данных |  сопутств.информация  |
 * -------------------------------------------------
 *
 * нпк          - номер подканала
 * ком          - код команды
 * дл.данных    - длина данных в словах (8 байт)
 * сопутств.инф.- сопутствующая информация, передаваемая в подканал
 *
 * Примечание: Эльбрус-Б всегда пихает данные в канал, начиная
 * с самого левого (старшего) байта слова.
 */

/*$Header:$*/

/*$Log:$*/

/*
 * bcinit ()
 *              - Инициализация канала: захват, пуск приемной части.
 *
 * bcsetup (subchan, psintr, printr)
 * int subchan;         - Номер подканала, 1..NBCHAN
 * int (*psintr) ();    - Вызов при окончании передачи.
 * int (*printr) ();    - Вызов при приеме.
 *
 *              - Установка подканала.
 *
 * bcsend (subchan, op, info, len, data)
 * int subchan;         - Номер подканала, 1..NBCHAN
 * int op;              - Код операции.
 * int info;            - Сопутствующая информация.
 * int len;             - Длина данных в словах или 0.
 * int *data;           - Адрес данных или 0.
 *
 *              - Постановка заявки в очередь на передачу.
 *              Если канал не активен, запустить bcstart ().
 *
 * bcrintr ()
 *              - Обработка прерывания по приему.
 *              Проанализировать заголовок. Если нужно,
 *              подождать данные. Если подканал не инициализирован,
 *              игнорировать пришедшую информацию. Иначе вызвать
 *              (*printr) (subchan, op, info, len, data).
 *              Перезапустить прием.
 *
 * bcsintr ()
 *              - Обработка прерывания по концу передачи.
 *              Убрать заявку из очереди. Вызвать (*psintr) (subchan).
 *              Запустить bcstart ().
 *
 * bcstart ()
 *              - Пуск передающей части. Если есть заявки в очереди
 *              на передачу, пустить канал.
 */

#define BCHANNUM        1       /* Номер используемого БК */
#define NBCHAN          8       /* Максимум 8 подканалов на канал */

typedef int (*funcptr) ();

struct chanhead {               /* Заголовок пакета в канале */
	unsigned subchan :8;    /* Номер подканала */
	unsigned op      :8;    /* Код операции */
	unsigned len     :16;   /* Длина данных */
	unsigned info    :32;   /* Сопутствующая информация */
};

struct chanpacket {             /* Пакет в канале */
	struct chanhead header; /* Заголовок */
	char data [BSIZE];      /* Данные */
};

struct subchantab {             /* Структура данных подканала */
	int subchan;            /* Номер подканала */
	int opened;             /* Подканал инициализирован */
	int active;             /* Передающая часть активна */
	funcptr rintr;          /* Вызов при приеме пакета */
	funcptr sintr;          /* Вызов по концу передачи */
	struct chanhead ohd;    /* Заявка на выдачу */
	int olen;               /* Длина данных для выдачи */
	int *odata;             /* Адрес данных для выдачи */
	int odataflag;          /* Будем выдавать данные отдельным пакетом */
	struct subchantab *next; /* Следующий подканал в очереди */
};

struct chantab {                                /* Структура данных канала */
	int chan;                               /* Номер канала */
	int opened;                             /* Канал захвачен */
	int active;                             /* Передающая часть активна */
	struct subchantab *first;               /* Начало очереди заявок */
	struct subchantab *last;                /* Конец очереди */
	struct subchantab subchan [NBCHAN+1];   /* Подканалы */
	struct chanpacket rbuf;                 /* Буфер для приема пакетов */
	int idataflag;                          /* Ждем данные отд. пакетом */
};

struct chantab bchan;           /* Наш канал */

bcinit ()
{
	register struct chantab *cp = chantab;

	cp->chan = BCHANNUM;
	cp->first = 0;
	cp->last = 0;
	cp->active = 0;
	cp->idataflag = 0;
	cp->opened = 0;

	/* Захват канала */

	rez = dvs_ex4 (0xed1, cp->chan*2, cp->chan*2 + 1, 0, 0);
	if (rez) {
		if (rez == 1)
			printf ("Нет БК%d-источника\n", cp->chan);
		else if (rez == 2)
			printf ("БК%d-источник занят\n", cp->chan);
		return;
	}
	rez = dvs_ex4 (0xed1, cp->chan*2 + 1, cp->chan*2 + 2, 0, 0);
	if (rez) {
		if (rez == 1)
			printf ("Нет БК%d-приемника\n", cp->chan);
		else if (rez == 2)
			printf ("БК%d-приемник занят\n", cp->chan);
		dvs_ex1 (0xed2, cp->chan*2);
		return;
	}

	cp->opened = 1;

	/* Пуск приемной части. */
	bcrstart (cp);
}

bcsetup (subchan, psintr, printr)
funcptr psintr;
funcptr printr;
{
	register struct chantab *cp = chantab;
	register struct subchantab *scp;

	if (! cp->opened)
		bcinit ();
	if (subchan<1 || subchan>NBCHAN) {
		printf ("Неверный номер подканала: %d!\n", subchan);
		return;
	}
	scp = cp->subchan [subchan];
	scp->subchan = subchan;
	scp->rintr = printr;
	scp->sintr = psintr;
	scp->active = 0;
	if (psintr)
		scp->opened = 1;        /* Канал открыт */
	else
		scp->opened = 0;        /* Канал сброшен */
}

bcsend (subchan, op, info, len, data)
int *data;
{
	register struct chantab *cp = chantab;
	register struct subchantab *scp;

	if (subchan<1 || subchan>NBCHAN) {
		printf ("Неверный номер подканала: %d!\n", subchan);
		return;
	}
	scp = cp->subchan [subchan];
	if (! scp->opened) {
		printf ("Подканал %d не открыт\n", subchan);
		return;
	}
	if (scp->active) {
		printf ("Подканал %d уже запущен!\n", subchan);
		return;
	}

	/* Формирование заявки. */

	scp->ohd.subchan = subchan;
	scp->ohd.op = op;
	scp->ohd.len = len;
	scp->ohd.info = info;

	scp->olen = len;
	scp->odata = data;
	scp->active = 1;
	scp->odataflag = 0;

	/* Постановка заявки в очередь на передачу. */

	scp->next = 0;
	if (cp->first) {
		cp->last->next = scp;
		cp->last = scp;
	} else {
		cp->first = scp;
		cp->last = scp;
	}

	/* Если канал не активен, запустить bcstart (). */

	if (! cp->active)
		bcstart (cp);
}

bcrintr (chan)
{
	register struct chantab *cp = chantab;
	register struct subchantab *scp;
	register struct chanhead *h;
	int subchan;
	int plen, rez;

	/* Обработка прерывания по приему. */
	if (chan != cp->chan)
		return;

	/* Анализ байта состояния */
	rez = get_reg (BC_DEM, cp->chan*2 + 1) & 0x7f;
	if (rez) {
		printf ("Ошибка БК%d-приемника: 0x%x\n", cp->chan, rez);
		goto out;
	}

	h = &cp->rbuf.header;
	subchan = h->subchan;
	if (subchan<1 || subchan>NBCHAN) {
		printf ("Неверный номер подканала в принятом пакете: %d\n",
			subchan);
		goto out;
	}
	scp = &cp->subchan [subchan];
	if (! scp->opened) {
		printf ("Подканал %d не открыт на прием\n", subchan);
		goto out;
	}

	/* Вычисляем длину принятого пакета в словах, >0 */
	plen = get_reg (BC_DEM, cp->chan*2 + 1) >> 24 & 0xffff;
	plen = (plen + 7) / 8;

	if (cp->idataflag) {
		/* Пришли данные к предыдущему заголовку. */
		/* Приклеиваем данные к заголовку. */
		plen += 1;
		cp->idataflag = 0;
	}
	if (plen == 1) {
		/* Пришел заголовок. */
		if (h->len) {
			/* Будем ждать прихода данных. */
			cp->idataflag = 1;
			goto out;
		}
		(*scp->rintr) (subchan, h->op, h->info, 0, 0);
	} else {
		/* Пришел заголовок с данными. */
		if (h->len != plen-1)
			printf ("Несоответствие длин заголовка (%d) и пакета (%d)\n",
				h->len, plen);
		(*scp->rintr) (subchan, h->op, h->info,
			(plen - 1) * 8, h->data);
	}
out:
	/* Перезапустить прием. */
	bcrstart (cp);
}

bcsintr (chan)
{
	register struct chantab *cp = chantab;
	register struct subchantab *scp = cp->first;

	/* Обработка прерывания по концу передачи. */
	if (chan != cp->chan)
		return;

	/* Анализ байта состояния */
	rez = get_reg (BC_DEM, cp->chan*2) & 0xff;
	if (rez) {
		printf ("Ошибка БК%d-источника (0x%x), повтор\n",
			cp->chan, rez);
		bcstart (cp);
		return;
	}

	if (! scp)
		return;
	cp->active = 0;

	if (scp->odataflag || ! scp->olen) {
		/* Передали данные. Убрать заявку из очереди. */

		cp->first = scp->next;
		scp->active = 0;
		if (scp->sintr)
			(*scp->sintr) (scp->subchan);
		bcstart (cp);
	} else {
		/* Заголовок уже передали. Будем передавать данные. */

		cp->active = 1;
		scp->odataflag = 1;
		while (dvs_ex4 (0xad9, cp->chan * 2, 0, olen, scp->odata)
			printf ("Ошибка при запуске источника БК %d\n",
				cp->chan);
	}
}

bcstart (cp)
register struct chantab *cp;
{
	register struct subchantab *scp;

	/* Берем блок из очереди и обрабатываем его */
	scp = cp->first;
	while (scp && ! scp->active) {
		printf ("Неактивная заявка в очереди\n");
		cp->first = scp->next;
		scp = cp->first;
	}
	if (! scp)
		return;
	cp->active = 1;

	/*
	 * Пускаем канал. Когда канал кончит,
	 * будет вызван bсsintr, который сам поймет, что делать дальше.
	 */

	while (dvs_ex4 (0xad9, cp->chan * 2, 0, 8, &cp->ohd)
		printf ("Ошибка при запуске источника БК %d\n", cp->chan);
}

bcrstart (cp)
register struct chantab *cp;
{
	if (cp->idataflag)
		/* Ждем данные отдельным пакетом */
		dvs_ex4 (0xad9, cp->chan * 2 + 1, 0,
			sizeof (cp->rbuf.data), &cp->rbuf.data);
	else
		/* Ждем заголовок */
		dvs_ex4 (0xad9, cp->chan * 2 + 1, 0,
			sizeof (cp->rbuf), &cp->rbuf);
}
