/*
 * Чтение и проверка номера ревизии контроллера.
 */
void node_check_revision (cpu_t *c);

/*
 * Тест шины между процессором и контроллером.
 */
void node_test_bus (cpu_t *c);

/*
 * Установка номера узла и таблицы расписания MEDL.
 */
void node_setup (cpu_t *c, int node_num);

/*
 * Установка режима кластера.
 */
void node_set_mode (cpu_t *c, int cluster_mode);

/*
 * Установка длительности цикла кластера.
 */
void node_set_cycle_duration (cpu_t *c, int one_cycle_only, signed long duration);

/*
 * Ожидание стартового пакета в течение одного цикла.
 * Возврат 1, если пакет получен.
 */
int node_wait_start_packet (cpu_t *c, int *cluster_mode, int *start_node,
	unsigned long *cluster_time, unsigned long *local_time);

/*
 * Ожидание битов в регистре GSR.
 * Возвращает текущее значение регистра GSR.
 */
int node_wait_gsr (cpu_t *c, int bit);

/*
 * Проверка наступления ожидаемого момента времени.
 */
int node_time_reached (unsigned long slot_time,
	unsigned long current_time, unsigned long prev_time);

/*
 * Извлечение 16-битного значения из контроллера TTP.
 */
int node_get_ushort (cpu_t *c, unsigned short *result,
	unsigned short addr0, unsigned short addr_status0,
	unsigned short addr1, unsigned short addr_status1);

/*
 * Распределение памяти данных кластера.
 */
struct _cluster_data_t {
	/* Приём стартового пакета в режиме старта. */
	unsigned short start0_status;
	unsigned char start0_cmode;
	unsigned char start0_node;
	unsigned long start0_cluster_time;
	unsigned long start0_local_time;

	unsigned short start1_status;
	unsigned char start1_cmode;
	unsigned char start1_node;
	unsigned long start1_cluster_time;
	unsigned long start1_local_time;

	/* Приём стартового пакета в нормальном режиме работы. */
	unsigned short s0_status;
	unsigned char s0_cmode;
	unsigned char s0_node;
	unsigned long s0_cluster_time;
	unsigned long s0_receive_time;

	unsigned short s1_status;
	unsigned char s1_cmode;
	unsigned char s1_node;
	unsigned long s1_cluster_time;
	unsigned long s1_receive_time;

	/* Передача 2-байтового значения X узлом 0. */
	unsigned short x0_status;
	unsigned short x0;

	unsigned short x1_status;
	unsigned short x1;

	/* Передача 2-байтового значения Y узлом 1. */
	unsigned short y0_status;
	unsigned short y0;

	unsigned short y1_status;
	unsigned short y1;
} __attribute__ ((packed));

#define ADDR(name)		(0x4000 + __builtin_offsetof (struct _cluster_data_t, name))

/* Моменты начала и длительность слотов. */
#define SLOT0_TIME	0
#define SLOT0_DURATION	(88*2)
#define SLOT1_TIME	SLOT0_DURATION
#define SLOT1_DURATION	(104*2)
#define SLOT2_TIME	(SLOT1_TIME + SLOT1_DURATION)
#define SLOT2_DURATION	(100*2)

/* Длительность цикла кластера. */
#define CYCLE_DURATION	(SLOT2_TIME + SLOT2_DURATION)
