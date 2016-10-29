#include "ttc-reg.h"
#include "crc32-ipmce.h"

struct _controller_t {
	const char *name;

	/* Входы. */
	unsigned int datain;
	unsigned int addr;
	int rd;
	int wrh;
	int wrl;
	int rx0;
	int rx1;

	/* Выходы. */
	unsigned int dataout;
	int ack;
	int tx;

	/* Память данных. */
	unsigned char data [TTC_RAM_BYTES / 2];

	/* Таблица расписания MEDL. */
	unsigned short medl [TTC_WORM_BYTES / 2];

	/* Регистры. */
	unsigned long sid;
	unsigned int nnr;
	unsigned int gcr;
	unsigned int gsr;
	unsigned int ier;
	unsigned int rsr [3];
	unsigned int rier;
	unsigned int cmr;
	unsigned int nmr;
	unsigned int ssr;
	unsigned int scp;
	unsigned int ssrp;
	unsigned int tplr;
	unsigned long long rvec;
	unsigned long long avec;
	unsigned long ctr;
	unsigned long cnr;
	unsigned long tdr;
	unsigned long mtr;
	unsigned long nmtr;
	unsigned long minl [3];
	unsigned long minu [3];
	unsigned long maxl [3];
	unsigned long maxu [3];
	unsigned long rpc [3];
	unsigned long tpc;
	unsigned long rspc [3];
	unsigned long evc [3];
	unsigned long cec [3];
	unsigned long cmec [3];

	int vecbits;		/* Разрядность регистров RVEC, AVEC (16, 32, 64) */

	/* Параметры текущего слота. */
	unsigned long slot_ticks;
	unsigned int slot_node;
	unsigned long slot_time;		/* Время начала слота */

	/* Текущее состояние передатчика. */
	unsigned int tx_bytes;
	unsigned int tx_addr;
	unsigned int tx_flags;
	unsigned int tx_pream;

	int tx_state;		/* Состояние конечного автомата кодера */
	int tx_bits;		/* Количество битов в пакете */
	int tx_bitnum;		/* Номер передаваемого бита пакета */
	unsigned long tx_crc;		/* Контрольная сумма */

	/* Текущее состояние приёмников. */
	unsigned int rx_bits;
	unsigned int rx_flags;

	unsigned int rx_addr [3];
	int rx_state [3];	/* Состояние конечного автомата декодера */
	int rx_bitnum [3];	/* Номер принимаемого бита пакета */
	int rx_byte [3];
	int rx_cmode [3];
	unsigned long long rx_vector [3];
	unsigned long rx_crc [3];
	unsigned long rx_time [3];
};
typedef struct _controller_t controller_t;

/*
 * Шаг выполнения.
 */
void ctlr_step_cpu (controller_t *c);
void ctlr_step_rxtx (controller_t *c);

controller_t *ctlr_alloc ();
void ctlr_free (controller_t *c);
void ctlr_reset (controller_t *c);

unsigned char ctlr_data_byte (controller_t *c, int addr);
void ctlr_store_data_byte (controller_t *c, int addr, int byte);
unsigned int ctlr_data_word (controller_t *c, int addr);
void ctlr_store_data_word (controller_t *c, int addr, int word);
void ctlr_or_data_word (controller_t *c, int addr, int word);
