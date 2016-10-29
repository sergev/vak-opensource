package stream

/*
 * Интерфеёс: текстовый вывод.
 * Функции должны располагаться в алфавитном порядке,
 * чтобы соответствовать интерфейсам в Си и Си++.
 */
type Stream_t interface {
	Put_Integer (val int, width int, base int)
	Put_String (str string)
	Put_Unsigned (val uintptr, width int, base int)
}
