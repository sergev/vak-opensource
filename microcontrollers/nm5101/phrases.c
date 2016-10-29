/*
 * Copyright (C) 2005 Serge Vakulenko, <vak@cronyx.ru>
 *
 * This file is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can redistribute this file and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software Foundation;
 * either version 2 of the License, or (at your discretion) any later version.
 * See the accompanying file "COPYING" for more details.
 */
#include "io.h"
#include "io2313.h"

/* Была ли нажата кнопка. */
unsigned char button_pressed;
unsigned char button_disabled;

/* Массив указателей на картинки - в отдельном файле. */
extern unsigned char *pic_table [];

/*
 * Микросекундная задержка - для кварца 4 MHz.
 */
void usleep (unsigned char usec)
{
	do {
		/* Нужно четыре такта на цикл.
		 * Три такта уходит на декремент и переход,
		 * плюс одна холостая команда. */
		asm volatile ("nop");
	} while (--usec);
}

/*
 * Миллисекундная задержка.
 */
void msleep (unsigned char msec)
{
	do {
		usleep (250);
		usleep (250);
		usleep (250);
		usleep (250);
	} while (--msec);
}

/*
 * Отображение одного рисунка в течении нескольких миллисекунд.
 */
void frame (unsigned char *data, int msec)
{
	unsigned char i;

	/* Горят только те светодиоды, для которых
	 * в строке 0 и в столбце 1 (PORTB=0, PORTD=1). */
	while (msec > 0) {
		for (i=0; i<7; ++i) {
			/* Выдаем строку номер i в течение 1 миллисекунды. */
			outb (~(1 << i), PORTB);
			outb (readb (data + i), PORTD);
			msleep (1);
		}
		msec -= 7;

		/* Опрос кнопки переключения узоров - вход B7. */
		if (testb (7, PINB)) {
			/* Кнопка не нажата. */
			if (button_disabled)
				button_disabled = 0;
		} else {
			/* Кнопка нажата. */
			if (! button_disabled) {
				button_pressed = 1;
				return;
			}
		}
	}
}

/*
 * Отображение последовательности рисунков.
 */
void show (unsigned char *data)
{
	while (readb (data+6) != 0xff) {
		/* Каждый кадр отображается 125 миллисекунд. */
		frame (data, 125);
		if (button_pressed)
			return;
		++data;
	}
}

int main ()
{
	/* Номер рисунка. */
	int n = 0;

	/*
	 * Устанавливаем направление портов ввода-вывода:
	 * D0-D6 - выход - столбцы 1-7.
	 * B0-B6 - выход - строки 7-1.
	 * B7 - вход - кнопка переключения узоров.
	 */
	outb (0x7f, DDRB);
	outb (0x7f, DDRD);

	for (;;) {
		show (pic_table [n]);

		/* Если была нажата кнопка - меняем рисунок. */
		if (button_pressed) {
			++n;
			if (! pic_table [n])
				n = 0;

			/* После переключения запрещаем реакцию на кнопку,
			 * пока она не будет отжата. Чтобы не было
			 * двойного срабатывания. */
			button_disabled = 1;
			button_pressed = 0;
		}
	}
}
