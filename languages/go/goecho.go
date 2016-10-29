package main

import (
	"os"
	"flag" // парсер параметров командной строки
)

var omitNewline = flag.Bool("n", false, "не печатать последнюю линию")

const (
	Space   = " "
	Newline = "\n"
)

func main() {
	flag.Parse() // Сканирование списка аргументов и установка флагов
	var s string = ""
	for ы := 0; ы < flag.NArg(); ы++ {
		if ы > 0 {
			s += Space
		}
		s += flag.Arg(ы)
	}
	if !*omitNewline {
		s += Newline
	}
	os.Stdout.WriteString(s)
}
