/*
 * Startup code for standalone system
 * Non-relocating version -- for programs which are loaded by boot
 * Relocating version for boot
 */

EXTINT  =       27h             ; ГРВП
INTINT  =       1fh             ; РПР
STACK   =       3e000h          ; область стека

sp      =       15              ; регистр стека
ap      =       12              ; регистр базирования параметров
rt      =       14              ; регистр адреса возврата

	.text

# ifdef REL
x1:     uj      entry-RELOC+1
# else
x1:     uj      start
# endif

x2:     rmod    EXTINT          ; внешние прерывания
	halt    2
x3:     halt    3               ; экстракоды (00) режима 3
x4:     rmod    INTINT          ; внутренние прерывания режима 1, 2
	halt    4
x5:     rmod    INTINT          ; запрещенная команда режима 1, 2
	halt    5
x6:     rmod    INTINT          ; внутренние прерывания режима 3
	halt    6
x7:     rmod    INTINT          ; запрещенная команда режима 3
	halt    7

; экстракоды (050-077) режима 1, 2

x8:     halt    8               ; Э50
x9:     halt    9               ; Э51
xa:     halt    0ah             ; Э52
xb:     halt    0bh             ; Э53
xc:     halt    0ch             ; Э54
xd:     halt    0dh             ; Э55
xe:     halt    0eh             ; Э56
xf:     halt    0fh             ; Э57
x10:    halt    10h             ; Э60
x11:    halt    11h             ; Э61
x12:    halt    12h             ; Э62
x13:    halt    13h             ; Э63
x14:    halt    14h             ; Э64
x15:    halt    15h             ; Э65
x16:    halt    16h             ; Э66
x17:    halt    17h             ; Э67
x18:    halt    18h             ; Э70
x19:    halt    19h             ; Э71
x1a:    halt    1ah             ; Э72
x1b:    halt    1bh             ; Э73
x1c:    halt    1ch             ; Э74
x1d:    halt    1dh             ; Э75
x1e:    halt    1eh             ; Э76
x1f:    halt    1fh             ; Э77

# ifdef REL

entry:  .globl  entry           ; move itself to address RELOC
	vtm     _edata,13
	vtm     _edata-RELOC+1,14
jloop:  ktga    -1,14
	atgk    -1,13
	vim     jloop-RELOC+1,14

	uj      start           ; jump to new place

start:
	vtm     0x3fff0,13      ; роспись области связи с тестером
:       do      15
	ztx     ,13++

	vtm     STACK,sp        ; область стека
	mtj     14,15           ; область стека
	xta
	ntr     400h            ; тег 0
:       do      4095            ; 32 K
	atgk    1,14            ; роспись области стека

	utm     1,sp            ; область стека
	ntr     200h            ; стандартный режим

# else /* REL */

start:
	ntr     200h            ; стандартный режим
	ita     rt
	atx     ret

# endif /* REL */

; Чистка сегмента bss

	vtm     _edata,14       ; адрес начала bss
	vtm     _end,13         ; адрес конца bss
	jsm     14,13           ; 1 - длина bss
	xta
	ntr     400h            ; тег 0
:       atgx    _end,14
	vlm     .,14

; Закачка фиктивных параметров в стек.

	xts     #0              ; число параметров
	vjm     _main,rt        ; main (argc, argv, environ);

; int _rtt ();  -- return from standalone program

__rtt:  .globl  __rtt
# ifdef REL
	uj      start           ; _rtt () from boot
# else
	uj      [ret]           ; _rtt () from standalone program
# endif

	.globl  _wcopy
_wcopy:                 ; wcopy (int *a, int *b, int sz);
	atc             ; перепись массивов из a в b
	utm     ,sp     ; pop args
	ita     ap
	mtj     ap,sp   ; указатель на 1 аргумент
	utm     3,sp    ; область аргументов
	its     1
	xts     0,ap    ; a
	ati     1
	ita     2
	xts     1,ap    ; b
	ati     2
:       wtc     2,ap    ; sz-1
	do      -1      ; sz-1
:       xta     ,1++
	atx     ,2++
	sti
	sti     2
	sti     1
	sti     ap
	utm     -3,sp   ; выталкивание аргументов
	uj      ,rt

	.globl  _wcmp
_wcmp:                  ; wcmp (int *a, int *b, int sz);
	atc             ; сравнение массивов
	utm     ,sp     ; pop args
	ita     ap
	mtj     ap,sp   ; указатель на 1 аргумент
	utm     3,sp    ; область аргументов
	its     1
	xts     0,ap    ; a
	ati     1
	ita     2
	xts     1,ap    ; b
	ati     2
	ita     3
	xts     2,ap    ; sz
	ati     3
:       xta     ,1++
	aex     ,2++
:       ui      .+2
	utm     -1,3
:       vpm     .-2,3
:       stx     2,ap    ; return (0);
	sti     3
	sti     2
	sti     1
	sti     ap
	utm     -3,sp   ; выталкивание аргументов
	uj      ,rt

	.globl  _wzero
_wzero:                 ; wzero (int *p, int sz);
	atc             ; обнуление массива
	utm     ,sp     ; pop args
	ita     ap
	mtj     ap,sp   ; указатель на 1 аргумент
	utm     2,sp    ; область аргументов
	its     1
	xts     0,ap    ; p
:       ati     1
	wtc     1,ap
:       do      -1      ; sz-1
	ztx     ,1++
	sti
	sti     1
	sti     ap
	utm     -2,sp   ; выталкивание аргументов
	uj      ,rt

	.globl  _clrtg
_clrtg:                 ; clrtg (int buf [131]);
	atc             ; обнуление тега
	utm     ,sp     ; pop args
	ita     ap
	mtj     ap,sp   ; указатель на 1 аргумент
	utm     1,sp    ; область аргументов
	its     2
	its     1
	xts     0,ap    ; &buf
	ati     1       ; &buf
	ati     2       ; &buf
:       ntr     400h    ; тег 0
	do      130
:       xta     ,1++
	atgx    ,2++
	sti
	sti     1
	sti     2
	sti     ap
	utm     -1,sp   ; выталкивание аргументов
	uj      ,rt

	.globl  _crc
_crc:                   ; int crc (int buf [128]);
	atc             ; подсчет контр. суммы
	utm     -1,sp   ; pop args
	ita     rt
	wtc     1,sp
	vtm     ,rt
	its
:       do      127
	ark     1,rt
	wtc     ,sp
	uj

	.globl  _inverword
_inverword:
	atc
	utm     -1,sp   ; pop args
	xta     1,sp
	asn     1024+32
	yta     10h

	atx     ,sp
	aln     1024+16
	aax     #0xffff0000ffff0000
	stx     ,sp
	aax     #0xffff0000ffff0000
	asn     1024+16
	aex     1,sp

	atx     ,sp
	aln     1024+8
	aax     #0xff00ff00ff00ff00
	stx     ,sp
	aax     #0xff00ff00ff00ff00
	asn     1024+8
	aex     1,sp

	uj      ,rt

	.globl  _inverhword
_inverhword:
	atc
	utm     -1,sp   ; pop args
	xta     1,sp
	asn     1024+32
	yta     10h

	atx     ,sp
	aln     1024+16
	aax     #0xffff0000ffff0000
	stx     ,sp
	aax     #0xffff0000ffff0000
	asn     1024+16
	aex     1,sp

	uj      ,rt

	.data
ret:    .word
