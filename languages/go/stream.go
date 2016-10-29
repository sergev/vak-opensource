package main

/*
 * Интерфеёс: двунаправленный байтовый поток данных.
 */
type Stream_t interface {
	Putc(int)			// выдача байта
//	Getc() int			// прием байта
//	Peekc()	int			// опрос первого байта во входящей очереди без удаления
	Flush()				// ожидание опустошения буфера выдачи
//	Eof() bool			// закончился ли входной поток
//	Close()				// окончание выходного потока
}

/*
 * Выдача строки
 */
func Puts(x Stream_t, str string) {
	for _, c := range str {
		x.Putc (int(c))
	}
}

/*
 * Поток выдачи в текстовую строку
 */
type stream_string_t struct {
	buf string
	Stream_t
}

func (x *stream_string_t) Putc(c int) {
	x.buf += string(c)
}

func (x *stream_string_t) Flush() {
	println("Flush string '" + x.buf + "',", len(x.buf), "bytes")
	x.buf = ""
}

func message (x Stream_t, msg string) {
	x.Putc ('Ё')
	Puts (x, msg)
	x.Flush()
}

func main() {
	println("Testing stream interface.")

	x := new (stream_string_t)
	message (x, "abracadabra")
	message (x, "trampampam")
}
