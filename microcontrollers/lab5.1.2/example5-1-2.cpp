//
// Пример для платы Uno32 и среды програмирования MPIDE.
// Copyright (C) 2012 Сергей Вакуленко
//
#include <WProgram.h>

// Кнопки на контактах 11 и 12.
const int button1 = 11;
const int button2 = 12;

// 7-сегментный индикатор на контактах 2-9.
const int segm_a = 2;
const int segm_b = 3;
const int segm_c = 4;
const int segm_d = 5;
const int segm_e = 6;
const int segm_f = 7;
const int segm_g = 8;
const int segm_h = 9;

// Время в миллисекундах.
unsigned msec;

// Состояние кнопок.
int button1_was_pressed, button2_was_pressed;

// Моменты нажатия кнопок.
unsigned time1, time2;

void setup ()
{
    // Сигналы от кнопок используем как входы.
    pinMode (button1, INPUT);
    pinMode (button2, INPUT);

    // Сигналы управления светодиодами - выходы.
    pinMode (segm_a, OUTPUT);
    pinMode (segm_b, OUTPUT);
    pinMode (segm_c, OUTPUT);
    pinMode (segm_d, OUTPUT);
    pinMode (segm_e, OUTPUT);
    pinMode (segm_f, OUTPUT);
    pinMode (segm_g, OUTPUT);
    pinMode (segm_h, OUTPUT);

    // Устанавливаем прерывание от таймера с частотой 1000 Гц.
    OpenTimer2 (T2_ON | T2_PS_1_256, F_CPU / 256 / 1000);
    ConfigIntTimer2 (T2_INT_ON | T2_INT_PRIOR_3);
}

// Отображение одной цифры на дисплее
void display (unsigned digit)
{
    static const unsigned pattern[10] = {
        1 + 2 + 4 + 8 + 16 + 32,        // Цифра 0
            2 + 4,                      // Цифра 1
        1 + 2     + 8 + 16      + 64,   // Цифра 2
        1 + 2 + 4 + 8           + 64,   // Цифра 3
            2 + 4          + 32 + 64,   // Цифра 4
        1     + 4 + 8      + 32 + 64,   // Цифра 5
        1     + 4 + 8 + 16 + 32 + 64,   // Цифра 6
        1 + 2 + 4,                      // Цифра 7
        1 + 2 + 4 + 8 + 16 + 32 + 64,   // Цифра 8
        1 + 2 + 4 + 8      + 32 + 64,   // Цифра 9
    };

    if (digit > 9)
        digit = 9;
    unsigned mask = pattern[digit];

    if (mask & 1)   LATDSET = 1 << 8;   // Контакт 2 - сигнал RD8
    else            LATDCLR = 1 << 8;
    if (mask & 2)   LATDSET = 1 << 0;   // Контакт 3 - сигнал RD0
    else            LATDCLR = 1 << 0;
    if (mask & 4)   LATFSET = 1 << 1;   // Контакт 4 - сигнал RF1
    else            LATFCLR = 1 << 1;
    if (mask & 8)   LATDSET = 1 << 1;   // Контакт 5 - сигнал RD1
    else            LATDCLR = 1 << 1;
    if (mask & 16)  LATDSET = 1 << 2;   // Контакт 6 - сигнал RD2
    else            LATDCLR = 1 << 2;
    if (mask & 32)  LATDSET = 1 << 9;   // Контакт 7 - сигнал RD9
    else            LATDCLR = 1 << 9;
    if (mask & 64)  LATDSET = 1 << 10;  // Контакт 8 - сигнал RD10
    else            LATDCLR = 1 << 10;
    if (mask & 128) LATDSET = 1 << 3;   // Контакт 9 - сигнал RD3
    else            LATDCLR = 1 << 3;
}

void loop ()
{
    // Опрашиваем кнопки.
    int button1_pressed = (digitalRead (button1) == LOW);
    int button2_pressed = (digitalRead (button2) == LOW);

    // Если кнопки были нажаты - запоминаем время.
    if (button1_pressed && ! button1_was_pressed)
        time1 = msec;
    if (button2_pressed && ! button2_was_pressed)
        time2 = msec;
    button1_was_pressed = button1_pressed;
    button2_was_pressed = button2_pressed;

    if (button1_pressed && button2_pressed) {
        // Обе кнопки нажаты: показываем разность.
        if (time1 > time2) {
            display (time1 - time2);
        } else {
            display (time2 - time1);
        }
    } else {
        // Отображаем текущее время, десятые доли секунды.
        display (msec / 100 % 10);
    }
    delay (1);
}

// Обработчик прерывания от таймера.
extern "C" {
    __ISR (_TIMER_2_VECTOR, IPL3AUTO)
    void timer2_handler (void)
    {
        // Сбрасываем флаг прерывания.
        mT2ClearIntFlag ();

        // Наращиваем счётчик миллисекунд.
        msec++;
    }
}
