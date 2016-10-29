/*****************************************************************
 *                      ОПИСАНИЕ ФУНКЦИЙ
 *****************************************************************
 *
 *    ПРИМЕЧАНИЕ: Счет разрядов ведется слева направо начиная с 0.
 *                Младший разряд байта - нулевой.
 *
 *________________________________________________________________
 */

# include <conio.h>
# include <dos.h>
# include "nandef.h"
# include "extend.h"				/* Объявление системы расширения */

# define in			inp
# define out			outp

# define BUFSZ 		(4*1024)

/* defines for second 8237A DMA controller */

# define DMA_PG6    		0x89		/* channel 6 page */
# define DMA_CNT6		0xca		/* count */
# define DMA_OFF6		0xc8		/* address */

# define DMA_PG7		0x8a		/* channel 7 page */
# define DMA_CNT7		0xce		/* count */
# define DMA_OFF7		0xcc		/* address */

# define DMA_CSR		0xd0		/* control & status */
# define DMA_REQ		0xd2		/* request */
# define DMA_MASK		0xd4		/* mask */
# define DMA_MODE		0xd6		/* mode */
# define DMA_CLRFF		0xd8		/* flip flop */
# define DMA_TMP		0xda		/* master clear & tmp */
# define DMA_CLRMASK		0xdc		/* clear mask */
# define DMA_ALLMASK		0xde		/* all mask */

# define DMAM_WRITE		4		/* write mode */
# define DMAM_READ		8		/* read mode */

# define DMA_CHAN6		2		/* 6 канал dma на прием */
# define DMA_CHAN7		3		/* 7 канал dma на передачу */

/* адреса управляющих регистров */

# define P_ICR         	port            /* управляющий регистр БК-П */
# define P_INTR        	port            /* регистр прерываний */
# define P_OCR         	(port+1)        /* управляющий регистр БК-И */
# define P_SR          	(port+1)        /* регистр состояния */
# define P_UCR         	(port+2)        /* управляющий регистр УК */
# define P_UDR         	(port+3)        /* регистры данных УК */

/* биты регистра прерываний */

# define R_URDY		0x20		/* готовность от Э1-КБ */
# define R_UDATA 		0x10            /* данные от Э1-КБ */
# define R_REOT		0x08            /* конец передачи */
# define R_RRDY		0x04            /* приемник готов */
# define R_TEOT		0x02            /* конец приема */
# define R_TRDY		0x01            /* источник готов */

/* биты регистра состояния */

# define S_TERR		0x01            /* ошибка при приеме */
# define S_TPARITY		0x02            /* плохая четность при приеме */
# define S_TLENGTH  		0x04            /* длина при приеме короче */
# define S_RERR		0x08            /* ошибка при передаче */

# define S_UIACK		0x10            /* хороший прием по УК */
# define S_UOACK		0x20            /* хорошая квитанция от Э1-КБ */
# define S_UID0		0x40            /* идентификатор 0 */
# define S_UID1		0x80            /* идентификатор 1 */

# define S_UID	   		(S_UID0|S_UID1) /* идентификатор */

# define PHYSADDR(p)		((unsigned) (p) + (((unsigned long) (p) >> 12) & 0xffff0L))

# define BYTE			unsigned char

# define INTCTLR1		0x20
# define INTCTLR2		0xa0

# define SYSTIMER 0x40		/* 55 ms hardware timer */

int intrflag;

void interrupt intrcatcher ();
void (interrupt *savevect) ();

int idma;               	/* input DMA channel number */
int odma;               	/* output DMA channel number */

int port;               	/* port number */
int irq;                	/* hardware IRQ number */

long randx;

short fillbuf [BUFSZ];
unsigned char inbuf [BUFSZ];
unsigned char outbuf [BUFSZ];

int buflen;
int fillmode;
int usercode;

long lpms;			/* loops per millisecond */

int dmacnt [] = { 0xca, 0xca, 0xca, 0xca, 0xca, 0xca, 0xca, 0xce };

int intvecnum [] = {
	0x8, 0x9, 0x71, 0xB, 0xC, 0xD, 0xE, 0xF,
	0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
};

char hexdigit [16] = {
	'0', '1', '2', '3', '4', '5', '6', '7',
	'8', '9', 'a', 'b', 'c', 'd', 'e', 'f',
};

int dma6 (), dma7 ();
long computelpms ();

/*________________________________________________________________
 *
 *                I.  Группа функций записи в UCCR.
 *________________________________________________________________
 *
 *  1. Обращение:  U0_UCCR()
 *
 *     Назначение: Сброс всех разрядов UCCR, включая ИД.
 *     Передаваемые параметры: Нет
 *     Возвращаемый результат: Нет
 *________________________________________________________________
 *
 *  2. Обращение:  W_UCCR(p1)
 *
 *     Назначение: Запись во все разряды UCCR, включая ИД.
 *     Передаваемые параметры:
 *      p1 - 8 символ.  p1  = 'xxxxxxxx' x=0,1
 *     Возвращаемый результат: Нет
 *________________________________________________________________
 *
 *  3. Обращение:  RAZ_V(p1)
 *
 *     Назначение: Установка/Сброс РАЗ.В.
 *     Передаваемые параметры:
 *      p1 - 1 символ.  p1  = '0'  - Сброс РАЗ.В
 *                      p1 != '0' - Установка РАЗ.В
 *     Возвращаемый результат: Нет
 *________________________________________________________________
 *
 *  4. Обращение:  RAZ_P(p1)
 *
 *     Назначение: Установка/Сброс РАЗ.П.
 *     Передаваемые параметры:
 *      p1 - 1 символ.  p1  = '0'  - Сброс РАЗ.П
 *                      p1 != '0' - Установка РАЗ.П
 *     Возвращаемый результат: Нет
 *________________________________________________________________
 *
 *  5. Обращение:  RM_V(p1)
 *
 *     Назначение: Установка/Сброс РМ.В.
 *     Передаваемые параметры:
 *      p1 - 1 символ.  p1  = '0'  - Сброс РМ.В
 *                      p1 != '0' - Установка РМ.В
 *     Возвращаемый результат: Нет
 *________________________________________________________________
 *
 *  6. Обращение:  RM_P(p1)
 *
 *     Назначение: Установка/Сброс РМ.П.
 *     Передаваемые параметры:
 *      p1 - 1 символ.  p1  = '0'  - Сброс РМ.П
 *                      p1 != '0' - Установка РМ.П
 *     Возвращаемый результат: Нет
 *________________________________________________________________
 */

CLIPPER U0_UCCR ()
{
	out (P_UCR, 0x2f);
	_ret ();
}

CLIPPER W_UCCR ()
{
	int reg = atob (_parc (1));

	out (P_UCR, reg);
	_ret ();
}

CLIPPER RAZ_V ()
{
	int set = _parc (1) [0] != '0';

	out (P_UCR, 4 | (set ? 0x10 : 0));
	_ret ();
}

CLIPPER RAZ_P ()
{
	int set = _parc (1) [0] != '0';

	out (P_UCR, 1 | (set ? 0x10 : 0));
	_ret ();
}

CLIPPER RM_V ()
{
	int set = _parc (1) [0] != '0';

	out (P_UCR, 8 | (set ? 0x10 : 0));
	_ret ();
}

CLIPPER RM_P ()
{
	int set = _parc (1) [0] != '0';

	out (P_UCR, 2 | (set ? 0x10 : 0));
	_ret ();
}

/*________________________________________________________________
 *
 *              II.  Группа функций передачи данных УК.
 *________________________________________________________________
 *
 *  1. Обращение:  W_DATA(p1)
 *
 *     Назначение: Выдача байта данных УК.
 *     Передаваемые параметры:
 *        Симв. переменная длиной 8 симв. ( 'xxxxxxxx' x=0,1 )
 *     Возвращаемый результат: Нет
 *________________________________________________________________
 *
 *  2. Обращение:  f = R_DATA()
 *
 *     Назначение: Чтение байта данных УК.
 *     Передаваемые параметры: Нет
 *     Возвращаемый результат:
 *        Симв. переменная длиной 8 симв. ( 'xxxxxxxx' x=0,1 )
 *________________________________________________________________
 *
 *  3. Обращение:  f = IOR2()
 *
 *     Назначение: Чтение по адресу 2.
 *     Передаваемые параметры: Нет
 *     Возвращаемый результат:
 *        Симв. переменная длиной 8 симв. ( 'xxxxxxxx' x=0,1 )
 *________________________________________________________________
 *
 *  4. Обращение:  W_ID(p1)
 *
 *     Назначение: Выдача ИД.
 *     Передаваемые параметры:
 *      p1 - 2 символ.  p1  = 'хх'  - Код ИД ( х=0,1 )
 *     Возвращаемый результат: Нет
 *________________________________________________________________
 *
 *  5. Обращение:  f = R_ID()
 *
 *     Назначение: Чтение ID.
 *     Передаваемые параметры: Нет
 *     Возвращаемый результат:
 *         Симв. переменная длиной 2 симв. ( 'xx' x=0,1 )
 *________________________________________________________________
 */

CLIPPER W_DATA ()
{
	int byte = atob (_parc (1));

	out (P_UDR, byte);
	_ret ();
}

CLIPPER R_DATA ()
{
	static char buf [10];
	int byte;

	byte = in (P_UDR) & 0xff;
	btoa (buf, byte, 8);
	_retc (buf);
}

CLIPPER W_ID ()
{
	int byte = atob (_parc (1));

	out (P_UCR, byte<<6 & 0xc0 | 0x20);
	_ret ();
}

CLIPPER R_ID ()
{
	static char buf [4];
	int byte;

	byte = in (P_SR) >> 6 & 3;
	btoa (buf, byte, 2);
	_retc (buf);
}

CLIPPER IOR2 ()
{
	static char buf [10];
	int byte;

	byte = in (P_UCR) & 0xff;
	btoa (buf, byte, 8);
	_retc (buf);
}

/*________________________________________________________________
 *
 *                III.  Группа функций записи в ICR.
 *________________________________________________________________
 *
 *  1. Обращение:  U0_ICR()
 *
 *     Назначение: Сброс всех разрядов ICR.
 *     Передаваемые параметры: Нет
 *     Возвращаемый результат: Нет
 *________________________________________________________________
 *
 *  2. Обращение:  ICR(p1)
 *
 *     Назначение: Занесение кода в ICR.
 *     Передаваемые параметры:
 *      p1 - 3 символ.  p1  = 'xxx' x=0,1
 *     Возвращаемый результат: Нет
 *________________________________________________________________
 */

CLIPPER U0_ICR ()
{
	out (P_ICR, 0);
	_ret ();
}

CLIPPER ICR ()
{
	int reg = atob (_parc (1));

	out (P_ICR, reg);
	_ret ();
}

/*________________________________________________________________
 *
 *                IV.  Группа функций записи в OCR.
 *________________________________________________________________
 *
 *  1. Обращение:  U0_OCR()
 *
 *     Назначение: Сброс всех разрядов OCR.
 *     Передаваемые параметры: Нет
 *     Возвращаемый результат: Нет
 *________________________________________________________________
 *
 *  2. Обращение:  OCR(p1)
 *
 *     Назначение: Занесение кода в OCR.
 *     Передаваемые параметры:
 *      p1 - 4 символ.  p1  = 'xxxx' x=0,1
 *     Возвращаемый результат: Нет
 *________________________________________________________________
 */

CLIPPER U0_OCR ()
{
	out (P_OCR, 0);
	_ret ();
}

CLIPPER OCR ()
{
	int reg = atob (_parc (1));

	out (P_OCR, reg);
	_ret ();
}

/*________________________________________________________________
 *
 *           V.  Группа функций чтения общих регистров.
 *________________________________________________________________
 *
 *  1. Обращение:  f = INTR()
 *
 *     Назначение: Чтение INTR.
 *     Передаваемые параметры: Нет
 *     Возвращаемый результат:
 *         Симв. переменная длиной 6 симв. ( 'xxxxxx' x=0,1 )
 *________________________________________________________________
 *
 *  2. Обращение:  f = SR()
 *
 *     Назначение: Чтение SR ( Разряды ИД отсекаются).
 *     Передаваемые параметры: Нет
 *     Возвращаемый результат:
 *         Симв. переменная длиной 6 симв. ( 'xxxxxx' x=0,1 )
 *________________________________________________________________
 */

CLIPPER INTR ()
{
	static char buf [8];
	int byte;

	byte = in (P_INTR);
	btoa (buf, byte, 6);
	_retc (buf);
}

CLIPPER SR ()
{
	static char buf [8];
	int byte;

	byte = in (P_SR);
	btoa (buf, byte, 6);
	_retc (buf);
}

/*________________________________________________________________
 *
 *                VI.  Специальные функции.
 *________________________________________________________________
 *
 *  1. Обращение:  f = WAIT_INT(p1)
 *
 *     Назначение: Функция открывает маску на IRQ и ожидает прерывание
 *                 в течение времени, заданного в p1 (таймаут).
 *                   Если в течение заданного времени прерывание поступило,
 *                 в результате возвращается '1', маска на IRQ закрывается.
 *                   Если в течение заданного времени прерывание не поступило,
 *                 в результате возвращается '0', маска на IRQ закрывается.
 *     Передаваемые параметры:
 *      p1 - целое - таймаут в миллисекундах. Если =0 то сигнал прерывания к
 *                      данному моменту уже поступил.
 *     Возвращаемый результат:
 *         Симв. переменная длиной 1 симв. ( 'x' x=0,1 )
 *         EOI ?? фронт ??       х != '0' - прерывание получено
 *                               х  = '0' - таймаут, прерывания нет
 */

CLIPPER WAIT_INT ()
{
	int timo = _parni (1);
	static char retbuf [4];
	long count;

	if (! lpms)
		lpms = computelpms ();

	count = (long) timo * lpms;

	intrflag = 0;
	in (P_UCR);
	setintr (irq);
	while (! intrflag && --count >= 0)	/* idle run */
		idlerun (0L);
	resetintr (irq);

	retbuf [0] = intrflag ? '1' : '0';
	retbuf [1] = 0;
	_retc (retbuf);
}

/*________________________________________________________________
 *
 *  2. Обращение:  f = RUN_DMA(p1,p2,p3)
 *
 *     Назначение: Функция запускает DMA.
 *                 Сбрасывает ( останавливает )??
 *     Передаваемые параметры:
 *      p1 - 1 символ.  p1 = 'I' - запуск DMA ввода в ОЗУ
 *                      p1 = 'O' - запуск DMA вывода из ОЗУ
 *      p2 - 1 символ.  p2 = '0' - запуск обмена на длину массива
 *                      p2 = 'n' - укорочение счетчика слов на n байт
 *      p3 - 1/2 симв.  p3 = '1', '2', ... , '64' - длина передаваемого
 *                                                  массива в Кбайтах
 *      n должно сохраняться для использования при сравнении массивов.
 *     Возвращаемый результат:
 */

CLIPPER RUN_DMA ()              /* RUN_DMA (p1, p2, p3) */
{
	int inp = _parc (1) [0] == 'I';
	int offset = atox (_parc (2));
	int klen = atox (_parc (3));
	register i;

	buflen = klen * 1024;

	if (inp) {
		for (i=0; i<buflen; ++i)
			inbuf [i] = 0;
		(*(idma==6 ? dma6 : dma7)) (inbuf, buflen - offset, 0);
	} else
		(*(odma==6 ? dma6 : dma7)) (outbuf, buflen - offset, 1);
	_ret ();
}

/*________________________________________________________________
 *
 *  3. Обращение:  f = DMA_STATE(p1)
 *
 *     Назначение: Функция передает значения счетчика указанного
 *                 DMA для последующего анализа.
 *     Передаваемые параметры:
 *      p1 - 1 символ.  p1 = 'I' - опрос DMA ввода в ОЗУ
 *                      p1 = 'O' - опрос DMA вывода из ОЗУ
 *     Возвращаемый результат:
 *                  Десятичное число INT ?
 */

CLIPPER DMA_STATE ()
{
	int inpflag = _parc (1) [0] == 'I';
	int portcnt = dmacnt [inpflag ? idma : odma];
	register unsigned len;

	out (DMA_CLRFF, 0);			/* clear flipflop */
	len = in (portcnt) & 0xff;              /* length in words */
	len |= (in (portcnt) << 8) & 0xff00;    /* length in words */
	_retni (len);
}

/*________________________________________________________________
 *
 *  4. Обращение:   SETUP(IOA, DMA, IRQ)
 *
 *     Назначение: Функция преобразует и запоминает IOA, DMA и IRQ для
 *                 использования другими функциями
 *     Передаваемые параметры:
 *     1. IOA     - символьная перем. длиной 3 симв.шестнадцатеричное
 *     2. DMA     - символьная перем. длиной 1 симв.
 *     3. IRQ     - символьная перем. длиной 2 симв.десятичное
 */

CLIPPER SETUP ()        /* SETUP (IOA, DMA, IRQ) */
{
	port = atox (_parc (1));
	idma = atox (_parc (2));
	odma = idma==6 ? 7 : 6;
	irq = atoi (_parc (3));
	_ret ();
}

/*________________________________________________________________
 *
 *  4. Обращение:   FILL_SETUP(codtype, xx)
 *
 *     Назначение: Функция определяет длину и расписывает вспомогательный
 *                 массив требуемым кодом.
 *     Передаваемые параметры:
 *     1. codtype - символьная перем. длиной 2 симв.
 *                  Определяет код и способ росписи WCODUK и wcodbk:
 *         00 -  Не расписывать, возврат
 *         22 -  Переменный код 00            32 -  Постоянный код 00
 *         23 -  Переменный код 55            33 -  Постоянный код FF
 *         24 -  Переменный код xx            34 -  Постоянный код 55
 *         25 -  Переменный код бег.1         35 -  Постоянный код AA
 *         26 -  Переменный код бег.0         36 -  Постоянный код xx
 *         27 -  Переменный код счет.
 *         28 -  Переменный код случ.
 *     2. xx      - символьная перем. длиной 2 симв.шестнадцатеричное
 *                  Код, заданный оператором.
 *
 *       Массив расписывается по следующиму алгоритму:
 *
 *         1. codtype = 22,23
 *              Нечетные элементы массива:
 *          Младш. 8 разрядов = '00' или '55' соответственно.
 *          стар. 2 разряда повторяют 2 млад. разряда.
 *              Четные элементы массива:
 *          Инверсия нечетных элементов массива.
 *
 *         2. codtype = 24
 *              Нечетные элементы массива:
 *          Младш. 8 разрядов = содержимому xx.
 *          стар. 2 разряда повторяют 2 млад. разряда.
 *              Четные элементы массива:
 *          Инверсия нечетных элементов массива.
 *
 *         3. codtype = 25,26,27,28
 *              Каждый элемент массива отличается от предыдущего по
 *              соответствующему правилу. После переполнения 10-ти
 *              разрядов цикл повторяется.
 *
 *         4. Для остальных codtype правила росписи очевидны.
 */

CLIPPER FILL_SETUP ()        /* FILL_SETUP (codtype, xx) */
{
	fillmode = atox (_parc (1));
	usercode = atox (_parc (2));

	fillarray (fillbuf, BUFSZ, fillmode, usercode);
	_ret ();
}

/*________________________________________________________________
 *
 *  4. Обращение:   FILL_BC(len)
 *
 *     Назначение: Функция расписывает массив для БК требуемым кодом.
 *
 *     1. len     - длина массива в байтах (целое).
 */

CLIPPER FILL_BC ()        /* FILL_BC (len) */
{
	int length = _parni (1);
	register count;

	for (count=0; count<length; ++count)
		outbuf [count] = fillbuf [count];
	_ret ();
}

/*________________________________________________________________
 *
 *  4. Обращение:   FILL_UC(WCODUK)
 *
 *     Назначение: Функция расписывает массив для УК требуемым кодом.
 *
 *     1. WCODUK  - имя массива для росписи.
 *                  Элемент массива WCODUK[i] - символьная перем.
 *                  длиной 10 симв.
 *                  Используется для выдачи данных УК.
 */

CLIPPER FILL_UC ()        /* FILL_SETUP (WCODUK) */
{
	int length;
	register i, count;
	char *p;

	length = _parinfa (1, 0);       /* length of WCODUK */

	for (count=0; count<length; ++count) {
		p = _parc (1, count+1);
		usercode = fillbuf [count];
		for (i=0; i<10; ++i) {
			*p++ = '0' | usercode & 1;
			usercode >>= 1;
		}
	}
	_ret ();
}

/*________________________________________________________________
 *
 *  5. Обращение: f = ACOMP(p1, ipbi, ipbo, inoff, outoff, wcell, rcell, bcnt)
 *
 *     Назначение: Функция сравнивает байты массивов записи и чтения в
 *                 возрастающем порядке до исчерпания "короткого" массива
 *                 и помещает результат в f, wcell, rcell и bcnt.
 *     Передаваемые параметры:
 *     1. p1      - символьная перем. длиной 1 симв.
 *                   p1 = '0' - сравнение с начала массивов
 *                   p1 = '+' - продолжить сравнение с возрастанием адресов
 *                   p1 = '-' - продолжить сравнение с убыванием адресов
 *     2. ipbi    - символьная перем. длиной 1 симв.
 *                  != '0' - изменение порядка байтов в массиве приема
 *     3. ipbo    - символьная перем. длиной 1 симв.
 *                  != '0' - изменение порядка байтов в массиве выдачи
 *     4. inoff   - укорочение массива приема
 *     5. outoff  - укорочение массива выдачи
 *     Возвращаемый результат:
 *     1. f       - символьная перем. длиной 1 симв.
 *                  f = '0' - массивы сравнились успешно
 *                  f = '1' - несравнение
 *     2. wcell   - символьная перем. длиной 8 симв.
 *                  Несовпавший элемент массива записи.
 *     3. rcell   - символьная перем. длиной 8 симв.
 *                  Несовпавший элемент массива чтения.
 *     4. bcnt    - символьная перем. длиной 4 симв.
 *                  Номер несовпавшего байта в массивах в HEX.
 */

CLIPPER ACOMP ()
{
	static char retbuf [4];
	char mode = _parc (1) [0];
	int ipbi = _parc (2) [0] != '0';
	int ipbo = _parc (3) [0] != '0';
	int inoff = atox (_parc (4));
	int outoff = atox (_parc (5));
	char *wcell = _parc (6);
	char *rcell = _parc (7);
	char *bcnt = _parc (8);
	static int offset;
	int cmplen;

	if (! buflen)
		goto equal;
	cmplen = buflen - (inoff > outoff ? inoff : outoff);
	if (mode == '0')
		offset = -1;
	if (offset >= cmplen)
		offset = cmplen;
	if (offset < 0)
		offset = -1;
	do {
		offset += mode=='-' ? -1 : 1;
		if (offset >= cmplen)
			goto equal;
		if (offset < 0)
			goto equal;
	} while (inbuf [offset] == outbuf [offset ^ ipbi ^ ipbo]);

	btoa (wcell, outbuf [offset ^ ipbi ^ ipbo], 8);
	btoa (rcell, inbuf [offset], 8);
	xtoa (bcnt, offset, 4);
	retbuf [0] = '1';
	retbuf [1] = 0;
	_retc (retbuf);
	return;
equal:
	retbuf [0] = '0';
	retbuf [1] = 0;
	_retc (retbuf);
}

/*________________________________________________________________
 *
 *  6. Обращение: f = PAR(p1)
 *
 *     Назначение: Функция определяет длину символьной переменной p1
 *                 и вычисляет сумму по модулю 2 всех символов p1.
 *     Передаваемые параметры:
 *                       p1 - символьная перем. длиной n симв.
 *                            Каждый символ - '0' или '1'.
 *     Возвращаемый результат:
 *                 f - символьная перем. длиной 1 симв.
 *                     f = '0' или '1' по результату вычисления четности.
 */

CLIPPER PAR ()          /* f = PAR (p1) */
{
	char *p = _parc (1);
	register i, rez;
	static char retbuf [2];

	rez = 0;
	for (i=strlen(p); --i>=0; ++p)
		rez ^= *p;
	retbuf [0] = '0' | rez & 1;
	retbuf [1] = 0;
	_retc (retbuf);
}

/*________________________________________________________________
 *
 * 		Вспомогательные пpоцедуpы.
 */

atox (p)
register char *p;
{
	int ret;

	for (ret=0; *p; ++p)
		ret = ret << 4 | hexdig (*p);
	return (ret);
}

atoi (p)
register char *p;
{
	int ret;

	for (ret=0; *p; ++p)
		ret = ret * 10 + (*p - '0');
	return (ret);
}

atob (p)
register char *p;
{
	register i, ret;

	for (ret=0, i=0; i<8; ++p, ++i)
		ret |= (*p & 1) << i;
	return (ret);
}

hexdig (c)
{
	switch (c) {
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
		return (c - '0');
	case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
		return (c - 'a' + 10);
	case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
		return (c - 'A' + 10);
	}
	return (0);
}

btoa (p, b, n)
register char *p;
{
	while (--n >= 0) {
		*p++ = '0' | b & 1;
		b >>= 1;
	}
	*p = 0;
}

xtoa (p, b, n)
register char *p;
{
	while (--n >= 0)
		*p++ = hexdigit [b >> (n*4) & 0xf];
	*p = 0;
}

setrandom (seed)
{
	randx = seed;
}

getrandom ()
{
	randx = randx * 1103515245L + 12345;
	return (randx >> 16 & 0x7fff);
}

fillarray (buf, length, mode, ucode)
short *buf;
{
	register count;

	switch (mode) {
	default:                /* 00 - Не расписывать, возврат */
		return;
	case 0x22:              /* 22 - Переменный код 00 */
		ucode = 0x00;
varloop:
		ucode = ucode & 0xff | ucode << 8 & 0x300;
		for (count=0; count<length; ++count) {
			buf [count] = ucode;
			ucode ^= 0x3ff;
		}
		break;
	case 0x23:              /* 23 - Переменный код 55 */
		ucode = 0x55;
		goto varloop;
	case 0x24:              /* 24 - Переменный код xx */
		goto varloop;
	case 0x25:              /* 25 - Переменный код бег.1 */
		ucode = 1;
		for (count=0; count<length; ++count) {
			buf [count] = ucode;
			ucode = ucode << 1 & 0x3ff;
			if (! ucode)
				ucode = 1;
		}
		break;
	case 0x26:              /* 26 - Переменный код бег.0 */
		ucode = 1;
		for (count=0; count<length; ++count) {
			buf [count] = ucode ^ 0x3ff;
			ucode = ucode << 1 & 0x3ff;
			if (! ucode)
				ucode = 1;
		}
		break;
	case 0x27:              /* 27 - Переменный код счет. */
		for (count=0; count<length; ++count)
			buf [count] = count & 0xff | count << 8 & 0x300;
		break;
	case 0x28:              /* 28 - Переменный код случ. */
		setrandom (ucode);
		for (count=0; count<length; ++count)
			buf [count] = getrandom () & 0x3ff;
		break;
	case 0x32:              /* 32 - Постоянный код 00 */
		ucode = 0x00;
fixloop:
		ucode = ucode & 0xff | ucode << 8 & 0x300;
		for (count=0; count<length; ++count)
			buf [count] = ucode;
		break;
	case 0x33:              /* 33 - Постоянный код FF */
		ucode = 0xff;
		goto fixloop;
	case 0x34:              /* 34 - Постоянный код 55 */
		ucode = 0x55;
		goto fixloop;
	case 0x35:              /* 35 - Постоянный код AA */
		ucode = 0xaa;
		goto fixloop;
	case 0x36:              /* 36 - Постоянный код xx */
		goto fixloop;
	}
}

dma6 (buf, len, wflag)				/* настройка канала 6 dma */
char *buf;
register len;
{
	register long addr;
	register mode;

	addr = PHYSADDR ((char far *) buf);
	len = (len + 1) / 2 - 1;
	mode = (wflag ? DMAM_READ : DMAM_WRITE) | DMA_CHAN6;
	out (DMA_CLRFF, 0);			/* clear flipflop */
	out (DMA_PG6, (BYTE) (addr>>16));	/* page number */
	out (DMA_OFF6, (BYTE) (addr>>1));	/* page offset */
	out (DMA_OFF6, (BYTE) (addr>>9));	/* page offset */
	out (DMA_CNT6, (BYTE) len);		/* length in words */
	out (DMA_CNT6, (BYTE) (len>>8));	/* length in words */
	out (DMA_MODE, mode);			/* Demand Write Chan 2 */
	out (DMA_MASK, DMA_CHAN6);		/* clear channel 2 mask */
}

dma7 (buf, len, wflag)				/* установка канала 7 dma */
char *buf;
register len;
{
	register long addr;
	register mode;

	addr = PHYSADDR ((char far *) buf);
	len = (len + 1) / 2 - 1;
	mode = (wflag ? DMAM_READ : DMAM_WRITE) | DMA_CHAN7;
	out (DMA_CLRFF, 0);			/* clear flipflop */
	out (DMA_PG7, (BYTE) (addr>>16));	/* page number */
	out (DMA_OFF7, (BYTE) (addr>>1));	/* page offset */
	out (DMA_OFF7, (BYTE) (addr>>9));	/* page offset */
	out (DMA_CNT7, (BYTE) len);		/* length in words */
	out (DMA_CNT7, (BYTE) (len>>8));	/* length in words */
	out (DMA_MODE, mode);			/* Demand Read Chan 3 */
	out (DMA_MASK, DMA_CHAN7);		/* clear channel 3 mask */
}

void interrupt intrcatcher ()
{
	++intrflag;
	out (INTCTLR1, 0x20);
	out (INTCTLR2, 0x20);
}

setintr (inum)
{
	/* установка перехватчика прерывания и маски */
	_disable ();
	savevect = _dos_getvect (intvecnum [inum]);
	_dos_setvect (intvecnum [inum], intrcatcher);
	if (inum < 8)
		out (INTCTLR1+1, in (INTCTLR1) & ~(1 << inum));
	else
		out (INTCTLR2+1, in (INTCTLR2) & ~(1 << (inum-8)));
	_enable ();
}

resetintr (inum)
{
	/* установка перехватчика прерывания и маски */
	_disable ();
	_dos_setvect (intvecnum [inum], savevect);
	if (inum < 8)
		out (INTCTLR1+1, in (INTCTLR1) | 1 << inum);
	else
		out (INTCTLR2+1, in (INTCTLR2) | 1 << (inum-8));
	_enable ();
}

idlerun (dummy)
long dummy;
{
	int i;

	for (i=0; i<10; ++i)
		dummy = dummy * dummy + 1;
	return (dummy);
}

long computelpms ()
{
	long count;
	register t1, t2;

	t1 = in (SYSTIMER);
	t2 = in (SYSTIMER);
	while ((t1 == in (SYSTIMER)) & (t2 == in (SYSTIMER)))
		/* void */;
	t1 = in (SYSTIMER);
	t2 = in (SYSTIMER);
	for (count=0; (t1 == in (SYSTIMER)) & (t2 == in (SYSTIMER)); ++count)
		idlerun (0L);
	return (count / 55 + 1);
}