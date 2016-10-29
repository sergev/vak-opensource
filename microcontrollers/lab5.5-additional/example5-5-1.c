#include <peripheral/int.h>

//
// Размер стека для задач: пятьсот слов, или примерно два килобайта.
//
#define STACK_NWORDS    500

//
// Память для стеков задач.
//
int task1_stack [STACK_NWORDS];
int task2_stack [STACK_NWORDS];

//
// Указатели стека для задач.
//
int *task1_stack_pointer;
int *task2_stack_pointer;

//
// Номер работающей задачи.
//
int current_task = 0;

//
// Текущее время в миллисекундах.
//
volatile unsigned time_msec;

//
// Отображение одного сегмента на дисплее
//
void display (int segment, int on)
{
    switch (segment) {
    case 'a':
        if (on) LATDSET = 1 << 8;   // Контакт 2 - сигнал RD8
        else    LATDCLR = 1 << 8;
        break;
    case 'b':
        if (on) LATDSET = 1 << 0;   // Контакт 3 - сигнал RD0
        else    LATDCLR = 1 << 0;
        break;
    case 'c':
        if (on) LATFSET = 1 << 1;   // Контакт 4 - сигнал RF1
        else    LATFCLR = 1 << 1;
        break;
    case 'd':
        if (on) LATDSET = 1 << 1;   // Контакт 5 - сигнал RD1
        else    LATDCLR = 1 << 1;
        break;
    case 'e':
        if (on) LATDSET = 1 << 2;   // Контакт 6 - сигнал RD2
        else    LATDCLR = 1 << 2;
        break;
    case 'f':
        if (on) LATDSET = 1 << 9;   // Контакт 7 - сигнал RD9
        else    LATDCLR = 1 << 9;
        break;
    case 'g':
        if (on) LATDSET = 1 << 10;  // Контакт 8 - сигнал RD10
        else    LATDCLR = 1 << 10;
        break;
    case 'h':
        if (on) LATDSET = 1 << 3;   // Контакт 9 - сигнал RD3
        else    LATDCLR = 1 << 3;
        break;
    }
}

//
// Опрос состояния кнопки.
// Возвращает ненулевое значение, если кнопка нажата.
//
int button_pressed (int button)
{
    switch (button) {
    case '1':
        return ~PORTG >> 8 & 1;     // Контакт 11 - сигнал RG8
    case '2':
        return ~PORTG >> 7 & 1;     // Контакт 12 - сигнал RG7
    }
    return 0;
}

//
// Функция ожидания, с остановом при нажатой кнопке.
//
void wait (unsigned msec, int button)
{
    unsigned t0 = time_msec;

    while (button_pressed (button) ||
           time_msec - t0 < msec)
    {
        // Если нажата указанная кнопка - ждём,
        // пока она не освободится.
    }
}

//
// Первая задача: вращаем нижнее кольцо восьмёрки, сегменты D-E-G-C.
// Функция не должна возвращать управление.
//
void task1()
{
    for (;;) {
        display ('d', 1); wait (100, '1'); display ('d', 0);
        display ('e', 1); wait (100, '1'); display ('e', 0);
        display ('g', 1); wait (100, '1'); display ('g', 0);
        display ('c', 1); wait (100, '1'); display ('c', 0);
    }
}

//
// Вторая задача: вращаем верхнее кольцо восьмёрки, сегменты A-B-G-F.
// Функция не должна возвращать управление.
//
void task2()
{
    for (;;) {
        display ('a', 1); wait (75, '2'); display ('a', 0);
        display ('b', 1); wait (75, '2'); display ('b', 0);
        display ('g', 1); wait (75, '2'); display ('g', 0);
        display ('f', 1); wait (75, '2'); display ('f', 0);
    }
}

//
// Установка начального значения стека для запуска новой задачи.
// Возвращает начальное значение для регистра sp.
//
int *create_task (int start, int *stack)
{
    // Для хранения контекста в стеке выделяется 34 слова.
    stack += STACK_NWORDS - 34 - 4;

    stack [3] = 0;              // Регистр at
    stack [4] = 0;              // Регистр v0
    stack [5] = 0;              // Регистр v1
    stack [6] = 0;              // Регистр a0
    stack [7] = 0;              // Регистр a1
    stack [8] = 0;              // Регистр a2
    stack [9] = 0;              // Регистр a3
    stack [10] = 0;             // Регистр t0
    stack [11] = 0;             // Регистр t1
    stack [12] = 0;             // Регистр t2
    stack [13] = 0;             // Регистр t3
    stack [14] = 0;             // Регистр t4
    stack [15] = 0;             // Регистр t5
    stack [16] = 0;             // Регистр t6
    stack [17] = 0;             // Регистр t7
    stack [18] = 0;             // Регистр s0
    stack [19] = 0;             // Регистр s1
    stack [20] = 0;             // Регистр s2
    stack [21] = 0;             // Регистр s3
    stack [22] = 0;             // Регистр s4
    stack [23] = 0;             // Регистр s5
    stack [24] = 0;             // Регистр s6
    stack [25] = 0;             // Регистр s7
    stack [26] = 0;             // Регистр t8
    stack [27] = 0;             // Регистр t9
    stack [28] = 0;             // Регистр s8
    stack [29] = 0;             // Регистр ra
    stack [30] = 0;             // Регистр hi
    stack [31] = 0;             // Регистр lo
    stack [32] = 0x10000003;    // Регистр Status: биты CU0, EXL, IE
    stack [33] = start;         // Регистр EPC: адрес начала

    return stack;
}

//
// Начальная инициализация.
//
int init()
{
    // Задаём количество тактов ожидания для памяти.
    // Скорость работы процессора увеличится.
    CHECON = 2;
    BMXCONCLR = 0x40;
    CHECONSET = 0x30;

    // Разрешаем кэширование сегмента kseg0, будет еще быстрее.
    // Это задаётся в младших битах регистра Config.
    int config;
    asm volatile ("mfc0 %0, $16" : "=r" (config));
    config |= 3;
    asm volatile ("mtc0 %0, $16" : : "r" (config));

    // Отключаем порт JTAG, чтобы освободить эти ножки для чего-то полезного.
    DDPCON = 0;

    // Переключаем все сигналы порта B в цифровой режим.
    AD1PCFG = ~0;

    //
    // Контроллер прерываний.
    //
    INTCON = 0;                 // Interrupt Control
    IPTMR = 0;                  // Temporal Proximity Timer
    IFS0 = IFS1 = 0;            // Interrupt Flag Status
    IEC0 = IEC1 = 0;            // Interrupt Enable Control

    IPC0 = IPC1 =               // Interrupt Priority Control
    IPC2 = IPC3 =
    IPC4 = IPC5 =
    IPC6 = IPC7 =
    IPC8 = IPC11 = 1<<2 | 1<<10 | 1<<18 | 1<<26;
}

//
// Основная функция программы.
//
int main()
{
    init();

    // Устанавливаем прерывание от таймера с частотой 1000 Гц.
    int compare = F_CPU / 2 / 1000;
    asm volatile ("mtc0 %0, $9" : : "r" (0));
    asm volatile ("mtc0 %0, $11" : : "r" (compare));
    IEC0SET = 1 << _CORE_TIMER_IRQ;

    // Сигналы от кнопок используем как входы.
    TRISGSET = 1 << 8;                      // Кнопка 1 - сигнал RG8
    TRISGSET = 1 << 7;                      // Кнопка 2 - сигнал RG7

    // Сигналы управления 7-сегментным индикатором - выходы.
    TRISDCLR = 1 << 8;  LATDCLR = 1 << 8;   // Сегмент A - сигнал RD8
    TRISDCLR = 1 << 0;  LATDCLR = 1 << 0;   // Сегмент B - сигнал RD0
    TRISFCLR = 1 << 1;  LATFCLR = 1 << 1;   // Сегмент C - сигнал RF1
    TRISDCLR = 1 << 1;  LATDCLR = 1 << 1;   // Сегмент D - сигнал RD1
    TRISDCLR = 1 << 2;  LATDCLR = 1 << 2;   // Сегмент E - сигнал RD2
    TRISDCLR = 1 << 9;  LATDCLR = 1 << 9;   // Сегмент F - сигнал RD9
    TRISDCLR = 1 << 10; LATDCLR = 1 << 10;  // Сегмент G - сигнал RD10
    TRISDCLR = 1 << 3;  LATDCLR = 1 << 3;   // Сегмент H - сигнал RD3

    // Создаём две задачи.
    task1_stack_pointer = create_task ((int) task1, task1_stack);
    task2_stack_pointer = create_task ((int) task2, task2_stack);

    // Разрешаем прерывания.
    asm volatile ("ei");

    for (;;) {
        // Ничего не делаем, ждём прерывания от таймера.
        // После первого же прерывания начинают работать задачи.
    }
}

//
// Обработчик прерывания от таймера.
//
__ISR (_CORE_TIMER_VECTOR, ipl1)
void timer_handler()
{
    // Увеличиваем регистр Compare чтобы получить следующее
    // прерывание еще через миллисекунду.
    int compare;
    asm volatile ("mfc0 %0, $11" : "=r" (compare));
    compare += F_CPU / 2 / 1000;
    asm volatile ("mtc0 %0, $11" : : "r" (compare));

    // Сбрасываем флаг в контроллере прерываний.
    IFS0CLR = 1 << _CORE_TIMER_IRQ;

    // Наращиваем счётчик времени.
    ++time_msec;

    // Запоминаем значение указателя стека для текущей задачи.
    int *sp;
    asm volatile ("move %0, $sp" : "=r" (sp));

    if (current_task == 1) {
        task1_stack_pointer = sp;

    } else if (current_task == 2) {
        task2_stack_pointer = sp;
    }

    // Переключаемся на другую задачу: меняем указатель стека.
    if (current_task == 1) {
        current_task = 2;
        sp = task2_stack_pointer;
    } else {
        current_task = 1;
        sp = task1_stack_pointer;
    }

    // Устанавливаем новое значение стека.
    asm volatile ("move $sp, %0" : : "r" (sp) :
                        "$1","$2","$3","$4","$5","$6","$7","$8","$9",
                        "$10","$11","$12","$13","$14","$15","$16","$17",
                        "$18","$19","$20","$21","$22","$23","$24","$25",
                        "$30","$31","hi","lo","sp");
    // Перечисляем здесь все регистры, которые необходимо сохранять и
    // и восстанавливать из стека при переключении контекста.
    // Компилятор сгенерирует нужные команды.
}
