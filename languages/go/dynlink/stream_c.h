/*
 * Структура, соответствующая интерфейсу языка Go.
 */
typedef struct _Stream_t {
	struct _Stream_call *call;	// Указатель на таблицу функций
	struct _Stream_t *data;		// Указатель на структуру данных
} Stream_t;

/*
 * Таблица функций интерфейса.
 * Первый элемент требуется для совместимости с языком Go.
 * В действительности он представляет собой указатель на
 * дескриптор типа данных.
 */
struct _Stream_call {
	void *Type_Descriptor;
	void (*Put_Integer) (Stream_t *s, long val, int width, int base);
	void (*Put_String) (Stream_t *s, const char *str);
	void (*Put_Unsigned) (Stream_t *s, unsigned long val, int width, int base);
};
