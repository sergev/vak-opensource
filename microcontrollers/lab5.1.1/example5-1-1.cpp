//
// Пример для платы Uno32 и среды програмирования MPIDE.
// Copyright (C) 2012 Сергей Вакуленко
//
#include <WProgram.h>

// Кнопки на контактах 11 и 12.
const int button1 = 11;
const int button2 = 12;

// Светодиоды на контактах 13 и 43.
const int led1 = 13;
const int led2 = 43;

void setup ()
{
    // Сигналы от кнопок используем как входы.
    pinMode (button1, INPUT);
    pinMode (button2, INPUT);

    // Сигналы управления светодиодами - выходы.
    pinMode (led1, OUTPUT);
    pinMode (led2, OUTPUT);
}

void loop ()
{
    int need_wait = 0;

    // Опрашиваем первую кнопку.
    if (digitalRead (button1) == HIGH) {
        // Не нажата - гасим первый светодиод.
        digitalWrite (led1, LOW);
        need_wait = 1;
    }

    // Опрашиваем вторую кнопку.
    if (digitalRead (button2) == HIGH) {
        // Не нажата - гасим второй светодиод.
        digitalWrite (led2, LOW);
        need_wait = 1;
    }

    // Если надо, подождём.
    if (need_wait)
        delay (150);

    // Зажигаем оба светодиода.
    digitalWrite (led1, HIGH);
    digitalWrite (led2, HIGH);
    delay (50);
}
