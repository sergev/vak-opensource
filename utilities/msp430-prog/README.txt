=== Утилита MSP430-PROG ===
Утилита MSP430-PROG предназначена для записи программного обеспечения
в память микроконтроллеров Texas Instruments MSP430.

В качестве программатора используется адаптер TI MSP-FET430UIF
или MSP-FETU430IF подключающийся к USB-порту компьютера.


=== Вызов ===

Первый параметр вызова утилиты MSP430-PROG - имя виртуального COM-порта,
к которому подключен программатор. Для Линукса имя выглядит как /dev/ttyUSBi.

При вызове с одним параметром утилита MSP430-PROG определяет тип процессора.
Например:

	C:>msp430-prog COM6
	Programmer for TI MSP430, Version 1.0, Copyright (C) 2009 Serge Vakulenko
	Library: msp430.dll, version 20401000
	Device: MSP430F5436

Для записи в flash-память необходимо в качестве второго параметра вызова
указать имя файла с прошивкой в формате SREC, например:

	C:>msp430-prog.exe COM6 test_eth.srec
	Programmer for TI MSP430, Version 1.0, Copyright (C) 2009 Serge Vakulenko
	Library: msp430.dll, version 20401000
	Device: MSP430F5436
	Memory: 05C00-0B5FF, 0FE00-0FFFF, total 23552 bytes
	Erase: done
	Program: ############################################## done
	Rate: 1389 bytes per second

Для сравнения содержимого flash-памяти с файлом следует вызывать утилиту
с флагом "-v", например:

	C:>msp430-prog.exe -v COM6 test_eth.srec
	Programmer for TI MSP430, Version 1.0, Copyright (C) 2009 Serge Vakulenko
	Library: msp430.dll, version 20401000
	Device: MSP430F5436
	Memory: 05C00-0B5FF, 0FE00-0FFFF, total 23552 bytes
	Verify: ############################################## done
	Rate: 3857 bytes per second


=== Формат SREC ===

Преобразовать формат бинарного файла ELF или COFF или A.OUT
в SREC можно командой objcopy, например:

	objcopy -O srec firmware.elf firmware.srec
___
С уважением,
Сергей Вакуленко
