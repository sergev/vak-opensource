;----------------------------------
; Стандартный заголовок для тестов
;----------------------------------

EXTINT  =       27h             ; ГРВП
INTINT  =       1fh             ; РПР

	.text
	uj      start

x2:     rmod    EXTINT          ; внешние прерывания
	halt    .
x3:     halt    .               ; экстракоды (00) режима 3
x4:     rmod    INTINT          ; внутренние прерывания режима 1, 2
	halt    .
x5:     rmod    INTINT          ; запрещенная команда режима 1, 2
	halt    .
x6:     rmod    INTINT          ; внутренние прерывания режима 3
	halt    .
x7:     rmod    INTINT          ; запрещенная команда режима 3
	halt    .
				; экстракоды (050-077) режима 1, 2
x8:     halt    .               ; Э50
x9:     halt    .               ; Э51
xa:     halt    .               ; Э52
xb:     halt    .               ; Э53
xc:     halt    .               ; Э54
xd:     halt    .               ; Э55
xe:     halt    .               ; Э56
xf:     halt    .               ; Э57
x10:    halt    .               ; Э60
x11:    halt    .               ; Э61
x12:    halt    .               ; Э62
x13:    halt    .               ; Э63
x14:    halt    .               ; Э64
x15:    halt    .               ; Э65
x16:    halt    .               ; Э66
x17:    halt    .               ; Э67
x18:    halt    .               ; Э70
x19:    halt    .               ; Э71
x1a:    halt    .               ; Э72
x1b:    halt    .               ; Э73
x1c:    halt    .               ; Э74
x1d:    halt    .               ; Э75
x1e:    halt    .               ; Э76
x1f:    halt    .               ; Э77

	.globl  __ttyout        ; буфер вывода тестера
__ttyout:
	.word   0

	.globl  __ttyin         ; буфер ввода тестера
__ttyin:
	.word   0

start:
	vtm     _end+1,15       ; область стека
	ntr     200h            ; стандартный режим

; Чистка сегмента bss и установка стека.

	vtm     _edata,14       ; адрес начала bss
loop:   ita     14
	aex     #_end
	uz      cont
	xta
	atx     ,14             ; роспись bss нулями
	vlm     loop,14
cont:

; Закачка фиктивных параметров в стек.

	xts     #1
	xts     #argv           ; "a.out"
	xts     #_environ
	xts     #-3             ; число параметров
	vjm     _main,14        ; main (argc, argv, environ);

stop:
	halt    -1              ; останов
	uj      stop

	.data
	.globl _environ
aout:
	.ascii  "a.out"
_environ:
	.word   0               ; char *environ[];
argv:
	.word   aout            ; char *argv[];
	.word   0

	.globl __ediag
__ediag:
	.word   0               ; диагностмка - по-русски
