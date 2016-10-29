/*
 * Эмуляция контроллера с 16-битной шиной адрес/данные.
 *
 * Автор: Сергей Вакуленко, ИТМиВТ 2008.
 */
#include <string.h>
#include <stdlib.h>
#include "ctlr.h"

#ifdef MATLAB_MEX_FILE
#   include "mex.h"
#else
#   include "simulator.h"
#endif

/* Состояние конечного автомата кодера Manchester */
enum {
	TX_STATE_IDLE,
	TX_STATE_PRE_A,
	TX_STATE_PRE_B,
	TX_STATE_PRE_C,
	TX_STATE_PRE_D,
	TX_STATE_SEND_0A,
	TX_STATE_SEND_0B,
	TX_STATE_SEND_1A,
	TX_STATE_SEND_1B,
	TX_STATE_FINAL,
};

/* Состояние конечного автомата декодера Manchester */
enum {
	RX_STATE_IDLE,
	RX_STATE_PRE_START,
	RX_STATE_PRE_A,
	RX_STATE_PRE_B,
	RX_STATE_PRE_C,
	RX_STATE_PRE_D,
	RX_STATE_START,
	RX_STATE_NEXT,
	RX_STATE_GET_0,
	RX_STATE_GET_1,
};

/*
 * Выдача следующего бита данных для передачи.
 * Номер бита задан счетчиком tx_bitnum.
 * Младший бит передаётся раньше.
 */
int ctlr_transmit_next (controller_t *c)
{
	int bit = 0;

	if (c->tx_flags & TTC_MEDL_START) {
		/* Передаём стартовый пакет. */
		if (c->tx_bitnum < 8) {
			bit = 0;
		} else if (c->tx_bitnum < 8+8) {
			bit = c->cmr >> (c->tx_bitnum - 8) & 1;
		} else if (c->tx_bitnum < 8+8+8) {
			bit = c->nnr >> (c->tx_bitnum - 16) & 1;
		} else if (c->tx_bitnum < 8+8+8+32) {
			bit = c->slot_time >> (c->tx_bitnum - 24) & 1;
		} else if (c->tx_bitnum < 8+8+8+32+32) {
			bit = c->tx_crc >> (c->tx_bitnum - 24 - 32) & 1;
		} else
			printf ("--%s-- tx error: invalid start bitnum=%d\n",
				c->name, c->tx_bitnum);
	} else {
		/* Передаём пакет данных. */
		if (c->tx_bitnum < 8) {
			bit = c->cmr >> c->tx_bitnum & 1;
		} else if (c->tx_bitnum < 8 + c->vecbits) {
			bit = c->rvec >> (c->tx_bitnum - 8) & 1;
		} else if (c->tx_bitnum < c->tx_bits - 32) {
			/* В начале данных - два байта статуса. */
			bit = ctlr_data_byte (c, c->tx_addr + (c->tx_bitnum + 8 -
				c->vecbits) / 8) >> (c->tx_bitnum & 7) & 1;
		} else if (c->tx_bitnum < c->tx_bits) {
			bit = c->tx_crc >> ((c->tx_bitnum + 32 - c->tx_bits) & 31) & 1;
		} else
			printf ("--%s-- tx error: invalid start bitnum=%d\n",
				c->name, c->tx_bitnum);
	}
	++c->tx_bitnum;
	return bit;
}

/*
 * Передача очередного полубита данных.
 */
void ctlr_transmit_step (controller_t *c)
{
	/* Кодирование Manchester. В преамбуле передаем "101010...".
	 * Начало пакета отмечается двумя единицами подряд: "101011xxx..xx111111"*/
	switch (c->tx_state) {
	case TX_STATE_IDLE:
/*		printf ("--%s-- tx idle: nnr=%d, current slot node=%d\n", c->name, c->nnr, c->slot_node);*/
		c->tx = 1;
		if (c->slot_node == c->nnr) {
			/* Есть данные для передачи в текущем слоте. */
			c->tx_state = TX_STATE_PRE_A;
		} else {
			/* В этом слоте мы только принимаем данные. */
			c->tx_state = TX_STATE_IDLE;
		}
		break;
	case TX_STATE_PRE_A:
		c->tx = 1;
		c->tx_state = TX_STATE_PRE_B;
		break;
	case TX_STATE_PRE_B:
		c->tx = 0;
		/* Зазор-преамбула в начале слота. */
		if (c->tx_pream > 0) {
			--c->tx_pream;
			c->tx_state = TX_STATE_PRE_C;
		} else
			c->tx_state = TX_STATE_SEND_1A;
		break;
	case TX_STATE_PRE_C:
		c->tx = 0;
		c->tx_state = TX_STATE_PRE_D;
		break;
	case TX_STATE_PRE_D:
		c->tx = 1;
		c->tx_state = TX_STATE_PRE_A;
		break;
	case TX_STATE_SEND_0A:
		c->tx = 0;
		c->tx_state = TX_STATE_SEND_0B;
		break;
	case TX_STATE_SEND_1A:
		c->tx = 1;
		c->tx_state = TX_STATE_SEND_1B;
		break;
	case TX_STATE_SEND_0B:
		c->tx = 1;
next:		if (c->tx_bitnum < c->tx_bits) {
			/* Есть данные для передачи в текущем слоте. */
			if (ctlr_transmit_next (c)) {
				/* Бит 1. */
				c->tx_state = TX_STATE_SEND_1A;
			} else {
				/* Бит 0. */
				c->tx_state = TX_STATE_SEND_0A;
			}
		} else {
			/* Передача завершена, устанавливаем статус. */
			ctlr_or_data_word (c, c->tx_addr, TTC_GSR_TDN);
			c->gsr |= TTC_GSR_TDN;
			c->tx_state = TX_STATE_FINAL;
		}
		break;
	case TX_STATE_SEND_1B:
		c->tx = 0;
		goto next;
	case TX_STATE_FINAL:
		c->tx = 1;
		break;
	}
}

/*
 * Обработка принятого бита данных.
 */
void ctlr_receive_bit (controller_t *c, int nbus, int bit)
{
	/* Помещаем принятый бит в однобайтовый сдвиговый регистр. */
	c->rx_byte [nbus] >>= 1;
	if (bit)
		c->rx_byte [nbus] |= 0x80;
	++c->rx_bitnum [nbus];
	if (c->rx_bitnum [nbus] & 7) {
		/* Дожидаемся полного байта. */
		return;
	}
	/* if (! nbus) printf ("--%s--%d rx byte %02x\n",
		c->name, nbus, c->rx_byte [nbus]); */

	/* Считаем контрольную сумму. */
	c->rx_crc [nbus] = crc32_ipmce_byte (c->rx_crc [nbus], c->rx_byte [nbus]);

	/* Первый байт - режим кластера. */
	if (c->rx_bitnum [nbus] == 8) {
		c->rx_cmode [nbus] = c->rx_byte [nbus];
		return;
	}

	if (c->rx_flags & TTC_MEDL_START) {
		/* Принимаем стартовый пакет, 6 байтов. */
		if (c->rx_bitnum [nbus] <= c->rx_bits - 32) {
			/* Данные пользователя. */
			ctlr_store_data_byte (c, c->rx_addr [nbus] +
				c->rx_bitnum [nbus] / 8, c->rx_byte [nbus]);
		}
	} else {
		/* Принимаем пакет данных. */
		if (c->rx_bitnum [nbus] <= 8 + c->vecbits) {
			/* Шкала узлов. */
			if (c->rx_bitnum [nbus] == 16)
				c->rx_vector [nbus] = c->rx_byte [nbus];
			else
				c->rx_vector [nbus] |= (unsigned long long)
					c->rx_byte [nbus] << (c->rx_bitnum [nbus] - 16);
			return;
		}
		if (c->rx_bitnum [nbus] <= c->rx_bits - 32) {
			/* Данные пользователя. */
			ctlr_store_data_byte (c, c->rx_addr [nbus] +
				(c->rx_bitnum [nbus] - c->vecbits) / 8,
				c->rx_byte [nbus]);
		}
	}
}

/*
 * Закончен приём пакета.
 */
void ctlr_receive_done (controller_t *c, int nbus)
{
	int i;

	if (! nbus) printf ("--%s--%d rx done: %d bits from node %d, sum=%08lx, time=%ld\n",
		c->name, nbus, c->rx_bitnum [nbus], c->slot_node, c->rx_crc [nbus], c->rx_time [nbus]);

	if (ctlr_data_word (c, c->rx_addr [nbus]) & TTC_RSR_REV) {
		/* Были ошибки кодирования. */
		return;
	}
	if (c->rx_bitnum [nbus] != c->rx_bits) {
		/* Неверная длина пакета. */
		ctlr_or_data_word (c, c->rx_addr [nbus], TTC_RSR_RLE);
		c->rsr [nbus] |= TTC_RSR_RLE;
		if (! nbus) printf ("--%s--%d bad length %d bytes, expected %d bytes\n",
			c->name, nbus, c->rx_bitnum [nbus] / 8, c->rx_bits / 8);
		return;
	}
	if (c->rx_crc [nbus] != 0) {
		/* Неправильная контрольная сумма. */
		ctlr_or_data_word (c, c->rx_addr [nbus], TTC_RSR_CSE);
		c->rsr [nbus] |= TTC_RSR_CSE;
		return;
	}
	if (c->rx_flags & TTC_MEDL_START) {
		/* В стартовом пакете первый байт должен быть нулевым. */
		if (c->rx_cmode [nbus] != 0) {
			/* Receive mode error. */
			ctlr_or_data_word (c, c->rx_addr [nbus], TTC_RSR_RCME);
			c->rsr [nbus] |= TTC_RSR_RCME;
			return;
		}
		ctlr_or_data_word (c, c->rx_addr [nbus], TTC_RSR_STRT);
		c->rsr [nbus] |= TTC_RSR_STRT;

		/* К шести принятым байтам стартового пакета дописываем
		 * ещё 4 - время приёма пакета. */
		ctlr_store_data_byte (c, c->rx_addr [nbus] + 8, c->rx_time [nbus]);
		ctlr_store_data_byte (c, c->rx_addr [nbus] + 9, c->rx_time [nbus] >> 8);
		ctlr_store_data_byte (c, c->rx_addr [nbus] + 10, c->rx_time [nbus] >> 16);
		ctlr_store_data_byte (c, c->rx_addr [nbus] + 11, c->rx_time [nbus] >> 24);

		printf ("--%s--%d rx data:", c->name, nbus);
		for (i=0; i<10; ++i)
			printf (" %02x", ctlr_data_byte (c, c->rx_addr [nbus] + 2 + i));
		printf ("\n");
	} else {
		/* В пакете данных поле режима кластера должно соответствовать
		 * значению CMODE регистра CMR. */
		if (c->rx_cmode [nbus] != (c->cmr & TTC_MR_CMODE_MASK)) {
			ctlr_or_data_word (c, c->rx_addr [nbus], TTC_RSR_RCME);
			c->rsr [nbus] |= TTC_RSR_RCME;
			return;
		}
		/* Устанавливаем бит регистра AVEC, соответствующий
		 * передававшему узлу. */
		if ((c->rx_vector [nbus] >> c->nnr) & 1)
			c->avec |= 1ULL << c->slot_node;
		else
			c->avec &= ~(1ULL << c->slot_node);

		printf ("--%s--%d rx data:", c->name, nbus);
		for (i=0; i<(c->rx_bitnum [nbus] - c->vecbits) / 8 - 5; ++i)
			printf (" %02x", ctlr_data_byte (c, c->rx_addr [nbus] + 2 + i));
		printf ("\n");
	}
	ctlr_or_data_word (c, c->rx_addr [nbus], TTC_RSR_RDN);
	c->rsr [nbus] |= TTC_RSR_RDN;
}

/*
 * Приём очередного полубита данных.
 */
void ctlr_receive_step (controller_t *c, int nbus, int input)
{
	if (c->slot_node == c->nnr) {
		/* В этом слоте мы передаём данные, приёмник отключен. */
		return;
	}

	switch (c->rx_state [nbus]) {
	case RX_STATE_IDLE:
		if (! input) {
			/* Получили стартовый 0, ждем второй 0. */
			/*if (! nbus) printf ("--%s--%d rx pre start, from node %d\n",
				c->name, nbus, c->slot_node);*/
			c->rx_state [nbus] = RX_STATE_PRE_START;
		}
		break;
	case RX_STATE_PRE_START:
		if (input) {
			/* После стартового 0 получили единицу - ошибка. */
			/* Receive encoding violation. */
			ctlr_or_data_word (c, c->rx_addr [nbus], TTC_RSR_REV);
			c->rsr [nbus] |= TTC_RSR_REV;
			++c->evc [nbus];
			c->rx_state [nbus] = RX_STATE_IDLE;
		} else {
			/* Получили стартовый 0, ждем второй 0. */
			c->rx_state [nbus] = RX_STATE_PRE_D;
		}
		break;
	case RX_STATE_PRE_A:
		/* Преамбула, ждем 1. */
		if (input) {
			c->rx_state [nbus] = RX_STATE_PRE_B;
		} else {
			/* Ошибка. */
			/* Receive encoding violation. */
			ctlr_or_data_word (c, c->rx_addr [nbus], TTC_RSR_REV);
			c->rsr [nbus] |= TTC_RSR_REV;
			++c->evc [nbus];
		}
		break;
	case RX_STATE_PRE_B:
		/* Преамбула, ждем 0. */
		if (input) {
			/* Ошибка. */
			++c->evc [nbus];
		} else {
			c->rx_state [nbus] = RX_STATE_PRE_C;
		}
		break;
	case RX_STATE_PRE_C:
		/* Преамбула, ждем 0. */
		if (input) {
			/* Начало пакета, запоминаем время. */
			c->rx_state [nbus] = RX_STATE_START;
			c->rx_time [nbus] = c->ctr;
			/*if (! nbus) printf ("--%s--%d rx time=%ld\n",
				c->name, nbus, c->rx_time [nbus]);*/
		} else {
			c->rx_state [nbus] = RX_STATE_PRE_D;
		}
		break;
	case RX_STATE_PRE_D:
		/* Преамбула, ждем 1. */
		if (input) {
			c->rx_state [nbus] = RX_STATE_PRE_A;
		} else {
			/* Ошибка. */
			++c->evc [nbus];
		}
		break;
	case RX_STATE_START:
		/* Начало пакета, ждем 0. */
		/*if (! nbus) printf ("--%s--%d rx start, from node %d\n",
			c->name, nbus, c->slot_node);*/
		if (input) {
			/* Ошибка. */
			++c->evc [nbus];
			c->rx_state [nbus] = RX_STATE_PRE_A;
		} else {
			/* Начальное значение контрольной суммы. */
			c->rx_crc [nbus] = c->sid;
			c->rx_bitnum [nbus] = 0;
			c->rx_state [nbus] = RX_STATE_NEXT;
		}
		break;
	case RX_STATE_NEXT:
		/* Ожидание очередного бита данных. */
		if (input) {
			c->rx_state [nbus] = RX_STATE_GET_1;
		} else {
			c->rx_state [nbus] = RX_STATE_GET_0;
		}
		break;
	case RX_STATE_GET_0:
		/* Приём бита 0. */
		if (input) {
			/* Приняли. */
			ctlr_receive_bit (c, nbus, 0);
		} else {
			/* Ошибка: receive encoding violation. */
			ctlr_or_data_word (c, c->rx_addr [nbus], TTC_RSR_REV);
			c->rsr [nbus] |= TTC_RSR_REV;
			++c->evc [nbus];
		}
		c->rx_state [nbus] = RX_STATE_NEXT;
		break;
	case RX_STATE_GET_1:
		/* Приём бита 1. */
		if (input) {
			if (! (c->rx_bitnum [nbus] & 7)) {
				/* Граница байта - конец пакета. */
				ctlr_receive_done (c, nbus);
				c->rx_state [nbus] = RX_STATE_IDLE;
				break;
			}
			/* Ошибка: receive encoding violation. */
			ctlr_or_data_word (c, c->rx_addr [nbus], TTC_RSR_REV);
			c->rsr [nbus] |= TTC_RSR_REV;
			++c->evc [nbus];
		} else {
			/* Приняли. */
			ctlr_receive_bit (c, nbus, 1);
		}
		c->rx_state [nbus] = RX_STATE_NEXT;
		break;
	}
}

void ctlr_receive_init (controller_t *c, int slot_addr)
{
	medl_t *sp;

	sp = (medl_t*) ((char*) c->medl + slot_addr);
	c->rx_bits = 8 + (sp->bytes * 8) + 32;
	c->rx_flags = sp->flags;
	c->rx_addr[0] = sp->data_addr0 - TTC_RAM_BASE;
	c->rx_addr[1] = sp->data_addr1 - TTC_RAM_BASE;
	c->rx_cmode[0] = 0;
	c->rx_cmode[1] = 0;
	c->rx_vector[0] = 0;
	c->rx_vector[1] = 0;
	ctlr_store_data_word (c, c->rx_addr [0], 0);
	ctlr_store_data_word (c, c->rx_addr [1], 0);

	/* TODO: аварийное завершение приёма текущего пакета, если
	 * не вписались в отведённую длительность слота. */
	c->rx_state[0] = RX_STATE_IDLE;
	c->rx_state[1] = RX_STATE_IDLE;
}

/*
 * Переход на следующий слот.
 */
void ctlr_next_slot (controller_t *c, int slot_addr)
{
	medl_t *sp;
	int i;

	if (c->scp) {
		/* Устанавливаем статус завершения предыдущего слота. */
		sp = (medl_t*) ((char*) c->medl + c->scp);
		ctlr_or_data_word (c, sp->data_addr0 - TTC_RAM_BASE, TTC_RSR_SF);
		ctlr_or_data_word (c, sp->data_addr1 - TTC_RAM_BASE, TTC_RSR_SF);

		/* Если мы должны были принимать пакет (но не в режиме старта),
		 * устанавливаем бит регистра RVEC, соответствующий
		 * передававшему узлу. Единица, если хотя бы по одной из шин
		 * был успешный приём. */
		if (sp->node != c->nnr && ! (c->cmr & TTC_MR_STRT)) {
			if ((ctlr_data_word (c, sp->data_addr0 - TTC_RAM_BASE) & TTC_RSR_RDN) ||
			    (ctlr_data_word (c, sp->data_addr1 - TTC_RAM_BASE) & TTC_RSR_RDN))
				c->rvec |= 1ULL << sp->node;
			else
				c->rvec &= ~(1ULL << sp->node);
		}
	}
	/* Переключаемся на следующий слот. */
	c->scp = slot_addr;

	/* Загружаем строку в текущие переменные. */
	sp = (medl_t*) ((char*) c->medl + c->scp);
	c->slot_ticks = sp->ticks;
	c->slot_node = sp->node;
	c->slot_time = c->ctr;

	/* В начале данных - два байта статуса.
	 * Обнуляем их в начале слота. После завершения приёма или передачи
	 * контроллер поместит сюда ненулевой статус завершения. */
	ctlr_store_data_word (c, sp->data_addr0 - TTC_RAM_BASE, 0);
	ctlr_store_data_word (c, sp->data_addr1 - TTC_RAM_BASE, 0);

	/* Для передатчика. */
	c->tx_bytes = sp->bytes;
	c->tx_addr = sp->data_addr0 - TTC_RAM_BASE;
	c->tx_flags = sp->flags;

	/* Длина преамбулы определяется регистром TPLR. */
	c->tx_pream = c->tplr + 1;

	if (c->tx_flags & TTC_MEDL_START) {
		/* Стартовый пакет. */
		c->tx_bits = 8 + 8 + 8 + 32 + 32;
	} else {
		/* Передаём пакет данных. */
		c->tx_bits = 8 + c->vecbits + (c->tx_bytes * 8) + 32;
	}
	/* TODO: аварийное завершение передачи текущего пакета, если не вписались
	 * в отведённую длительность слота. */
	c->tx_state = TX_STATE_IDLE;
	c->tx_bitnum = 0;
	if (c->slot_node == c->nnr) {
		/* Вычисляем контрольную сумму будущего пакета.
		 * Начинаем с идентификатора расписания. */
		c->tx_crc = c->sid;
		if (c->tx_flags & TTC_MEDL_START) {
			/* Стартовый пакет. */
			c->tx_crc = crc32_ipmce_byte (c->tx_crc, 0);
			c->tx_crc = crc32_ipmce_byte (c->tx_crc, c->cmr);
			c->tx_crc = crc32_ipmce_byte (c->tx_crc, c->nnr);
			c->tx_crc = crc32_ipmce_byte (c->tx_crc, c->slot_time);
			c->tx_crc = crc32_ipmce_byte (c->tx_crc, c->slot_time >> 8);
			c->tx_crc = crc32_ipmce_byte (c->tx_crc, c->slot_time >> 16);
			c->tx_crc = crc32_ipmce_byte (c->tx_crc, c->slot_time >> 24);

			printf ("--%s-- tx start data: %02x %02x %02x %02x %02x %02x\n",
				c->name, (unsigned char) c->cmr,
				(unsigned char) c->nnr, (unsigned char) c->slot_time,
				(unsigned char) (c->slot_time >> 8),
				(unsigned char) (c->slot_time >> 16),
				(unsigned char) (c->slot_time >> 24));
		} else {
			/* Пакет данных. */
			c->tx_crc = crc32_ipmce_byte (c->tx_crc, c->cmr);
			c->tx_crc = crc32_ipmce_byte (c->tx_crc, c->rvec);
			c->tx_crc = crc32_ipmce_byte (c->tx_crc, c->rvec >> 8);
			if (c->vecbits >= 32) {
				c->tx_crc = crc32_ipmce_byte (c->tx_crc, c->rvec >> 16);
				c->tx_crc = crc32_ipmce_byte (c->tx_crc, c->rvec >> 24);
			}
			if (c->vecbits >= 64) {
				c->tx_crc = crc32_ipmce_byte (c->tx_crc, c->rvec >> 32);
				c->tx_crc = crc32_ipmce_byte (c->tx_crc, c->rvec >> 40);
				c->tx_crc = crc32_ipmce_byte (c->tx_crc, c->rvec >> 48);
				c->tx_crc = crc32_ipmce_byte (c->tx_crc, c->rvec >> 56);
			}
			for (i=0; i<c->tx_bytes; ++i)
				c->tx_crc = crc32_ipmce_byte (c->tx_crc,
					ctlr_data_byte (c, c->tx_addr + 2 + i));

			printf ("--%s-- tx data:", c->name);
			for (i=0; i<c->tx_bytes; ++i)
				printf (" %02x", ctlr_data_byte (c, c->tx_addr + 2 + i));
			printf ("\n");
		}
	}

	/* Для приёмника.
	 * В режиме старта работа приёмника не зависит от текущего слота. */
	if (! (c->cmr & TTC_MR_STRT)) {
		ctlr_receive_init (c, c->scp);
	}
}

/*
 * Чтение 16-битного слова из памяти.
 * Адрес словный, младший бит равен 0.
 */
unsigned int mem_read (controller_t *c, unsigned int addr)
{
	unsigned int val;

	if (addr & 1) {
		printf ("--%s-- read error: odd address %04x\n", c->name, addr);
		addr &= ~1;
	}
	if (addr >= TTC_REG_BASE) {
		switch (addr) {
/*
 * Для компактности кода, широкие регистры и группы
 * регистров оформим в виде макросов.
 */
#define CASE_REG32(reg, field) \
		case TTC_##reg:\
			val = c->field & 0xffff; \
			break; \
		case TTC_##reg + 2: \
			val = (c->field >> 16) & 0xffff; \
			break
#define CASE_REG64(reg, field) \
		case TTC_##reg: \
			val = c->field & 0xffff; \
			break; \
		case TTC_##reg + 2: \
			val = (c->field >> 16) & 0xffff; \
			break; \
		case TTC_##reg + 4: \
			val = (c->field >> 32) & 0xffff; \
			break; \
		case TTC_##reg + 6: \
			val = (c->field >> 48) & 0xffff; \
			break
#define CASE_REG16_TRIPLE(reg, field) \
		case TTC_##reg(0):\
			val = c->field[0]; \
			break; \
		case TTC_##reg(1): \
			val = c->field[1]; \
			break
#define CASE_REG32_TRIPLE(reg, field) \
		case TTC_##reg(0):\
			val = c->field[0] & 0xffff; \
			break; \
		case TTC_##reg(0) + 2: \
			val = (c->field[0] >> 16) & 0xffff; \
			break; \
		case TTC_##reg(1): \
			val = c->field[1] & 0xffff; \
			break; \
		case TTC_##reg(1) + 2: \
			val = (c->field[1] >> 16) & 0xffff; \
			break; \
		case TTC_##reg(2): \
			val = c->field[2] & 0xffff; \
			break; \
		case TTC_##reg(2) + 2: \
			val = (c->field[2] >> 16) & 0xffff; \
			break

		/*
		 * Общесистемные регистры
		 */
		case TTC_FRR:			/* firmware revision register */
			val = TTC_REVISION_NUMBER << 8 |
				TTC_REVISION_DATE >> 16;
			break;

		case TTC_FDR:			/* firmware date register */
			val = TTC_REVISION_DATE & 0xffff;
			break;

		CASE_REG32 (SID, sid);		/* schedule identification register */

		case TTC_NID:			/* node number register */
			val = c->nnr;
			break;

		case TTC_GCR:			/* global configuration register */
			val = c->gcr;
			break;

		case TTC_GSR:			/* global status register */
			val = c->gsr;
			break;

		case TTC_IER:			/* interrupt enable register */
			val = c->ier;
			break;

		CASE_REG16_TRIPLE (RSR, rsr);	/* receive status register */

		case TTC_RIER:			/* receive interrupt enable register */
			val = c->rier;
			break;

		case TTC_CMR:			/* current mode register */
			val = c->cmr;
			break;

		case TTC_NMR:			/* next mode register */
			val = c->nmr;
			break;

		case TTC_SSR:			/* schedule start register */
			val = c->ssr;
			break;

		case TTC_SCP:			/* schedule current pointer */
			val = c->scp;
			break;

		case TTC_SSRP:			/* schedule start receive pointer */
			val = c->ssrp;
			break;

		case TTC_TPLR:			/* transmit preamble length register */
			val = c->tplr;
			break;

		/* Вектора */
		CASE_REG64 (RVEC, rvec);	/* receive vector */
		CASE_REG64 (AVEC, avec);	/* acknowledge vector */

		/* Время */
		CASE_REG32 (CTR, ctr);		/* cycle time register */
		CASE_REG32 (CNR, cnr);		/* cycle number register */
		CASE_REG32 (TDR, tdr);		/* time delta register */
		CASE_REG32 (MTR, mtr);		/* max time register */
		CASE_REG32 (NMTR, nmtr);	/* next max time register */

		CASE_REG32_TRIPLE (MINL, minl);	/* slowest node time */
		CASE_REG32_TRIPLE (MINU, minu);	/* next to slowest node time */
		CASE_REG32_TRIPLE (MAXL, maxl);	/* slowest node time */
		CASE_REG32_TRIPLE (MAXU, maxu);	/* next to slowest node time */

		/* Счётчики */
		CASE_REG32_TRIPLE (RPC, rpc);
		CASE_REG32 (TPC, tpc);
		CASE_REG32_TRIPLE (RSPC, rspc);
		CASE_REG32_TRIPLE (EVC, evc);
		CASE_REG32_TRIPLE (CEC, cec);
		CASE_REG32_TRIPLE (CMEC, cmec);

		default:
			printf ("--%s-- error reading register %04x\n", c->name, addr);
			val = 0;
		}
/*		printf ("--%s-- reg %04x read %04x\n", c->name, addr, val);*/

	} else if (addr >= TTC_RAM_BASE) {
		/* Память данных */
		val = ctlr_data_word (c, addr - TTC_RAM_BASE);
/*		printf ("--%s-- mem %04x read %04x\n", c->name, addr, val);*/

	} else {
		/* Таблица расписания MEDL */
		val = c->medl [addr >> 1];
/*		printf ("--%s-- medl %04x read %04x\n", c->name, addr, val);*/
	}
	return (unsigned short) val;
}

/*
 * Запись 16-битного слова в память.
 * Адрес словный, младший бит равен 0.
 */
void mem_write (controller_t *c, unsigned int addr, unsigned int val)
{
	if (addr & 1) {
		printf ("--%s-- write error: odd address %04x\n", c->name, addr);
		addr &= ~1;
	}
	if (addr >= TTC_REG_BASE) {
/*		printf ("--%s-- reg %04x write %04x\n", c->name, addr, val);*/
		switch (addr) {
		/*
		 * Общесистемные регистры
		 */
		case TTC_SID:		/* schedule identification register */
			if (c->gcr & TTC_GCR_GRUN) {
				printf ("--%s-- error writing SID on GRUN\n", c->name);
				return;
			}
			c->sid &= ~0xffff;
			c->sid |= val & 0xffff;
			break;
		case TTC_SID + 2:
			if (c->gcr & TTC_GCR_GRUN) {
				printf ("--%s-- error writing SID on GRUN\n", c->name);
				return;
			}
			c->sid &= 0xffff;
			c->sid |= (val & 0xffff) << 16;
			break;

		case TTC_NID:		/* node number register */
			if (c->gcr & TTC_GCR_GRUN) {
				printf ("--%s-- error writing NID on GRUN\n", c->name);
				return;
			}
			c->nnr = val;
			break;

		case TTC_GCR:		/* global configuration register */
			if (val & TTC_GCR_GRST) {
				printf ("--%s-- global reset\n", c->name);
				c->gcr = TTC_GCR_GRST;
				c->sid = 0;
				c->nnr = 0;
				c->gsr = 0;
				c->ier = 0;
				c->rier = 0;
				c->cmr = 0;
				c->nmr = 0;
				c->ssr = 0;
				c->scp = 0;
				c->rvec = 0;
				c->avec = 0;
				c->ctr = 0;
				c->cnr = 0;
				c->tdr = 0;
				c->mtr = 0;
				c->nmtr = 0;
				memset (c->minl, 0, sizeof (c->minl));
				memset (c->minu, 0, sizeof (c->minu));
				memset (c->maxl, 0, sizeof (c->maxl));
				memset (c->maxu, 0, sizeof (c->maxu));
				c->vecbits = 16;
				break;
			}
			if ((val & TTC_GCR_GRUN) && ! (c->gcr & TTC_GCR_GRUN)) {
				printf ("--%s-- grun, nmr = %04x\n", c->name, c->nmr);
				c->cmr = c->nmr;

				/* Устанавливаем текущую строку расписания. */
				ctlr_next_slot (c, c->ssr);

				/* Режим старта - загружаем строку приёма. */
				if (c->cmr & TTC_MR_STRT) {
					ctlr_receive_init (c, c->ssrp);
				}
			}
			c->gcr = val;

			if (c->gcr & TTC_GCR_N64)
				c->vecbits = 64;
			else if (c->gcr & TTC_GCR_N32)
				c->vecbits = 32;
			else
				c->vecbits = 16;
			break;

		case TTC_GSR:		/* global status register */
			/* Запись единицы сбрасывает разряд в 0. */
			c->gsr &= ~val;
			break;

		case TTC_IER:		/* interrupt enable register */
			c->ier = val;
			break;

		case TTC_RIER:		/* receive interrupt enable register */
			c->rier = val;
			break;

		case TTC_RSR(0):	/* receive status register */
			/* Запись единицы сбрасывает разряд в 0. */
			c->rsr[0] &= ~val;
			break;
		case TTC_RSR(1):
			c->rsr[1] &= ~val;
			break;
		case TTC_RSR(2):
			c->rsr[2] &= ~val;
			break;

		case TTC_NMR:		/* next mode register */
			c->nmr = val;
			break;

		case TTC_SSR:		/* schedule start register */
			c->ssr = val;
			break;

		case TTC_SSRP:		/* schedule start receive pointer */
			c->ssrp = val;
			break;

		case TTC_TPLR:		/* transmit preamble length register */
			c->tplr = val;
			break;

		case TTC_MTR:		/* max time register */
			c->mtr &= ~0xffff;
			c->mtr |= val & 0xffff;
			break;
		case TTC_MTR + 2:
			c->mtr &= 0xffff;
			c->mtr |= (val & 0xffff) << 16;
			break;

		case TTC_NMTR:		/* next max time register */
			c->nmtr &= ~0xffff;
			c->nmtr |= val & 0xffff;
			break;
		case TTC_NMTR + 2:
			c->nmtr &= 0xffff;
			c->nmtr |= (val & 0xffff) << 16;
			break;

/* TODO: сброс счётчиков RPCi, TPC, RSPCi, EVCi, CECi, CMECi */

		default:
			printf ("--%s-- error writing register %04x\n", c->name, addr);
			val = 0;
		}

	} else if (addr >= TTC_RAM_BASE) {
		/* Память данных */
		ctlr_store_data_word (c, addr - TTC_RAM_BASE, val);
/*		printf ("--%s-- mem %04x write %04x\n", c->name, addr, val);*/

	} else {
		/* Таблица расписания MEDL */
		if (c->gcr & TTC_GCR_GRUN) {
			printf ("--%s-- error writing MEDL %04x on GRUN\n", c->name, addr);
			return;
		}
		c->medl [addr >> 1] = val;
/*		printf ("--%s-- medl %04x write %04x\n", c->name, addr, val);*/
	}
}

/*
 * Выполнение одного шага.
 * Перед вызовом должны быть установлены значения входных портов.
 * После вызова будут установлены значения выходных портов.
 *
 * Есть два вызова: по тактам процессора и по тактам каналов передачи данных.
 */
void ctlr_step_cpu (controller_t *c)
{
	unsigned int val;

	/*printf ("--%s-- step\n", c->name);*/

	/*
	 * Обслуживание обращений от процессора.
	 */
	if (c->wrh || c->wrl) {
		/* Запись в память. */
		if (c->addr & 1) {
			if (c->wrh)
				printf ("--%s-- error: writing high byte to odd address %04x\n",
					c->name, c->addr);
			val = mem_read (c, c->addr & ~1) & 0x00ff;
			val |= (c->datain & 0x00ff) << 8;
			mem_write (c, c->addr & ~1, val);
			/*printf ("--%s-- %04x write byte %02x\n",
				c->name, c->addr, c->datain);*/
		} else {
			if (c->wrh && c->wrl) {
				mem_write (c, c->addr, c->datain);
				/*printf ("--%s-- %04x write %04x\n",
					c->name, c->addr, c->datain);*/
			} else if (c->wrh) {
				val = mem_read (c, c->addr) & 0x00ff;
				val |= c->datain & 0xff00;
				mem_write (c, c->addr, val);
				/*printf ("--%s-- %04x write high byte %02x\n",
					c->name, c->addr, c->datain >> 8);*/
			} else {
				val = mem_read (c, c->addr) & 0xff00;
				val |= c->datain & 0x00ff;
				mem_write (c, c->addr, val);
				/*printf ("--%s-- %04x write low byte %02x\n",
					c->name, c->addr, c->datain);*/
			}
		}
		c->ack = 1;

	} else if (c->rd) {
		/* Чтение из памяти. */
		c->dataout = mem_read (c, c->addr & ~1);
		if (c->addr & 1)
			c->dataout >>= 8;
		/*printf ("--%s-- %04x read %04x\n",
			c->name, c->addr, c->dataout);*/

		c->ack = 1;
	} else
		c->ack = 0;
}

void ctlr_step_rxtx (controller_t *c)
{
	if (! (c->gcr & TTC_GCR_GRUN))
		return;

	/* Приём данных. */
	if (c->cmr & TTC_MR_RXEN0)
		ctlr_receive_step (c, 0, c->rx0);
	if (c->cmr & TTC_MR_RXEN1)
		ctlr_receive_step (c, 1, c->rx1);

	/* Передача данных. */
	if (c->cmr & TTC_MR_TXEN0)
		ctlr_transmit_step (c);
#if 0
	/* Время. */
	++c->ctr;
	if (c->slot_ticks <= 0) {
		/* Конец слота. */
		c->gsr |= TTC_GSR_SF;

		if (c->scp < c->sfr) {
			/* Переход к следующему слоту. */
			ctlr_next_slot (c, c->scp + 16);
			printf ("--%s--%ld/%ld: next slot, ticks=%ld, node=%d, tx bytes=%d, flags=%04x\n",
				c->name, c->cnr, c->ctr, c->slot_ticks, c->slot_node,
				c->tx_bytes, c->tx_flags);
		} else {
			/* Начинаем новый цикл кластера. */
			c->gsr |= TTC_GSR_CCL;
			c->cmr = c->nmr;
			c->ctr = 0;
			++c->cnr;
			ctlr_next_slot (c, c->ssr);
			printf ("--%s-- cycle %ld, cmr=%04x\n",
				c->name, c->cnr, c->cmr);
		}
	}
	--c->slot_ticks;
#else
	/* Время. */
	++c->ctr;
	if (c->ctr == c->mtr) {
		/* Начинаем новый цикл кластера. */
		c->gsr |= TTC_GSR_CCL | TTC_GSR_SF;
		c->cmr = c->nmr;
		c->mtr = c->nmtr;
		c->ctr = 0;
		++c->cnr;
		ctlr_next_slot (c, c->ssr);
		printf ("--%s-- cycle %ld, cmr=%04x\n",
			c->name, c->cnr, c->cmr);

	} else if (c->slot_ticks <= 0) {
		/* Конец слота, переход к следующему. */
		c->gsr |= TTC_GSR_SF;
		ctlr_next_slot (c, c->scp + 16);
		printf ("--%s--%ld/%ld: next slot, ticks=%ld, node=%d, tx bytes=%d, flags=%04x\n",
			c->name, c->cnr, c->ctr, c->slot_ticks, c->slot_node,
			c->tx_bytes, c->tx_flags);
	}
	--c->slot_ticks;
#endif
}

void ctlr_reset (controller_t *c)
{
	const char *name;

	/*printf ("--%s-- reset\n", c->name);*/
	name = c->name;
	memset (c, 0, sizeof (controller_t));
	c->name = name;
	c->tx = 1;
	c->tx_state = TX_STATE_IDLE;
	c->rx_state[0] = RX_STATE_IDLE;
	c->rx_state[1] = RX_STATE_IDLE;
}

controller_t *ctlr_alloc ()
{
	controller_t *c;

	/*printf ("\n--%s-- started\n", c->name);*/
	c = calloc (1, sizeof (*c));
	ctlr_reset (c);
	return c;
}

void ctlr_free (controller_t *c)
{
	/*printf ("--%s-- terminated\n\n", c->name);*/
	free (c);
}

/*
 * Обращение к памяти данных: байтное и словное.
 */
unsigned char ctlr_data_byte (controller_t *c, int addr)
{
	unsigned int byte;

	byte = c->data [addr];
	return byte;
}

void ctlr_store_data_byte (controller_t *c, int addr, int byte)
{
	c->data [addr] = byte;
}

unsigned int ctlr_data_word (controller_t *c, int addr)
{
	unsigned int low, high;

	low = c->data [addr & ~1];
	high = c->data [addr | 1];
	return low | high << 8;
}

void ctlr_store_data_word (controller_t *c, int addr, int word)
{
	c->data [addr & ~1] = word;
	c->data [addr | 1] = word >> 8;
}

void ctlr_or_data_word (controller_t *c, int addr, int word)
{
	c->data [addr & ~1] |= word;
	c->data [addr | 1] |= word >> 8;
}
