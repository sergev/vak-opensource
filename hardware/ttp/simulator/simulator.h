/*
 * Эмулятор кластера TTP.
 *
 * Автор: Сергей Вакуленко, ИТМиВТ 2008.
 */
void ui_print (const char *fmt, ...)
	__attribute__ ((__format__ (__printf__, 1, 2)));

#define printf ui_print
