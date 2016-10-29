/*
 * Формат пакетов пользователя для платы Seismonet XYZ-E10.
 *
 * Автор: Сергей Вакуленко, <serge@vak.ru>, сентябрь-декабрь 2009.
 */
#ifndef MSP430
#pragma pack(2)
#endif

/*
 * Данные, полученные от приёмника GPS.
 */
typedef struct _gps_report_t {
	unsigned short r41_counter;
	unsigned short r46_counter;
	unsigned short r4b_counter;
	unsigned short r56_counter;
	unsigned short r6d_len;
	unsigned short r6d_counter;
	unsigned short r82_counter;
	unsigned short r84_counter;

	/* GPS time */
#define REPORT_41_LEN	10
	unsigned char r41_data [REPORT_41_LEN];

	/* Health of receiver */
#define REPORT_46_LEN	2
	unsigned char r46_data [REPORT_46_LEN];

	/* Machine code/status */
#define REPORT_4B_LEN	3
	unsigned char r4b_data [REPORT_4B_LEN];

	/* Velocity fix */
#define REPORT_56_LEN	20
	unsigned char r56_data [REPORT_56_LEN];

	/* All-in-view satellite selection */
#define REPORT_6D_LEN	17
	unsigned char r6d_data [REPORT_6D_LEN + 16];

	/* SBAS fix mode */
#define REPORT_82_LEN	1
	unsigned char r82_data [REPORT_82_LEN];

	/* Position fix */
#define REPORT_84_LEN	36
	unsigned char r84_data [REPORT_84_LEN];

} gps_report_t;

/*
 * Пакет статистики для пользователя.
 */
typedef struct _xyze10_stat_t {
	unsigned char location [40];
	unsigned char macaddr [6];
	unsigned long msec;
	unsigned long pps;
	unsigned long free_bytes;

	unsigned long eth_intr;
	unsigned long eth_out_packets;
	unsigned long eth_out_errors;
	unsigned long eth_out_collisions;
	unsigned long eth_in_packets;
	unsigned long eth_in_errors;
	unsigned long eth_in_discards;

	unsigned long lapb_out_packets;
	unsigned long lapb_out_bytes;
	unsigned long lapb_out_discards;
	unsigned long lapb_in_bytes;
	unsigned long lapb_in_packets;
	unsigned long lapb_in_errors;
	unsigned long lapb_in_discards;
	unsigned long lapb_in_overruns;
	unsigned long lapb_in_framing_errors;
	unsigned long lapb_in_crc_errors;

	gps_report_t gps;
} xyze10_stat_t;

#ifndef MSP430
#pragma pack()
#endif
