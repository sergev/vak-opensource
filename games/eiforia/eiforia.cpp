#include <conio.h>
#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "help.h"
#include "intro.h"

/* описание возможных ресурсов */
/* и их начальные значения */
long money = 10000; // деньги, руб
long gold = 0;      // золото, кг
long land = 100;    // земля, га
long zerno = 1000;  // зерно, тонн
long krest = 100;   // крестьяне, душ
long guard = 100;   // гвардия, чел

/* текущее состояние ресурсов */
long cur_money, cur_gold, cur_land, cur_zerno, cur_krest, cur_guard;
long build_xram; // построено храмов
long ochki;      // набранные игровые очки

/* средние цены */
long pr_gold = 1000; // руб/кг
long pr_land = 200;  // руб/га
long pr_zerno = 10;  // руб/тонн
long pr_krest = 50;  // руб/душу
long pr_guard = 100; // руб/чел
long sod_guard = 10; // руб/год

/* текущие цены */
long cur_pr_gold;
long cur_pr_land;
long cur_pr_zerno;
long cur_pr_krest;
long cur_pr_guard;

long for_kar;  // выделено денег на караван
long for_xram; // выделено денег на храм
long abs_sod_guard; // абсолютная стоимость содержания в текущем году
long run_krest;   // убежало крестьян в текущем году
long add_krest;   // родилось крестьян в текущем году
long run_guard;   // дезертировало гвардейцев в текущем году
long eat_rat;     // сожрали зерна крысы
long grab_money;  // украл денег визирь
long grab_money2; // украл денег визирь
long grab_gold;   // украдено золота из сокровищницы

/* расход зерна на единицу */
long ed_posev = 1; // на посев на 1 га
long ed_eat = 3;   // на еду на 1 душу (krest+guard)

/* расход зерна в текущем году */
long for_posev; // на посев
long for_eat;   // на еду

long add_zerno;  // абсолютный урожай в текущем году
long urog = 7;   // прирост зерна, средний/га
long fl_urog;    // флаг урожайности
long fl_r;       // флаг революции
long fl_kar;     // флаг каравана
long fl_marry;   // флаг женитьбы
long fl_end;     // флаг окончания
long fl_vis;     // флаг визиря
long fl_mar_war; // флаг войны из-за отказа
long fl_block;   // флаг экономичкеской блокады

void intro();
void prn_sost(long god);
void new_game();
void prn_rule();

void beg_init()
{
    cur_money = money;
    cur_gold = gold;
    cur_land = land;
    cur_zerno = zerno;
    cur_krest = krest;
    cur_guard = guard;
    fl_r = 0;
}

long min(long a, long b)
{
    if (a <= b)
        return a;
    else
        return b;
}

long menu(long c, const char *s, int x, int y)
{
    int i, l, l2;
    char ss[10][255];
    char ch;
    l = 0;
    for (i = 0; i < c; i++) {
        while (s[l] != 39)
            l++;
        l++;
        l2 = 0;
        while (s[l] != 39) {
            ss[i][l2] = s[l];
            l++;
            l2++;
        }
        ss[i][l2] = 0;
        l++;
    }
    for (i = 0; i < c; i++) {
        gotoxy(x, y + i);
        printf("%i. %s.", i + 1, ss[i]);
    }
    do {
        ch = getch();
    } while ((ch < 49) || (ch > 48 + c));
    return ch - 48;
}

int main(int argc, char *argv[])
{
    long n;
    randomize();
    textcolor(7);
    if ((argc <= 1) || (stricmp(argv[1], "no") != 0))
        intro();
    do {
        clrscr();
        n = menu(3, "'Новая Игра' 'Помощь' 'Выход'", 33, 11);
        if (n == 1)
            new_game();
        if (n == 2)
            prn_rule();
    } while (n != 3);
    clrscr();
}

void prn_sost(long god)
{
    if (fl_end == 1)
        printf("\n                   Состояние Ваших дел после %i лет правления.", god);
    else
        printf("\n                   Состояние Ваших дел на %i-й год правления.", god);
    printf("\nНаличность в казне: %li руб.", cur_money);
    printf("\n╔════════════════╤════════════╗");
    printf("\n║    Название    │   Запасы   ║");
    printf("\n╠════════════════╪════════════╣");
    printf("\n║ Золото, кг     │ %-10li ║", cur_gold);
    printf("\n║ Земля, га      │ %-10li ║", cur_land);
    printf("\n║ Зерно, тонн    │ %-10li ║", cur_zerno);
    printf("\n║ Крестьяне, душ │ %-10li ║", cur_krest);
    printf("\n║ Гвардия, чел.  │ %-10li ║", cur_guard);
    printf("\n╚════════════════╧════════════╝");
}

long get_choice()
{
    char ch;
    long n;
    do {
        ch = getch();
    } while ((ch != 'y') && (ch != 'Y') && (ch != 'n') && (ch != 'N'));
    if ((ch == 'y') || (ch == 'Y'))
        n = 1;
    else
        n = 0;
    if (n == 1)
        printf("ДА");
    else
        printf("НЕТ");
    return n;
}

long get_chislo()
{
    char s[255];
    do {
        scanf("%s", s);
    } while ((atol(s) <= 0) && (s[0] != '0'));
    return atol(s);
}

long check_enter(long v_old, long v_new)
{
    while (v_new > v_old) {
        printf("\nУ Вас столько нету. Повторите ввод: ");
        v_new = get_chislo();
    }
    return v_new;
}

void visir_message()
{
    printf("\n\nВаше Величество, прибыл Главный Визирь с докладом.");
    printf("\nВизирь сообщает:");
    printf("\nЖалованье гвардии за прошлый год составило %li рублей.", abs_sod_guard);
    switch (fl_urog) {
    case 0:
        printf("\nСтрашная засуха поразила посевы. Очень неурожайный год.");
        printf("\nСобрано всего %li тонн зерна.", add_zerno);
        break;
    case 1:
        printf("\nУрожайность была низкая. Собрано %li тонн зерна.", add_zerno);
        break;
    case 2:
        printf("\nСредний по урожайности год.");
        printf("\nНаши крестьяне собрали %li тонн зерна.", add_zerno);
        break;
    case 3:
        printf("\nУрожайный год. Собрано %li тонн зерна.", add_zerno);
        break;
    case 4:
        printf("\nПролившиеся вовремя дожди обеспечили невиданно высокий урожай.");
        printf("\nАмбары ломятся от зерна - собрано %li тонн!", add_zerno);
        break;
    }
    if (eat_rat > 0)
        printf("\nПреступная халатность! Крысы сожрали %li тонн зерна!", eat_rat);
    if (add_krest > 0)
        printf("\nЧисло Ваших подданных увеличилось. Родилось %li детей.", add_krest);
    if (run_krest != -1)
        printf("\nВашим крестьянам не хватает земли. Сбежало %li человек.", run_krest);
    if (run_guard != -1) {
        printf("\nНе хватило денег на выплату денежного довольствия Вашей гвардии.");
        printf("\nДезертировало %li солдат.", run_guard);
    }
    if (grab_gold > 0)
        printf("\nСкандал! Из сокровищницы похищено %li кг золота!", grab_gold);
    if (grab_money > 0)
        printf("\nКража! Визирь похитил %li руб. и скрылся!..", grab_money);
}

void make_price()
{
    cur_pr_gold = ((pr_gold * 75) / 100) + (random(50) * pr_gold / 100);
    cur_pr_land = ((pr_land * 75) / 100) + (random(50) * pr_land / 100);
    cur_pr_zerno = ((pr_zerno * 75) / 100) + (random(50) * pr_zerno / 100);
    cur_pr_krest = ((pr_krest * 75) / 100) + (random(50) * pr_krest / 100);
    cur_pr_guard = ((pr_guard * 75) / 100) + (random(50) * pr_guard / 100);
}

void prn_birge()
{
    double f;
    printf("Наличность в казне: %li руб.", cur_money);
    printf("\n╔════════════════╤══════════════╤══════════════╤══════════════╗");
    printf("\n║    Название    │    Запасы    │ Текущая цена │ Текущий курс ║");
    printf("\n╠════════════════╪══════════════╪══════════════╪══════════════╣");
    f = cur_pr_gold * 100 / pr_gold;
    printf("\n║ Золото, кг     │ %-12li │ %-12li │ %5.1lf%%       ║", cur_gold, cur_pr_gold, f);
    f = cur_pr_land * 100 / pr_land;
    printf("\n║ Земля, га      │ %-12li │ %-12li │ %5.1lf%%       ║", cur_land, cur_pr_land, f);
    f = cur_pr_zerno * 100 / pr_zerno;
    printf("\n║ Зерно, тонн    │ %-12li │ %-12li │ %5.1lf%%       ║", cur_zerno, cur_pr_zerno, f);
    f = cur_pr_krest * 100 / pr_krest;
    printf("\n║ Крестьяне, душ │ %-12li │ %-12li │ %5.1lf%%       ║", cur_krest, cur_pr_krest, f);
    f = cur_pr_guard * 100 / pr_guard;
    printf("\n║ Гвардия, чел.  │ %-12li │ %-12li │ %5.1lf%%       ║", cur_guard, cur_pr_guard, f);
    printf("\n╚════════════════╧══════════════╧══════════════╧══════════════╝");
}

void torgovla()
{
    long n, nn, nn2;
    long cur;
    char ch;
    make_price();
    do {
        clrscr();
        prn_birge();
        printf("\n\nВыберите товар, с которым проводить операции:");
        nn = menu(6, "'Золото' 'Земля' 'Зерно' 'Крестьяне' 'Гвардия' 'Выход с биржи'", 1, 13);
        if (nn != 6) {
            printf("\n\nВыберите тип операций");
            switch (nn) {
            case 1:
                printf(" с золотом: ");
                break;
            case 2:
                printf(" с землей: ");
                break;
            case 3:
                printf(" с зерном: ");
                break;
            case 4:
                printf(" с крестьянами: ");
                break;
            case 5:
                printf(" с гвардией: ");
                break;
            }
            nn2 = menu(2, "'Покупать' 'Продавать'", 1, 21);
        }
        switch (nn) {
        case 1:
            if (nn2 == 1) {
                printf("\n\nСколько килограммов золота желаете купить: ");
                cur = get_chislo();
                if (cur * cur_pr_gold > cur_money) {
                    printf("\nУ вас не хватает денег чтобы оплатить покупку!");
                    ch = getch();
                    if (ch == 0)
                        ch = getch();
                } else {
                    cur_gold += cur;
                    cur_money -= cur * cur_pr_gold;
                }
            } else {
                printf("\n\nСколько килограммов золота желаете продать: ");
                cur = get_chislo();
                cur = check_enter(cur_gold, cur);
                cur_gold -= cur;
                cur_money += cur * cur_pr_gold;
            }
            break;
        case 2:
            if (nn2 == 1) {
                printf("\n\nСколько гектаров земли желаете купить: ");
                cur = get_chislo();
                if (cur * cur_pr_land > cur_money) {
                    printf("\nУ вас не хватает денег чтобы оплатить покупку!");
                    ch = getch();
                    if (ch == 0)
                        ch = getch();
                } else {
                    cur_land += cur;
                    cur_money -= cur * cur_pr_land;
                }
            } else {
                printf("\n\nСколько гектаров земли желаете продать: ");
                cur = get_chislo();
                cur = check_enter(cur_land, cur);
                cur_land -= cur;
                cur_money += cur * cur_pr_land;
            }
            break;
        case 3:
            if (nn2 == 1) {
                printf("\n\nСколько тонн зерна желаете купить: ");
                cur = get_chislo();
                if (cur * cur_pr_zerno > cur_money) {
                    printf("\nУ вас не хватает денег чтобы оплатить покупку!");
                    ch = getch();
                    if (ch == 0)
                        ch = getch();
                } else {
                    cur_zerno += cur;
                    cur_money -= cur * cur_pr_zerno;
                }
            } else {
                printf("\n\nСколько тонн зерна желаете продать: ");
                cur = get_chislo();
                cur = check_enter(cur_zerno, cur);
                cur_zerno -= cur;
                cur_money += cur * cur_pr_zerno;
            }
            break;
        case 4:
            if (nn2 == 1) {
                printf("\n\nСколько душ крестьян желаете купить: ");
                cur = get_chislo();
                if (cur * cur_pr_krest > cur_money) {
                    printf("\nУ вас не хватает денег чтобы оплатить покупку!");
                    ch = getch();
                    if (ch == 0)
                        ch = getch();
                } else {
                    cur_krest += cur;
                    cur_money -= cur * cur_pr_krest;
                }
            } else {
                printf("\n\nСколько душ крестьян желаете продать: ");
                cur = get_chislo();
                cur = check_enter(cur_krest, cur);
                cur_krest -= cur;
                cur_money += cur * cur_pr_krest;
            }
            break;
        case 5:
            if (nn2 == 1) {
                printf("\n\nСколько гвардейцев желаете нанять: ");
                cur = get_chislo();
                if (cur * cur_pr_guard > cur_money) {
                    printf("\nУ вас не хватает денег чтобы оплатить покупку!");
                    ch = getch();
                    if (ch == 0)
                        ch = getch();
                } else {
                    cur_guard += cur;
                    cur_money -= cur * cur_pr_guard;
                }
            } else {
                printf("\n\nСколько гвардейцев желаете продать: ");
                cur = get_chislo();
                cur = check_enter(cur_guard, cur);
                cur_guard -= cur;
                cur_money += cur * cur_pr_guard;
            }
            break;
        }
    } while (nn != 6);
}

void choice_zerno()
{
    long n;
    long cur;
    printf("\n\nЖелаете сами распределить расход зерна (y/n): ");
    n = get_choice();
    if (n == 0) {
        for_eat = (cur_krest + cur_guard) * ed_eat;
        for_posev = min(cur_land, cur_krest) * ed_posev;
        if ((for_eat + for_posev) <= cur_zerno) {
            cur_zerno = cur_zerno - for_eat - for_posev;
            printf("\n\nВыделено зерна: на еду - %li тонн, на посев - %li тонн.", for_eat,
                   for_posev);
            printf("\nОставшийся запас в амбарах: %li тонн.", cur_zerno);
        } else {
            printf("\n\nНе могу самостоятельно распределить зерно.");
            printf("\nНе хватает зерна на посев и еду по полной норме.");
            printf("\nПожалуйста, распределите зерно лично.");
            n = 1;
        }
    }
    if (n == 1) {
        printf("\nНапоминаю, Ваше состояние сейчас составляет:");
        printf("\nЗемля - %li га, крестьяне - %li душ, гвардия - %li чел.", cur_land, cur_krest,
               cur_guard);
        printf("\nЗапас зерна в амбарах - %li тонн.", cur_zerno);
        printf("\n\nУкажите расход зерна на еду: ");
        cur = get_chislo();
        cur = check_enter(cur_zerno, cur);
        for_eat = cur;
        cur_zerno -= cur;
        printf("\nУкажите расход зерна на посев: ");
        cur = get_chislo();
        cur = check_enter(cur_zerno, cur);
        for_posev = cur;
        cur_zerno -= cur;
        printf("\nОставшийся запас в амбарах: %li тонн.", cur_zerno);
    }
}

/* 0 - страшная засуха, 1 - плохонькая урожайность, 2 - средняя урожайность
   3 - хорошая урожайность, 4 - невиданно плодородный год */

long make_turn()
{
    long n;
    long a, b;
    double f, f1, f2, f3;
    // обработка (крестьяне+гвардия) - зерно
    f1 = for_eat;
    f2 = ed_eat;
    f3 = cur_krest + cur_guard;
    f = (f1 / f2) / f3;
    printf("\n\nНорма продуктов на год на 1 человека составляет %.1lf%% от необходимой.", f * 100);
    if (f >= 1.)
        printf("\nНарод доволен таким щедрым правитетем.");
    if ((f < 1.) && (f >= 0.7))
        printf("\nКормите народ получше, не то получите РЕВОЛЮЦИЮ...");
    if ((f < 0.7) && (f >= 0.4)) {
        n = random(100);
        if (n < 100 - (f * 100)) {
            printf("\nВы доигрались... Народ не стал терпеть такие унижения и сверг Вас!!!");
            printf("\nНе доводите больше свой народ до РЕВОЛЮЦИИ!!!");
            return 1;
        } else {
            printf("\nНедовольство вами резко возросло. Вы сильно рискуете...");
            printf("\nТолько случай спас Вас в этот раз от РЕВОЛЮЦИИ...");
        }
    }
    if (f < 0.4) {
        printf("\nДа Вы что, издеваетесь?!! Так морить голодом свой народ!..");
        printf("\nПродали бы лишних крестьян, изверг, если прокормить не можете...");
        printf("\nЕстественно, умирающий от голода народ сверг такого тирана...");
        printf("\nПолучите РЕВОЛЮЦИЮ!!!");
        return 1;
    }
    // обработка урожая
    fl_urog = random(5);
    a = min(cur_krest, cur_land);
    b = min(a, for_posev);
    add_zerno = (fl_urog * 2 + 3) * b;
    cur_zerno += add_zerno;
    // обработка крыс
    n = random(100);
    if (n < 20) {
        eat_rat = (random(20) * cur_zerno) / 100;
        cur_zerno -= eat_rat;
    } else
        eat_rat = -1;
    // обработка земля - крестьяне
    if (cur_krest > cur_land) {
        run_krest = random(cur_krest - cur_land);
        cur_krest -= run_krest;
    } else
        run_krest = -1;
    n = random(10) + 6;
    add_krest = (cur_krest * n) / 100;
    cur_krest += add_krest;
    // обработка гвардия - деньги
    abs_sod_guard = (cur_guard + 1) * sod_guard;
    if (abs_sod_guard > cur_money) {
        n = random(10) + 6;
        run_guard = (cur_guard * n) / 100;
        cur_guard -= run_guard;
        abs_sod_guard = (cur_guard + 1) * sod_guard;
        if (abs_sod_guard > cur_money)
            abs_sod_guard = cur_money;
    } else
        run_guard = -1;
    cur_money -= abs_sod_guard;
    // обработка похищения золота
    if (cur_gold > 0) {
        n = random(100);
        if (n < 20) {
            grab_gold = (random(25) * cur_gold) / 100;
            cur_gold -= grab_gold;
        } else
            grab_gold = -1;
    } else
        grab_gold = -1;
    // обработка визирь - деньги
    if (cur_money > 0) {
        n = random(100);
        if (n < 10) {
            grab_money = (random(25) * cur_money) / 100;
            grab_money2 = grab_money;
            cur_money -= grab_money;
            fl_vis = 1;
        } else
            grab_money = -1;
    } else
        grab_money = -1;
    return 0;
}

void prn_ochki(long i)
{
    printf("\nДеньги - %li; ", cur_money / 1000);
    printf("золото - %li; ", cur_gold * 2);
    printf("земля - %li; ", cur_land / 5);
    printf("зерно - %li; ", cur_zerno / 100);
    printf("\nкрестьяне - %li; ", cur_krest / 20);
    printf("гвардия - %li; ", cur_guard / 10);
    printf("новые храмы - %li; ", build_xram * 200);
    printf("время правления - %i.", i * 10);
}

long make_ochki(long i)
{
    ochki = 0;
    ochki += (cur_money / 1000);
    ochki += (cur_gold * 2);
    ochki += (cur_land / 5);
    ochki += (cur_zerno / 100);
    ochki += (cur_krest / 20);
    ochki += (cur_guard / 10);
    ochki += build_xram * 200;
    ochki += i * 10;
    return ochki;
}

long snarad_kar()
{
    long n;
    long cur;
    printf("\n\nЗаморский купец предлагает снарядить караван. Вы согласны (y/n): ");
    n = get_choice();
    if (n == 0)
        return 0;
    printf("\nВ казне - %li руб, сколько на караван: ", cur_money);
    cur = get_chislo();
    cur = check_enter(cur_money, cur);
    printf("Караван отправился за три-девять земель...");
    cur_money -= cur;
    for_kar = cur;
    return 1;
}

void grabeg_kar()
{
    long n, grab;
    n = random(100);
    if (n < 5) {
        printf("\n\nПроизошло ЧП! Ваш караван полностью разграблен бандитами!!!");
        fl_kar = 0;
        for_kar = 0;
    } else {
        n = random(40);
        grab = (for_kar * n) / 100;
        printf("\n\nВнимание, ЧП! Ваш караван ограблен бандитами на сумму %li руб.!!!", grab);
        for_kar -= grab;
    }
}

void pribil_kar()
{
    long prib;
    prib = for_kar * 6;
    printf("\n\nВернулся Ваш караван! Получено прибыли на сумму %li руб.!", prib);
    cur_money += prib;
}

void mitropolit()
{
    long cur, nn;
    double f, f1, f2;
    printf("\n\nМитрополит Вашего государства просит денег на новый храм.");
    printf("\nСколько выделяте (в казне %li руб.): ", cur_money);
    cur = get_chislo();
    cur = check_enter(cur_money, cur);
    for_xram += cur;
    f1 = cur;
    f2 = cur_money;
    f = f1 / f2;
    f *= 100;
    cur_money -= cur;
    if (f <= 1)
        printf("\nТы что, насмехаешься?! Скряга!!! За твою жадность ты сгоришь в аду!");
    if ((f > 1) && (f <= 10))
        printf("\nОпомнись, сын мой! Нельзя же быть таким жадным, это смертный грех!");
    if ((f > 10) && (f <= 20))
        printf("\nНе слишком-то щедры твои приношения, сын мой. Можно было дать и побольше.");
    if ((f > 20) && (f <= 30))
        printf("\nЧто ж, спасибо и на этом. Ваши приношения пойдут на богоугодное дело.");
    if ((f > 30) && (f <= 50))
        printf("\nБлагодарю тебя, сын мой. Твоя щедрость будет оценена по достоинству.");
    if (f > 50)
        printf("\nВо всех храмах страны поют молитвы во славу мудрого и щедрого короля!");
    nn = for_xram / 100000;
    if (nn > 0) {
        for_xram -= nn * 100000;
        build_xram += nn;
        if (nn == 1)
            printf("\nВоздвигнут новый храм.");
        if ((nn >= 2) && (nn <= 4))
            printf("\nВоздвигнуто %li новых храма.", nn);
        if (nn >= 5)
            printf("\nВоздвигнуто %li новых храмов.", nn);
    }
}

void nasledstvo()
{
    long cur;
    long n;
    printf("\n\nУмер Ваш дальний родственник. Вы получили наследство в размере:");
    n = random(90) + 10;
    cur = cur_money * n / 100;
    cur_money += cur;
    printf("\nДеньги - %li руб.", cur);
    n = random(90) + 10;
    cur = cur_gold * n / 100;
    cur_gold += cur;
    printf("\nЗолото - %li кг.", cur);
    n = random(90) + 10;
    cur = cur_land * n / 100;
    cur_land += cur;
    printf("\nЗемля - %li га.", cur);
    n = random(90) + 10;
    cur = cur_zerno * n / 100;
    cur_zerno += cur;
    printf("\nЗерно - %li тонн.", cur);
    n = random(90) + 10;
    cur = cur_krest * n / 100;
    cur_krest += cur;
    printf("\nКрестьяне - %li душ.", cur);
    n = random(90) + 10;
    cur = cur_guard * n / 100;
    cur_guard += cur;
    printf("\nСолдаты - %li чел.", cur);
}

void poimali_visir()
{
    long cur;
    printf("\n\nВаша полиция поймала сбежавшего визиря!");
    printf("\nУ него конфисковано все имущество, а его самого посадили на кол!");
    cur = (random(50) + 50) * grab_money2 / 100;
    printf("\nВ казну возвращено %li руб.", cur);
    cur_money += cur;
    fl_vis = 0;
}

void sosed_marry()
{
    long n;
    long cur;
    long prid_money, prid_gold, prid_land, prid_zerno, prid_krest, prid_guard;
    printf("\n\nСоседний король сватает за Вас свою дочку.");
    printf("\nВ приданное он предлагает:");
    n = random(90) + 10;
    cur = cur_money * n / 100;
    prid_money = cur;
    printf("\nДеньги - %li руб.", cur);
    n = random(90) + 10;
    cur = cur_gold * n / 100;
    prid_gold = cur;
    printf("\nЗолото - %li кг.", cur);
    n = random(90) + 10;
    cur = cur_land * n / 100;
    prid_land = cur;
    printf("\nЗемля - %li га.", cur);
    n = random(90) + 10;
    cur = cur_zerno * n / 100;
    prid_zerno = cur;
    printf("\nЗерно - %li тонн.", cur);
    n = random(90) + 10;
    cur = cur_krest * n / 100;
    prid_krest = cur;
    printf("\nКрестьяне - %li душ.", cur);
    n = random(90) + 10;
    cur = cur_guard * n / 100;
    prid_guard = cur;
    printf("\nСолдаты - %li чел.", cur);
    printf("\n\nВы согласны (y/n): ");
    n = get_choice();
    if (n == 1) {
        cur = (random(40) + 20) * cur_money / 100;
        printf("\n\nПоздравляю. На свадебный пир потрачено %li руб.", cur);
        cur_money -= cur;
        cur_money += prid_money;
        cur_gold += prid_gold;
        cur_land += prid_land;
        cur_zerno += prid_zerno;
        cur_krest += prid_krest;
        cur_guard += prid_guard;
        fl_marry = 1;
    } else {
        printf("\n\nАх так?!! Вы пренебрегаете моим предложением??! Готовьтесь к ВОЙНЕ!");
        fl_mar_war = 1;
    }
}

void begin_war()
{
    char ch;
    long cur, ras;
    long your_men, enemy_men;
    long n, victory;
    cur = random((cur_guard + cur_krest) * 2);
    enemy_men = cur;
    your_men = cur_guard;
    ras = cur - (50 * cur / 100) + random(cur);
    printf("\nРазведка доносит о предполагаемой численности войск врага: %li солдат.", ras);
    printf("\nВаши силы: %li солдат. Объявляете мобилизацию крестьян (y/n)? ", your_men);
    n = get_choice();
    if (n == 1) {
        cur = (random(50) + 50) * cur_krest / 100;
        printf("\nМобилизовано %li человек. В народе растет недовольство!", cur);
        your_men += cur;
    }
    printf("\nЕсть возможность завербовать наемников на время этой войны.");
    printf("\nОдин наемник стоит 100 рублей. Будете вербовать (y/n)? ");
    n = get_choice();
    if (n == 1) {
        printf("\nСколько наемников хотите нанять (в казне - %li руб.): ", cur_money);
        cur = get_chislo();
        cur = check_enter(cur_money / 100, cur);
        your_men += cur;
        cur_money -= cur * 100;
    } else
        printf("\n");
    printf("\nПеред битвой выяснилось точное число войск противника: %li солдат.", enemy_men);
    printf("\nВаши войска составляют %li солдат.", your_men);
    printf("\nНажмите любую клавишу для начала сражения...");
    ch = getch();
    if (ch == 0)
        ch = getch();
    victory = 0;
    n = random(enemy_men + your_men * 2);
    if (n <= your_men * 2)
        victory = 1;
    if (victory == 1) {
        printf("\n\nВы победили! Ваша армия захватила трофеи:");
        n = random(90) + 10;
        cur = cur_money * n / 100;
        cur_money += cur;
        printf("\nДеньги - %li руб.", cur);
        n = random(90) + 10;
        cur = cur_gold * n / 100;
        cur_gold += cur;
        printf("\nЗолото - %li кг.", cur);
        n = random(90) + 10;
        cur = cur_land * n / 100;
        cur_land += cur;
        printf("\nЗемля - %li га.", cur);
        n = random(90) + 10;
        cur = cur_zerno * n / 100;
        cur_zerno += cur;
        printf("\nЗерно - %li тонн.", cur);
        n = random(90) + 10;
        cur = cur_krest * n / 100;
        cur_krest += cur;
        printf("\nКрестьяне - %li душ.", cur);
    } else {
        printf("\n\nВы проиграли... Ваши потери в этой войне:");
        n = random(90) + 10;
        cur = cur_money * n / 100;
        cur_money -= cur;
        printf("\nДеньги - %li руб.", cur);
        n = random(90) + 10;
        cur = cur_gold * n / 100;
        cur_gold -= cur;
        printf("\nЗолото - %li кг.", cur);
        n = random(90) + 10;
        cur = cur_land * n / 100;
        cur_land -= cur;
        printf("\nЗемля - %li га.", cur);
        n = random(90) + 10;
        cur = cur_zerno * n / 100;
        cur_zerno -= cur;
        printf("\nЗерно - %li тонн.", cur);
        n = random(90) + 10;
        cur = cur_krest * n / 100;
        cur_krest -= cur;
        printf("\nКрестьяне - %li душ.", cur);
        n = random(90) + 10;
        cur = cur_guard * n / 100;
        cur_guard -= cur;
        printf("\nСолдаты - %li чел.", cur);
    }
}

void rodilsa_sin()
{
    long cur;
    printf("\n\nУ Вас родился сын! Поздравляю! Ваша династия не угаснет в веках!");
    cur = (random(40) + 20) * cur_money / 100;
    printf("\nНа праздничный банкет по случаю рождения сына потрачено %li руб.");
    cur_money -= cur;
}

void dead_wife()
{
    long cur;
    long n;
    printf("\n\nПрибыл гонец от королевы. Впустить (y/n)? ");
    n = get_choice();
    if (n == 0)
        printf("\nХоть Вы и не приняли гонца, но печальная весть все равно дошла до Вас.");
    printf("\nВеликое несчастье! Умерла королева! Овдовевший монарх безутешен!");
    cur = (random(40) + 20) * cur_money / 100;
    printf("\nНа похороны королевы потрачено %li руб.", cur);
    cur_money -= cur;
    fl_marry = 0;
}

void koroleva_prosit()
{
    long n, cur;
    char ch;
    n = random(100);
    if (n < 15) {
        printf("\n\nПрибыл гонец от королевы. Впустить (y/n)? ");
        n = get_choice();
        if (n == 0)
            return;
        cur = (random(30) + 1) * cur_money / 100;
        printf("\nКоролева просит %li руб. на новое платье. Выделить средства (y/n)? ", cur);
        n = get_choice();
        if (n == 1) {
            printf("\nКоролева благодарит Вас /лично и ОЧЕНЬ горячо... :-) /");
            cur_money -= cur;
        } else
            printf("\nКоролева ЖУТКО на Вас обиделась... Видеть Вас больше не желает...");
        printf("\n\nНажмите любую клавишу...");
        ch = getch();
        if (ch == 0)
            ch = getch();
    } else {
        n = random(100);
        if (n < 15) {
            printf("\n\nПрибыл гонец от королевы. Впустить (y/n)? ");
            n = get_choice();
            if (n == 0)
                return;
            cur = (random(30) + 1) * cur_money / 100;
            printf("\n\nКоролева просит %li руб., чтобы устроить бал. Выделить средства (y/n)? ",
                   cur);
            n = get_choice();
            if (n == 1) {
                printf("\nКоролева благодарит Вас /лично и ОЧЕНЬ горячо... :-) /");
                cur_money -= cur;
            } else
                printf("\nКоролева на Вас обиделась... Хоть и не очень сильно, но все таки...");
            printf("\n\nНажмите любую клавишу...");
            ch = getch();
            if (ch == 0)
                ch = getch();
        }
    }
}

long shaman()
{
    long cur, n, n2, fl;
    printf("\nМестный шаман берется вылечить Вас (с вероятностью 20%%...)");
    cur = (random(40) + 1) * cur_money / 100;
    printf("\nНо за это он требует половину Вашего золота и %li руб.", cur);
    printf("\nВы согласны (y/n)? ");
    if (n == 1) {
        cur_money -= cur;
        cur_gold -= cur_gold / 2;
        n2 = random(100);
        if (n2 < 20) {
            printf("\n\nКолдовство шамана помогло! Вы выздоровели!");
            fl = 0;
        } else {
            printf("\n\nШаман ничем не смог помочь Вам... Король УМЕР!!!");
            fl = 1;
        }
    } else {
        n2 = random(100);
        if (n2 < 5) {
            printf("\n\nСлучилось ЧУДО! Вы победили болезнь и встали со смертного одра!");
            fl = 0;
        } else {
            printf("\n\nБолезнь победила Вас... Король УМЕР!!!");
            fl = 1;
        }
    }
    return fl;
}

long korol_bolen(long i)
{
    long fl, n, n2, cur;
    cur = (random(30) + 1) * cur_money / 100;
    printf("\n\nВы заболели! За лечение лекарь просит %li рублей.", cur);
    printf("\nВы можете выздороветь сами, но можете и не выздороветь...");
    printf("\nБудете лечиться (y/n)? ");
    n = get_choice();
    if (n == 1) {
        cur_money -= cur;
        n2 = random(100);
        if (n2 < 5) {
            printf("\nЛечение не дало результатов. Вы присмерти...");
            fl = shaman();
        } else {
            printf("\nЛечение помогло, Вы благополучно выздоровели...");
            fl = 0;
        }
    } else {
        n2 = random(100);
        if (n2 < i * 2) {
            printf("\nЯ же Вас предупреждал! Болезнь обострилась, Вы присмерти!");
            fl = shaman();
        } else {
            printf("\nВы благополучно исцелились сами...");
            fl = 0;
        }
    }
    return fl;
}

void new_game()
{
    long i, n;
    char ch;
    long cur;
    clrscr();
    beg_init();
    i = 1;
    fl_kar = 0;
    fl_marry = 0;
    fl_end = 0;
    fl_vis = 0;
    build_xram = 0;
    for_xram = 0;

    do {
        clrscr();

        prn_sost(i);
        if (i > 1)
            visir_message();

        if (fl_kar == 5) {
            pribil_kar();
            fl_kar = 0;
        }

        n = random(100);
        if (n < 25) {
            printf("\n\nМеждународный кризис! Торговля невозможна!");
            printf("\nВашему государству объявлена экономическая блокада!");
            fl_block = 1;
            printf("\n\nНажмите любую клавишу...");
            ch = getch();
            if (ch == 0)
                ch = getch();
        } else {
            printf("\n\nЖелаете торговать на бирже (y/n)? ");
            n = get_choice();
            if (n == 1)
                torgovla();
            fl_block = 0;
        }

        if (fl_kar == 0) {
            n = random(100);
            if (n < 25)
                fl_kar = snarad_kar();
        }

        if (fl_kar > 1) {
            n = random(100);
            if (n < 20)
                grabeg_kar();
        }

        if (fl_kar > 0)
            fl_kar++;

        n = random(100);
        if (n < 20) {
            mitropolit();
            printf("\n\nНажмите любую клавишу...");
            ch = getch();
            if (ch == 0)
                ch = getch();
        }

        if (fl_mar_war) {
            printf("\n\nРазозленный отказом жениться на его дочке, соседний король начал ВОЙНУ!");
            begin_war();
            printf("\n\nНажмите любую клавишу...");
            ch = getch();
            if (ch == 0)
                ch = getch();
            fl_mar_war = 0;
        }

        if (cur_guard < 100) {
            n = random(100);
            if (n > cur_guard) {
                printf(
                    "\n\nСоседние короли, видя малочисленность Ваших войск, объявили Вам ВОЙНУ!");
                begin_war();
                printf("\n\nНажмите любую клавишу...");
                ch = getch();
                if (ch == 0)
                    ch = getch();
            }
        } else {
            n = random(100);
            if (n < 30) {
                printf("\n\nЕсть возможность объявить войну одному из соседей. Объявляете? ");
                n = get_choice();
                if (n == 1) {
                    begin_war();
                    printf("\n\nНажмите любую клавишу...");
                    ch = getch();
                    if (ch == 0)
                        ch = getch();
                }
            }
        }

        if (fl_vis == 1) {
            n = random(100);
            if (n < 15) {
                poimali_visir();
                printf("\n\nНажмите любую клавишу...");
                ch = getch();
                if (ch == 0)
                    ch = getch();
            }
        }

        n = random(100);
        if (n < 10) {
            nasledstvo();
            printf("\n\nНажмите любую клавишу...");
            ch = getch();
            if (ch == 0)
                ch = getch();
        }

        if (fl_marry == 1) {
            n = random(100);
            if (n < 10) {
                rodilsa_sin();
                printf("\n\nНажмите любую клавишу...");
                ch = getch();
                if (ch == 0)
                    ch = getch();
            }
        }

        if (fl_marry == 0) {
            n = random(100);
            if (n < 15) {
                sosed_marry();
                printf("\n\nНажмите любую клавишу...");
                ch = getch();
                if (ch == 0)
                    ch = getch();
            }
        }

        if (fl_marry == 1) {
            n = random(100);
            if (n < 10) {
                dead_wife();
                printf("\n\nНажмите любую клавишу...");
                ch = getch();
                if (ch == 0)
                    ch = getch();
            }
        }

        if (fl_marry == 1)
            koroleva_prosit();

        n = random(100);
        if (n < i * 2) {
            fl_end = korol_bolen(i);
            printf("\n\nНажмите любую клавишу...");
            ch = getch();
            if (ch == 0)
                ch = getch();
            if (fl_end)
                break;
        }

        choice_zerno();

        fl_r = make_turn();
        if (fl_r == 1) {
            ch = getch();
            if (ch == 0)
                ch = getch();
            fl_end = 1;
            break;
        }

        printf("\n\nБудете править в следующем году (y/n)? ");
        n = get_choice();
        if (n == 0) {
            fl_end = 1;
            break;
        }

        printf("\n\nБудете устраивать Новогодний Бал (y/n)? ");
        n = get_choice();
        if (n == 1) {
            printf("\nСколько даете на бал (в казне - %li руб.): ", cur_money);
            cur = get_chislo();
            cur = check_enter(cur_money, cur);
            cur_money -= cur;
            if (cur == 0) {
                printf(
                    "Я не понял, Вы устраиваете бал или нет?! Что это за сумма такая - 0 рублей?!");
                printf("\nЕсли денег жалко, то скажите 'НЕТ' сразу и не пудрите мне мозги!!!");
            } else {
                if (fl_marry > 0)
                    printf("Королева благодарит Вас.");
                else
                    printf("Придворные благодарят Вас.");
            }
        }

        i++;
        printf("\n\nНажмите любую клавишу...");
        ch = getch();
        if (ch == 0)
            ch = getch();
    } while (i <= 50);

    if (i > 50)
        i = 50;
    clrscr();
    fl_end = 1;
    if (fl_r == 0)
        printf("Ваше правление завершилось...\n");
    else {
        printf("Голодающий народ ВОССТАЛ и свергнул нерадивого правителя!!!");
        printf("\nПоздравляю Вас, батенька, с РЕВОЛЮЦИЕЙ, ха-ха...\n");
    }
    prn_sost(i);
    printf("\n\nЗа ваше состояние Вам даются следующее количество очков:");
    ochki = make_ochki(i);
    prn_ochki(i);
    printf("\nОбщая сумма Ваших очков: %li", ochki);
    printf("\n\nНу что ж... Поздравляю с успешным (?) окончанием игры.");
    if ((ochki >= 0) && (ochki <= 100))
        printf("\nP.S. Вам бы лучше гусей пасти... Вместо Ваших крестьян...");
    if ((ochki > 100) && (ochki <= 300))
        printf("\nP.S. Для новичка - сойдет... Хотя, конечно, неважно...");
    if ((ochki > 300) && (ochki <= 500))
        printf("\nP.S. Ну, это уже кое-что... Худо-бедно, да ладно...");
    if ((ochki > 500) && (ochki <= 1000))
        printf("\nP.S. Ну вот, кое-что уже получается. Старайтесь дальше...");
    if ((ochki > 1000) && (ochki <= 3000))
        printf("\nP.S. Неплохо, весьма неплохо... Уважаю...");
    if ((ochki > 3000) && (ochki <= 5000))
        printf("\nP.S. Что ж, видно, играть Вы умеете... Весьма недурственно...");
    if ((ochki > 5000) && (ochki <= 10000))
        printf("\nP.S. Круто, что говорить... Да Вы, батенька, профессионал...");
    if ((ochki > 10000) && (ochki <= 100000))
        printf("\nP.S. Прости их, Господи... Ну Вы, блин, даете!!!");
    if (ochki > 100000)
        printf("\nP.S. NO pity, NO mercy, NO REGRET!!!!!!!!!!");
    ch = getch();
    if (ch == 0)
        ch = getch();
}
