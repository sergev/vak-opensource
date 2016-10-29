/*
 * Структура, соответствующая интерфейсу языка Go.
 */
struct Interface_t {
	void *methods;	// Указатель на таблицу функций
	void *object;	// Указатель на структуру данных
};

/*
 * Абстрактный класс: текстовый вывод.
 * Первый элемент требуется для совместимости с языком Go.
 * В действительности он представляет собой не функцию,
 * а указатель на дескриптор типа данных.
 */
class Stream_t {
public:
	virtual void Type_Descriptor (void) {}
	virtual void Put_Integer (long val, int width = 0,
		int base = 10) = 0;
	virtual void Put_String (const char *str, int strlim[2] = 0) = 0;
	virtual void Put_Unsigned (unsigned long val, int width = 0,
		int base = 10) = 0;
};
