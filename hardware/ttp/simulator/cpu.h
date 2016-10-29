struct _cpu_t {
	/* Saved context of subtask. */
	unsigned entry_ebp;
	unsigned entry_ebx;
	unsigned entry_edi;
	unsigned entry_esi;
	unsigned entry_esp;
	unsigned entry_eip;

	/* Saved context of parent process. */
	unsigned return_ebp;
	unsigned return_ebx;
	unsigned return_edi;
	unsigned return_esi;
	unsigned return_esp;
	unsigned return_eip;

	const char *name;
	void (*reset_func) (struct _cpu_t*);
	unsigned reset_eip;

	/* Входы. */
	int rst;
	unsigned int datain;
	int ack;

	/* Выходы. */
	unsigned int dataout;
	unsigned int addr;
	int rd;
	int wrh;
	int wrl;
};
typedef struct _cpu_t cpu_t;

/*
 * Завершение цикла процессора.
 */
void cpu_tick (cpu_t *c);

/*
 * Цикл шины: запись слова в память.
 */
void cpu_write (cpu_t *c, unsigned int addr, unsigned int val);
void cpu_write32 (cpu_t *c, unsigned int addr, unsigned long val);

/*
 * Цикл шины: чтение слова из памяти.
 */
unsigned int cpu_read (cpu_t *c, unsigned int addr);
unsigned long cpu_read32 (cpu_t *c, unsigned int addr);
