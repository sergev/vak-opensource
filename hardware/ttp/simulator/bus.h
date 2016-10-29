struct _bus_t {
	const char *name;

	/* Входы. */
	int rn0;
	int rn1;
	int rn2;
	int rn3;

	/* Выходы. */
	int tx;

	int run;
	int idle_counter;
};
typedef struct _bus_t bus_t;

/*
 * Шаг выполнения.
 */
void bus_step (bus_t *c);

bus_t *bus_alloc ();
void bus_free (bus_t *c);
void bus_reset (bus_t *c);
