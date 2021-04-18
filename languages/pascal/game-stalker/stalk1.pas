(*
Это восстановленный из исполнимого файла код на языке Паскаль для игры,
которая называлась 'Зона' или 'Сталкер', файл игры назывался
STALK.SAV, PIKNIK.SAV или ZONA.SAV и предназначался для советских PDP11-
совместимых машин - таких как ДВК и УКНЦ.
Это игра в жанре рогалик (Rogue-like), бродилка по подземельям, по мотивам
повести Стругацких "Пикник на обочине".
Игра, по-видимому, создана в конце 1980-х, автор игры - неизвестный
программист из Ульяновска - первоначально программа писала копирайт:
*** (С)УЛЬЯНОВСК УЛПИ "ГОЛОГРАФИЯ"***
Данный исходник предназначается для Паскаля ДВК или OMSI PASCAL-1
и компилируется в исполнимый файл под системой RT-11 командами:
    PASCAL STALK1,STALK1=STALK1.PAS
    LINK/STACK:1000 STALK1,FRANDU,PASCAL
*)
PROGRAM STALK;
{$S+}
{$A-}
{$T-}
uses
    math, crt;
type
    charptr = ^char;
type
    intptr = ^integer;
var
    DUNGEON: integer; { Номер подземелья }
    VAR2:   integer;
    VAR4:   integer; { Номер этажа }
    VAR6:   integer; { новый X игрока }
    VAR10:  integer; { новый Y игрока }
    VAR12:  integer; { Позиция Y игрока (строка) }
    VAR14:  integer; { Позиция X игрока (столбец) }
    VAR16:  integer; { переменная цикла }
    i:      integer; { переменная цикла }
    VAR20:  integer; { переменная цикла }
    VAR22:  integer;
    VAR24:  integer;
    VAR26:  integer; { Усталость }
    VAR30:  integer; { Вредность }
    VAR32:  integer; { Заряд батарей фонаря }
    VAR34:  integer; { Y }
    VAR36:  integer; { X }
    VAR40:  integer;
    VAR42:  integer; { Число для генератора случайных чисел }
    VAR46:  boolean;
    VAR47:  boolean; { Фонарь горит или нет }
    VAR50:  boolean; { Приготовился к сражению }
    VAR51:  boolean;
    VAR52:  char;
    VAR53:  char;
    VAR54:  char; { Символ в клетке на которой стоит игрок }
    VAR55:  char;
    IND:    array[1..5] of integer; { Индикаторы справа: }
    { 1 - Рейтинг, 2 - Энергия, 3 - Оружие, 4 - Защита, 5 - В банке денег }
    F:  array[0..8,1..16,1..32] of char;
    V11070: charptr; { Указатель на клетку в F }
    V11072: charptr; { Указатель на клетку в F }
    F2: array[0..8,1..16,1..32] of integer;
    V33074: intptr; { Указатель на клетку в F2 }
    V33076: intptr; { Указатель на клетку в F2 }
    RUKSAK: array[1..6] of char;    {033100..033105}
    V33106: array[1..6] of integer; {033106..033120}
    V33122: array[1..4] of char;    {033122..033125}
    V33126: array[1..4] of integer; {033126..033134}
    V33136: array[1..4] of integer; {033136..033144}
    V33146: array[1..4] of integer; {033146..033154}
    V33156: char;
    TITLE:  array[0..29] of char;

function RANDOM(A,B:integer):integer; {L01046}
begin
    RANDOM := RandomRange(A, B+1);
end;

procedure READCHAR(var CH:char); {L01166}
label 1;
begin
1:
    CH := readkey;
    if CH = CHR(0) then begin
        { функциональная клавиша }
        CH := readkey;
        case CH of
        'H': CH := '8'; { стрелка вверх }
        'P': CH := '2'; { стрелка вниз }
        'K': CH := '4'; { стрелка влево }
        'M': CH := '6'; { стрелка вправо }
        'I': CH := '.'; { page up }
        'Q': CH := '5'; { page down }
        else goto 1;    { неизвестная клавиша - игнорируем }
        end;
    end;

    if (ORD(CH) >= ORD('a')) and (ORD(CH) <= ORD('z')) then
        CH := CHR(ORD(CH) + ORD('A') - ORD('a')); { преобразуем в заглавные буквы }
end;

{ Поставить курсор на позицию (X*2,Y) }
procedure CURSORTO(Y, X:integer); {L01216}
begin
    WRITE(CHR(&33), '[', 1+Y, ';', 1+X+X, 'H');
end;

{ Очистка блока сообщений внизу экрана }
procedure CLEARMSG; {L01320}
begin
    CURSORTO(16,0);
    for VAR16:=1 to 7 do begin
        WRITE(CHR(&12), CHR(&33), '[K'); { стираем до конца строки }
    end;
    CURSORTO(17,0);
end;

procedure L01460(A,B,C:integer);
var I,J:integer;
begin
    for I:=-1 TO 1 do begin
        for J:=-1 TO 1 do begin
            VAR22:=B+I;
            VAR24:=C+J;
            if (F[A,VAR22,VAR24] = '^') and ((F2[A,VAR22,VAR24] and 6) <> 4) then begin
                CURSORTO(VAR22,VAR24);
                WRITE('.');
            end else begin
                F2[A,VAR22,VAR24]:=F2[A,VAR22,VAR24] or &100;
                CURSORTO(VAR22,VAR24);
                WRITE(F[A,VAR22,VAR24]);
            end;
        end;
    end;
end;

{ Поиск предметов в рюкзаке, возвращает индекс 1..6, или 7 если не найдены }
function RUKSEEK(A,B:char):integer; {L02332}
var K:integer;
begin
    K:=1; {L02340}
    while (RUKSAK[K] <> A) and (RUKSAK[K] <> B) and (K < 7) do begin
        K:=K+1;
    end;
    RUKSEEK:=K;
    V33156:=CHR(1);
end;

procedure NELZYA;
begin
    WRITELN(' Н е л ь з я ! ! !');
end;

procedure ZVERX(V,Y,X:integer); {L02574}
var FV:char; FV2:integer;
begin
    FV:=F[V,Y,X];
    FV2:=F2[V,Y,X];
    WRITELN('З в е р ь !');
    if (FV2 and 4) <> 0 then begin
        if (FV <> V33122[1]) and (FV <> V33122[2]) and (FV <> V33122[3]) then begin
            VAR16:=0;
            for VAR20:=1 to 3 do begin
                VAR16:=VAR16+1;
                if V33122[VAR16] = ' ' then
                    exit;
            end;
            V33122[VAR16]:=FV;
            V33126[VAR16]:=FV2;
            V33136[VAR16]:=Y;
            V33146[VAR16]:=X;
        end;
    end;
end;

procedure ZAKLYATIE;
begin
    WRITELN('На этой штуке заклятие');
end;

{ Параметр: 'J' - обновить экран; '.' - вверх; '5' - вниз }
procedure L03362(A:char);
label 1;
begin
    V33156:=CHR(1);
    if A <> 'J' then begin {L03370}
        if V11070^ = '%' then begin { Мы стоим на лестнице? }
            for VAR16:=1 TO 4 do begin
                V33122[VAR16]:=' ';
            end;
            if (A = '.') and (VAR4 <> 0) then begin {L03510}
                VAR4:=VAR4-1; { Этаж вниз } {L03560}
                IND[2]:=IND[2]-2; { Энергия минус 2 }
                IND[1]:=IND[1]+1;
                goto 1;
            end else if VAR4 <> 8 then begin {L03610}
                VAR4:=VAR4+1;
                IND[1]:=IND[1]+3;
                goto 1;
            end;
            begin
                V11070:=@F[VAR4,VAR12,VAR14]; {L03644}
                V33074:=@F2[VAR4,VAR12,VAR14]; {L03716}
                V11072:=@F[VAR4,VAR6,VAR10]; {L04000}
                V33076:=@F2[VAR4,VAR6,VAR10]; {L04052}
            end;
        end else begin { не на лестнице }
            WRITELN(' Без лестницы?'); {L04140}
        end;
        exit; { Выход из процедуры }
    end;
1:
    WRITE(CHR(&33),'[H'); { в начало экрана }
    WRITE(CHR(&33),'[2J'); { стираем экран }
    WRITE('    Подземелье ');
    WRITE(DUNGEON); { Номер подземелья }
    WRITELN('');
    WRITELN('                                                                  Рейтинг');
    WRITELN('                                                                  Энергия');
    WRITELN('                                                                  Оружие');
    WRITELN('                                                                  Защита');
    WRITELN('                                                                  В банке');
    WRITELN('');
    WRITELN('                                                                 Вредность');
    WRITELN('');
    WRITE('                                                                   Рюкзак:');
    for VAR16:=1 to 5 do begin { выводим значения индикаторов }
        CURSORTO(VAR16,37);
        WRITE(IND[VAR16]:3);
        WRITE(' ');
    end;
    CURSORTO(7,37);
    WRITE(VAR30:3); { Вредность }
    CURSORTO(10,33);
    for VAR16:=1 to 6 do begin { выводим содержимое рюкзака }
        WRITE(RUKSAK[VAR16]:2);
    end;
    CURSORTO(0,0);
    for VAR16:=1 to 16 do begin { выводим текущее поле }
        for VAR20:=1 to 32 do begin
            if F2[VAR4,VAR16,VAR20] and (&100) <> 0 then begin
                CURSORTO(VAR16,VAR20);
                WRITE(F[VAR4,VAR16,VAR20]);
            end;
        end;
    end;
    CLEARMSG;
    exit; { выход из процедуры }
end;

procedure L05370(DY,DX:integer);
begin
    if not (F[VAR4,VAR12+DY,VAR14+DX] in ['!','-']) then begin { Если не стенка }
        VAR12:=VAR12+DY; {L05500}
        VAR14:=VAR14+DX;
        V11070:=@F[VAR4,VAR12,VAR14];
        V33074:=@F2[VAR4,VAR12,VAR14];
    end else
        NELZYA;
end;

procedure L05702(A:char);
begin
    VAR12:=RANDOM(2,15);
    VAR14:=RANDOM(2,31);
    F[VAR4,VAR12,VAR14]:=A;
    F2[VAR4,VAR12,VAR14]:=RANDOM(0,32767) and &177677;
end;

label 1, 2;
BEGIN {MAIN}
TITLE[00]:='(';
TITLE[01]:='H';
TITLE[02]:=')';
TITLE[03]:=' ';
TITLE[04]:='H';
TITLE[05]:='a';
TITLE[06]:='c';
TITLE[07]:='k';
TITLE[08]:='e';
TITLE[09]:='d';
TITLE[10]:=' ';
TITLE[11]:='b';
TITLE[12]:='y';
TITLE[13]:=' ';
TITLE[14]:='O';
TITLE[15]:='l';
TITLE[16]:='e';
TITLE[17]:='g';
TITLE[18]:=' ';
TITLE[19]:='H';
TITLE[20]:='.';
TITLE[21]:=' ';
TITLE[22]:=' ';
TITLE[23]:=' ';
TITLE[24]:=' ';
TITLE[25]:=' ';
TITLE[26]:=' ';
TITLE[27]:=' ';
TITLE[28]:=' ';
TITLE[29]:=' ';
WRITE('*** ');
WRITE(TITLE); { Вывод начальной строки}
WRITELN('***');
WRITELN(' ^_^_^_^  __ С Т А Л К Е Р __       '); {место под спецсимволы в строке}
WRITE('загадайте число. может быть, оно вам пригодится... там... ');
{ Ввод числа, которое "может быть пригодится там" }
READLN(VAR42);
RandSeed := VAR42;
VAR34 := 1; { Позиция Y игрока }
VAR36 := 1; { Позиция X игрока }
VAR55 := ' ';

DUNGEON:=1; { Подземелье = 1 }
1:
{ Начало игры, или рестарт на следующем подземелье }
WRITE(CHR(&33),'[H'); { в начало экрана }
WRITE(CHR(&33),'[2J'); { стираем экран }
CURSORTO(10,1);
for VAR4:=0 to 8 do begin { цикл по этажам }
    VAR22:=RANDOM(0,88);
    for VAR20:=1 to 32 do begin
        F[VAR4,1,VAR20]:='-'; { стенка вверху }
    end;
    for VAR16:=2 to 15 do begin
        F[VAR4,VAR16,1]:='!'; { стенка слева }
        for VAR20:=2 to 31 do begin
            F[VAR4,VAR16,VAR20]:='.';
            F2[VAR4,VAR16,VAR20]:=VAR22;
        end;
        F[VAR4,VAR16,32]:='!'; { стенка слева }
    end;
    for VAR20:=1 to 32 do begin
        F[VAR4,16,VAR20]:='-'; { стенка снизу }
    end;
end;
WRITELN('   Для подсказки нажимайте "H" ');
for VAR4:=1 to 8 do begin { цикл по этажам } {L07652}
    VAR16:=RANDOM(2,7); {L07712}
    while VAR16 < 16 do begin {L07736}
        VAR22:=2; {L07752}
        VAR24:=RANDOM(1,7); {L07760}
        while VAR24 < 32 do begin {L10004}
            for VAR20:=VAR22 to VAR24 do begin {L10020}
                F[VAR4,VAR16,VAR20]:='-'; {L10060}
            end;
            VAR22:=RANDOM(1,5)+VAR24; {L10144}
            VAR24:=RANDOM(3,10)+VAR24; {L10174}
        end;
        VAR16:=RANDOM(0,7)+VAR16; {L10230}
    end;
end;
delay(1000);
WRITE(CHR(&15));
WRITE('     Темный  коридор ...     ');
for VAR4:=1 to 8 do begin { цикл по этажам } {L10326}
    VAR20:=RANDOM(2,7); {L10366}
    while VAR20 < 32 do begin {L10412}
        VAR22:=2; {L10426}
        VAR24:=RANDOM(1,7);
        while VAR24 < 16 do begin {L10460}
            for VAR16:=VAR22 to VAR24 do begin {L10474}
                if F[VAR4,VAR16,VAR20] = '.' then begin {L10576}
                    F[VAR4,VAR16,VAR20]:='!'; {L10654}
                end else begin
                    F[VAR4,VAR16,VAR20]:='#'; {L10730}
                end;
            end;
            VAR22:=RANDOM(1,5)+VAR24; {L10752}
            VAR24:=RANDOM(2,7)+VAR24; {L11002}
        end;
        VAR20:=RANDOM(2,5)+VAR20; {L11036}
    end;
end;
delay(1000);
WRITE(CHR(&15));
WRITE('     С к е л е т ы . . .   ');
for VAR20:=1 to DUNGEON do begin {L11136}
    for VAR16:=32 to 127 do begin {L11174}
        VAR4:=RANDOM(0,8);
        L05702(CHR(VAR16)); {L11256}
    end;
end;
delay(1000);
WRITE(CHR(&15));
WRITE(' a-a-a-a-a-a-a-a-a-a-a-a-a . . . . . . .');
for VAR4:=0 to 8 do begin {L11350}
    L05702('^'); {L11406}
    L05702('*');
    L05702('%');
end;
VAR4:=8; {L11452}
L05702('%');
L05702(',');
if DUNGEON = 1 then begin
    for VAR16:=1 to 6 do begin {L11512}
        RUKSAK[VAR16]:='.'; {L11552}
    end;
    RUKSAK[1]:=']';
    RUKSAK[2]:='(';
    RUKSAK[3]:='<';
    for VAR16:=1 to 5 do begin {L11624}
        IND[VAR16]:=0; { Очищаем индикаторы }
    end;
    { Инициализация переменных }
    IND[2]:=25; { Энергия }
    VAR47:=false; { Фонарь не горит }
    VAR51:=false;
    VAR50:=false;
    VAR40:=0;
    VAR32:=400; { Заряд батарей } {L11742}
    VAR26:=0; { Усталость = 0 }
    V33156:=CHR(1);
end;
for VAR16:=1 to 6 do begin {L11762}
    V33106[VAR16]:=RANDOM(0,8191) and &175777; {L12022}
end;
VAR46:=false; {L12074}
for VAR16:=1 to 4 do begin {L12100}
    V33122[VAR16]:=' '; {L12140}
end;
VAR4:=0; { Этаж = 0 } {L12170}
delay(1000);
L03362('J');
CLEARMSG;
WRITE('Прогнивший пол провалился...');
VAR2:=0; {L12226}
VAR12:=2; { Позиция Y игрока = 2 }
VAR14:=2; { Позиция X игрока = 2 }
VAR30:=1;
while true do begin {L12254}
{ Начало игрового цикла }
V11070:=@F[VAR4,VAR12,VAR14]; {L12266}
V33074:=@F2[VAR4,VAR12,VAR14]; {L12340}
for VAR16:=-1 to 1 do begin {L12422}
    for VAR20:=-1 to 1 do begin {L12462}
        VAR6:=VAR12+VAR16; { новый Y }
        VAR10:=VAR14+VAR20; { новый X }
        V11072:=@F[VAR4,VAR6,VAR10]; {L12552}
        V33076:=@F2[VAR4,VAR6,VAR10]; {L12624}
        case V11072^ of {L12706}
        '%':    WRITE('Лестница.');
        '*':    WRITE('Золото.');
        ',':    begin
                if VAR4 = 8 then begin
                    WRITE('Золотой шар!!!');
                    VAR46:=true;
                end else
                    WRITE('Волшебная кирка.');
            end;
        '$','+': WRITELN('Бутылка с надписью "Drink me!"');
        ' ':    begin
                WRITELN('Черная дыра');
                if (VAR16 = 0) and (VAR20 = 0) then begin
                    if VAR46 <> false then begin
                        if VAR4 > 0 then
                            VAR4:=VAR4-1; { Этаж вниз }
                    end else begin
                        if VAR4 < 8 then
                            VAR4:=VAR4+1; { Этаж вверх }
                    end;
                    IND[2]:=IND[2]-3; { Энергия минус 3 }
                    VAR30:=VAR30+7; { Вредность увеличиваем на 7 }
                    L03362('J');
                    WRITELN('Ой, как больно!...');
                end;
            end;
        '=':    WRITE('Кольцо.');
        '^':    if (VAR16 = 0) and (VAR20 = 0) then begin
                WRITELN('Ведьмин студень!');
                IND[2]:=IND[2]-RANDOM(3,15);
                VAR30:=VAR30+RANDOM(0,5);
                V33076^:=4;
            end;
        '#':    if (VAR16 = 0) and (VAR20 = 0) then begin
                if RANDOM(1,2) = 1 then
                    V11070^:='!'
                else
                    V11070^:='-';
            end;
        '(',')': WRITE('Доспехи.');
        '[',']': WRITE('Оружие.');
        '?':    WRITE('Свисток.');
        ':',';': WRITE('Еда!!!');
        '\','/': WRITE('"ВП".');
        '<','>': WRITE('Батареи.');
        '"','''': begin {L14040}
                WRITELN('Комариная плешь');
                VAR12:=VAR6;
                VAR14:=VAR10;
                V11070:=@F[VAR4,VAR12,VAR14];
                V33074:=@F2[VAR4,VAR12,VAR14];
                if VAR46 <> false then
                    V11070^:='.';
            end;
        '&':    WRITE('Папирус.');
        'C','H','J','T': begin {L14300}
                WRITE(CHR(7)); { bell }
                WRITELN('Холодная, скользкая рука схватила вас за ногу ...');
                IND[2]:=IND[2]-RANDOM(2,8); { Энергия уменьшается }
                V11072^:='.';
            end;
        'D','E','F','G','I','K','L','M','N','O','P','Q','R','S','U','V','W','X','Y','Z':
            if (VAR16 = 0) and (VAR20 = 0) then begin
                if (V33074^ and 6) = 0 then begin
                    V33076^:=(V33076^ or 6);
                    WRITELN('Ну, сейчас он вам покажет...')
                end else if (V33076^ and 6) = 2 then begin
                    VAR22:=RANDOM(1,3)-2+VAR12;
                    VAR24:=RANDOM(1,3)-2+VAR14;
                    if not (F[VAR4,VAR22,VAR24] in ['!','-']) then begin
                        VAR53:=V11070^;
                        V11070^:='.';
                        F[VAR4,VAR22,VAR24]:=VAR53;
                        F2[VAR4,VAR22,VAR24]:=V33074^;
                    end;
                end else if not VAR51 then begin {L15100}
                    if not VAR50 then begin {L15120}
                        if IND[4] > 1 then {L15140}
                            IND[4]:=IND[4]-RANDOM(2,15)
                        else
                            IND[2]:=IND[2]-RANDOM(2,15); { Энергия уменьшается }
                        WRITE(CHR(7)); { bell }
                        WRITELN('Защищайтесь же!!!');
                    end else begin
                        IND[3]:=IND[3]-RANDOM(2,11);
                        IND[4]:=IND[4]-2;
                        VAR50:=false;
                        VAR54:='.';
                        WRITE(CHR(7));
                        WRITELN('Готов!');
                        IND[1]:=IND[1]+10;
                        if IND[4] < 0 then begin
                            IND[2]:=IND[2]+IND[4];
                            IND[4]:=0;
                        end;
                        if IND[3] < 0 then begin
                            IND[1]:=IND[1]+IND[3];
                            IND[3]:=0;
                        end;
                        i := 1;
                        while (V33122[i] <> V11070^) and (i < 4) do
                            i := i+1;
                        V33122[i]:=' ';
                        V11070^:='m';
                    end;
                end;
            end else begin
                ZVERX(VAR4,VAR6,VAR10);
            end;
        '0','2','3','4','5','6','7','8','9','{','}': begin {L15616}
                if (VAR16 = 0) and (VAR20 = 0) then begin
                    VAR22:=RANDOM(-7,7);
                    IND[2]:=IND[2]+VAR22;
                    if VAR22 > 0 then begin
                        WRITELN('Недурственно!');
                        V11070^:='z';
                    end else begin
                        WRITELN('Тьфу...');
                        V11072^:='y';
                    end;
                end else begin
                    WRITELN('Свертoк. съедим?');
                end;
            end;
        '!','-','.':; { Стенки и пустая клетка } {L16046}
        'b':    WRITELN('Надпись "Здесь был Вася"(здесь Вася и остался)'); {L16046}
        'c':    WRITE('Груда камней.');
        'd':    WRITELN('Метла бабы яги (сломана)');
        'e':    WRITE('Череп.');
        'f':    WRITELN('Странствующий скелет');
        'g':    WRITELN('Стадо бродячих живых трупов. ');
        'h':    WRITELN('Кусочек Ноева ковчега');
        'i':    WRITELN('Записка:" Зря ты сюда полез, парень... Автор. "');
        'j':    WRITE('Черный ящик.');
        'k':    WRITELN('Лужа машинного масла');
        'l':    WRITELN('Раздолбанный компьютер ZX SPECTRUM - 128.    '); {Пробелы компенсируют нули после строки}
        'm':    WRITE('Дохлый морлок');
        'n':    WRITE('Зуда.');
        'o':    WRITE('Пустышка.');
        'p':    WRITE('Куча сепулек.');
        'r':    WRITELN('Разложившийся труп девушки.     '); {Пробелы компенсируют нули после строки}
        's':    WRITELN('Надпись: "Мы до тебя еще доберемся!!! "  '); {Пробелы компенсируют нули после строки}
        't':    WRITELN('Отрезанная голова предыдущего Сталкера.                  '); {Пробелы компенсируют нули после строки}
        'u':    WRITELN('Зачитанный журнал "АКУШЕРСТВО И ГИНЕКОЛОГИЯ"    '); {Пробелы компенсируют нули после строки}
        'v':    WRITE('Артефакт.');
        'w':    WRITELN('Надпись "Здесь-то мы его и съели"');
        'x':    WRITELN('Окровавленная бензопила');
        'y':    WRITELN('Что-то очень мерзкое');
        'z':    WRITE('Следы пикника.');
        'q':    WRITELN('Кладбище снусмумриков.         '); {L17026} {Пробелы компенсируют нули после строки}
        '~':    WRITE('Черт-те что.');
        '1','A','B': WRITELN('Бродячий торговый автомат');
        '|':    WRITELN('Пережаренный зелюк.');
        '`':    WRITELN('Мышелот (в собственном соку)');
        '@':    begin {L17200}
                WRITE('Призрак.');
                VAR22:=RUKSEEK('.','.');
                if VAR22 < 7 then
                    VAR30:=VAR30+1;
                RUKSAK[VAR22]:='@';
            end;
        else begin {L17302}
                if (VAR16 = 0) and (VAR20 = 0) then begin
                    V11070^:=CHR(RANDOM(32,126));
                    V33074^:=RANDOM(1,8191);
                end else begin
                    WRITELN('Мешок с надписью "Take me!"');
                end;
            end
        end; { case of }
    end; { for VAR20 }
end; { for VAR16 }
VAR54:=V11070^; { Сохраняем символ, на который встал игрок }
V11070^:='@'; {L20006}
if VAR47 <> false then begin { Фонарь горит }
    L01460(VAR4,VAR12,VAR14); {L20026}
end else begin
    CURSORTO(VAR12,VAR14); {L20052}
    WRITE('@'); { игрок }
    V33074^:=V33074^ OR &100; {L20100}
    CURSORTO(VAR34,VAR36); {L20106}
    WRITE(VAR55); {L20122}
end;
V11070^:=VAR54; {L20136}
VAR34:=VAR12; { Позиция Y игрока }
VAR36:=VAR14; { Позиция X игрока }
VAR55:=VAR54;
CURSORTO(1,0); {L20166}
WRITE(VAR4:3); {L20200}
for VAR16:=1 to 5 do begin {L20214}
    CURSORTO(VAR16,37); {L20254}
    WRITE(IND[VAR16]:3); {L20270}
    WRITE(' ');
end;
CURSORTO(7,37); {L20342}
WRITE(VAR30:3); { Вредность } {L20356}
CURSORTO(10,33); { Рюкзак }
if V33156 <> CHR(0) then begin {L20406}
    for VAR16:=1 to 6 do begin
        WRITE(RUKSAK[VAR16]:2); {L20460}
    end;
end;
V33156:=CHR(0); {L20520}
CURSORTO(0,0);
{ Ввод символа - команда игрока }
READCHAR(VAR53); {L20534}
CLEARMSG;
case VAR53 of {L20554}
'1':L05370(1,-1);   { влево-вниз }
'2':L05370(1,0);    { вниз }
'3':L05370(1,1);    { вправо-вниз }
'4':L05370(0,-1);   { влево }
'5':L03362('5');    { вниз по лестнице }
'6':L05370(0,1);    { вправо }
'7':L05370(-1,-1);  { влево-вверх }
'8':L05370(-1,0);   { вверх }
'9':L05370(-1,1);   { вправо-вверх }
'0':VAR26:=0;       { отдохнуть }
'.':L03362('.');    { вверх по лестнице }
'Q':begin { пить }
        VAR16:=RUKSEEK('$','+');
        if VAR16 < 7 then begin
            RUKSAK[VAR16]:='.';
            VAR30:=VAR30-1;
            if (V33106[VAR16] AND 6) = 0 then begin {L21122}
                for VAR16:=2 to 15 do begin
                    for VAR20:=2 to 31 do begin
                        if F[VAR4,VAR16,VAR20] <> '.' then begin
                            CURSORTO(VAR16,VAR20);
                            VAR53:=F[VAR4,VAR16,VAR20]; {L21342}
                            if not (F[VAR4,VAR16,VAR20] in ['!','#','-','.']) then begin
                                VAR53:=CHR(RANDOM(33,126));
                            end;
                            WRITE(VAR53);
                        end;
                    end;
                end;
                CLEARMSG; {L21600}
            end else if (V33106[VAR16] AND 6) = 2 then begin
                IND[2]:=IND[2]+20; { выпил }
                VAR26:=0;
                WRITELN('Чувствуете прилив сил?'); {L21654}
            end else if (V33106[VAR16] AND 6) = 4 then begin
                for VAR16:=2 to 15 do begin {L21734}
                    for VAR20:=2 to 31 do begin
                        if F[VAR4,VAR16,VAR20] <> '.' then begin
                            CURSORTO(VAR16,VAR20); {L22112}
                            WRITE(F[VAR4,VAR16,VAR20]); {L22126}
                            F2[VAR4,VAR16,VAR20]:=F2[VAR4,VAR16,VAR20] OR &100; {L22204}
                        end;
                    end;
                end;
                CLEARMSG;
            end else begin
                VAR40:=20; {L22404}
                WRITELN('Напился - сдай стеклотару!!!!');
            end;
        end else
            WRITELN('Пить нечего');
    end;
'D':begin { выбросить предмет } {L22466}
        WRITE('Что выбросить? ');
        READCHAR(VAR53);
        CLEARMSG;
        VAR16:=RUKSEEK(VAR53,VAR53); { Ищем в рюкзаке }
        if VAR16 < 7 then begin
            if (V33106[VAR16] and &2000) = 0 then begin
                if VAR53 <> '.' then begin
                    VAR30:=VAR30-1; {L22646}
                    RUKSAK[VAR16]:='.';
                    if not (V11070^ in ['"','''']) then begin
                        V11070^:=VAR53;
                        V33074^:=V33106[VAR16];
                    end;
                end;
            end else
                ZAKLYATIE;
        end else
            WRITELN('Нету');
    end;
'Z':begin { перевести деньги в банк на счёт пещеры } {L23010}
        VAR16:=RUKSEEK('*','*'); { Ищем в рюкзаке }
        if VAR16 < 7 then begin
            IND[1]:=IND[1]+1;
            VAR30:=VAR30-1;
            RUKSAK[VAR16]:='.';
            IND[5]:=IND[5]+RANDOM(10,70);
            WRITELN('Там!');
        end else
            WRITELN('Где золото-тo?');
    end;
'S':begin { свистнуть }
        VAR22:=RUKSEEK('?','?'); { Ищем в рюкзаке свисток }
        if VAR22 < 7 then begin
            if (V33106[VAR22] and 6) = 4 then begin
                WRITE(CHR(7)); { bell }
            end else if (V33106[VAR22] and 6) = 2 then begin {L23310}
                for VAR16:=-2 to 2 do begin
                    for VAR20:=-2 to 2 do begin
                        if ((VAR12+VAR16) in [2..15]) and ((VAR14+VAR20) in [2..31]) then begin
                            F[VAR4,VAR12+VAR16,VAR14+VAR20]:=' ';
                        end;
                    end;
                end;
                RUKSAK[VAR22]:='.';
            end else if (V33106[VAR22] and 6) = 0 then begin
                WRITELN('Б А М - М - М ! ! !');
                VAR12:=RANDOM(2,15);
                VAR14:=RANDOM(2,31);
                V11070:=@F[VAR4,VAR12,VAR14];
                V33074:=@F2[VAR4,VAR12,VAR14];
            end else begin
                WRITELN('Уничтожение зверя');
                for VAR16:=1 to 4 do
                    V33122[VAR16]:=' ';
                for VAR16:=-1 to 1 do begin
                    for VAR20:=-1 to 1 do begin
                        if F[VAR4,VAR12+VAR16,VAR14+VAR20] in ['A'..'Z'] then begin
                            F[VAR4,VAR12+VAR16,VAR14+VAR20]:='*';
                        end;
                    end;
                end;
            end;
        end else
            WRITELN('Однакo, свисток нужен');
    end;
'E':begin { поесть }
        VAR16:=RUKSEEK(':',';');
        if VAR16 < 7 then begin
            IND[2]:=IND[2]+18; { Энергия }
            RUKSAK[VAR16]:='.';
            VAR30:=VAR30-1;
            WRITELN('Спасибо!');
        end else
            WRITELN('Еды нет');
    end;
'T':begin { взять предмет, на котором стоишь }
        VAR16:=RUKSEEK('.','.'); { Ищем место в рюкзаке }
        if VAR16 < 7 then begin
            if V11070^ in ['"',''''] then
                NELZYA { Комариную плешь брать нельзя }
            else begin
                WRITELN('Берем');
                VAR30:=VAR30+1;
                RUKSAK[VAR16]:=V11070^;
                V33106[VAR16]:=V33074^;
                V11070^:='.';
            end;
        end else
            WRITELN('Рюкзак полон');
    end;
'P':begin { надеть доспехи }
        VAR16:=RUKSEEK(')','(');
        if VAR16 < 7 then begin
            if ((V33106[VAR16]) and &2000) = 0 then begin
                RUKSAK[VAR16]:='.';
                IND[4]:=IND[4]+11;
                WRITELN('Доспехи надеты');
            end else
                ZAKLYATIE;
        end else
            WRITELN('Нету');
    end;
'L':begin { включить фонарь } {L25436}
        if VAR32 > 0 then begin
            VAR47:=true; { Фонарь горит }
            WRITELN('Фонарь включен');
        end else begin
            WRITELN('Батареи сели.Надо было экономить ... ');
        end;
    end;
'O':begin { выключить фонарь } {L25532}
        VAR47:=false; { Фонарь не горит }
        WRITELN('Фонарь выключен');
    end;
'W':begin { взмахнуть волшебной палочкой }
        VAR16:=RUKSEEK('/','\');
        if VAR16 < 7 then begin
            if ((V33106[VAR16]) and 3) = 3 then begin
                WRITELN('Выпустили до госприемки...');
            end else if ((V33106[VAR16]) and 3) = 2 then begin
                for VAR20:=2 to 31 do begin
                    for VAR22:=2 to 15 do begin
                        CURSORTO(VAR22,VAR20);
                        WRITE(F[VAR4,VAR22,VAR20]);
                        F2[VAR4,VAR22,VAR20]:=F2[VAR4,VAR22,VAR20] or &100;
                    end;
                end;
                RUKSAK[VAR16]:='.';
                CLEARMSG;
                WRITELN('"Да будет свет..."');
            end else if ((V33106[VAR16]) and 3) = 1 then begin
                if VAR46 <> false then begin {L26420}
                    VAR4:=VAR4-3;
                    if VAR4 < 0 then VAR4:=0;
                end else begin
                    VAR4:=VAR4+3;
                    if VAR4 > 8 then VAR4:=8;
                end;
                RUKSAK[VAR16]:='.';
                L03362('J');
                WRITELN('Пока Вы летели сквозь этажи, "ВП" потерялась');
                VAR30:=VAR30-1;
            end else begin {L26570}
                WRITELN('Превращение догоняющего зверя');
                for VAR22:=1 to 3 do begin {L26612}
                    if V33122[VAR22] <> ' ' then begin {L26652}
                        V33122[VAR22]:=' ';
                        F[VAR4,V33136[VAR22],V33146[VAR22]]:=CHR(RANDOM(36,63));
                    end;
                end;
            end;
        end else
            WRITELN('Махать-то нечем !');
    end;
'/','?':begin { переложить вещи в рюкзаке }
        VAR52:=RUKSAK[6];
        VAR20:=V33106[6];
        for VAR16:=1 to 6 do begin
            RUKSAK[7-VAR16]:=RUKSAK[6-VAR16];
            V33106[7-VAR16]:=V33106[6-VAR16];
        end;
        RUKSAK[1]:=VAR52;
        V33106[1]:=VAR20;
        V33156:=CHR(1);
        WRITELN('Перестройка в рюкзаке');
    end;
'F':begin { приготовиться к сражению }
        VAR50:=true;
        WRITELN('У-р-р-р-а-a ! ! !');
    end;
'N':begin { идёт начальник }
        { игнорируем }
    end;
'X':begin { закончить }
        WRITE('Закончить изволите? ');
        READCHAR(VAR53);
        if VAR53 in ['D','Y'] then begin
            WRITELN('А ведь предупреждали...');
            goto 2; { Выход из игрового цикла }
        end;
    end;
'M':begin { приготовить оружие } {L27742}
        VAR16:=RUKSEEK('[',']'); { Ищем в рюкзаке }
        if VAR16 < 7 then begin
            if (V33106[VAR16] and &2000) = 0 then begin
                RUKSAK[VAR16]:='.';
                IND[3]:=IND[3]+15;
                WRITELN('Оружие приготовлено');
            end else
                ZAKLYATIE;
        end else
            WRITELN('Нету!');
    end;
'B':begin { заменить батареи }
        VAR16:=RUKSEEK('<','>');
        if VAR16 < 7 then begin
            if (V33106[VAR16] and &2000) = 0 then begin
                RUKSAK[VAR16]:='.';
                VAR30:=VAR30-1;
                VAR32:=400;
                WRITELN('Батареи заменены');
            end else
                ZAKLYATIE;
        end else
            WRITELN('Батарей нет');
    end;
'J':L03362('J'); { обновить экран }
'R':begin { читать папирус }
        VAR16:=RUKSEEK('&','&');
        if VAR16 < 7 then begin
            if (V33106[VAR16] and 6) = 0 then begin
                for VAR20:=1 to 8 do begin
                    V33106[VAR20]:=V33106[VAR20] and &175777;
                end;
                WRITELN('"APCHXYZZYURR!!!"... Заклятие снято');
            end else if (V33106[VAR16] and 6) = 2 then begin
                WRITELN('Надпись гласит: "Сам дурак"');
            end else if (V33106[VAR16] and 6) = 4 then begin {L30656}
                VAR22:=2; {L30710}
                while VAR22 < 16 do begin {L30716}
                    VAR24:=2;
                    while VAR24 < 32 do begin {L30740}
                        if F[VAR4,VAR22,VAR24] = '%' then begin
                            exit;
                        end else begin
                            VAR24:=VAR24+1;
                        end;
                    end;
                    if F[VAR4,VAR22,VAR24] = '%' then begin
                        exit;
                    end else begin
                        VAR22:=VAR22+1;
                    end;
                end;
                WRITE('Лестница -');
                WRITE(VAR22:4,VAR24:4);
                WRITELN;
            end else begin {L31226}
                VAR22:=1;
                while VAR22 < 16 do begin
                    VAR24:=2;
                    while VAR24 < 32 do begin
                        if F[8,VAR22,VAR24] = ',' then begin
                            exit;
                        end else begin
                            VAR24:=VAR24+1;
                        end;
                    end;
                    if F[8,VAR22,VAR24] = ',' then begin
                        exit;
                    end else begin
                        VAR22:=VAR22+1;
                    end;
                end;
                if (VAR22 = 16) and (VAR24 = 32) then
                    WRITELN('Золотой шар украден!')
                else begin
                    WRITE('Золотой шар - 8');
                    WRITE(VAR22:4,VAR24:4);
                    WRITELN;
                end;
            end;
        end else
            WRITELN('А читать-то и нечего');
    end;
'I':begin { надеть кольцо }
        VAR16:=RUKSEEK('=','=');
        if VAR16 < 7 then begin
            if (V33106[VAR16] and &2000) = 0 then begin
                WRITELN('Силовое поле включено!');
                VAR51:=true;
                RUKSAK[VAR16]:='.';
            end else
                ZAKLYATIE;
        end else
            WRITELN('Нету');
    end;
'V':begin { снять кольцо }
        if VAR51 <> false then begin
            VAR16:=RUKSEEK('.','.');
            if VAR16 < 7 then begin
                RUKSAK[VAR16]:='=';
                V33106[VAR16]:=&2000;
                VAR51:=false;
            end else
                WRITELN('Рюкзак полон');
        end else
            WRITELN('Кольца нет');
    end;
'A':begin { купить (на золото) } {L32222}
        if V11070^ in ['1','A','B'] then begin
            if IND[5] > 0 then begin  { В банке есть деньги? }
                WRITE('Чего изволите?');
                READCHAR(VAR53);
                VAR16:=RUKSEEK('.','.');
                if VAR16 < 7 then begin
                    RUKSAK[VAR16]:=VAR53;
                    V33106[VAR16]:=RANDOM(1,8191);
                    IND[5]:=IND[5]-RANDOM(10,250);
                    WRITE(CHR(&15));
                    WRITELN('П о л у ч и т е !');
                    V11070^:='k';
                end;
            end else begin
                WRITELN('Подаю только по пятницам!');
            end;
        end else
            WRITELN('Подойди ближе к автомату!');
    end;
'K':begin { сломать стенку киркой } {L32600}
        VAR16:=RUKSEEK(',',',');
        if VAR16 < 7 then begin
            WRITE('Направление? ');
            READCHAR(VAR53);
            CLEARMSG;
            VAR22:=VAR12;
            VAR24:=VAR14;
            case VAR53 of
            '1':begin VAR22:=VAR12+1; VAR24:=VAR14-1; end;
            '2':VAR22:=VAR12+1;
            '3':begin VAR22:=VAR12+1; VAR24:=VAR14+1; end;
            '4':VAR24:=VAR14-1;
            '5':V11070^:=' ';
            '6':VAR24:=VAR14+1;
            '7':begin VAR22:=VAR12-1; VAR24:=VAR14-1; end;
            '8':VAR22:=VAR12-1;
            '9':begin VAR22:=VAR12-1; VAR24:=VAR14+1; end;
            '.':if VAR4 > 0 then begin
                    F[VAR4-1,VAR12,VAR14]:=' ';
                    WRITELN('Кусок свода обрушился и раскололся о вашу глупую голову');
                end;
            else
                NELZYA
            end; { case of }
            if (VAR22 in [2..15]) and (VAR24 in [2..31]) and (F[VAR4,VAR22,VAR24] in ['!','-']) then begin
                F[VAR4,VAR22,VAR24]:='c';
                IND[2]:=IND[2]-1;
            end else
                WRITELN('Ну, чего размахался?');
        end else
        WRITELN('А стенку вы будете лбом прошибать?..');
    end;
'Y':begin { зажарить убегающего зверя } {L33724}
        if ((V33074^ AND 6) = 2) and (V11070^ in ['A'..'Z']) then
            V11070^:=':' { Зверь становится едой }
        else
            NELZYA; {L34036}
    end;
'H':begin { хелп } {L34046}
        WRITE(CHR(&33),'[H'); { в начало экрана }
        WRITE(CHR(&33),'[2J'); { стираем экран }
        WRITELN('Ладно, я кое-что подскажу. Итак: здесь творится черт знает что,');
        WRITELN('но на восьмом уровне лежит золотой шар. Только достав его, Вы');
        WRITELN('сможете выйти из подземелья, придя на то же место, откуда вы вышли');
        WRITELN('вначале. Своим глазам не всегда стоит доверять!');
        WRITELN(' Вы можете использовать команды:');
        WRITELN('A - Купить (на золото)');
        WRITELN('B - Заменить батареи');
        WRITELN('D - Выбросить предмет');
        WRITELN('E - Поесть');
        WRITELN('F - Приготовиться к сражению');
        WRITELN('H - HELP (этот текст)');
        WRITELN('I - Надеть кольцо');
        WRITELN('J - Обновить экран');
        WRITELN('K - Сломать стенку (киркой)');
        WRITELN('L - Включить фонарь');
        WRITELN('M - Приготовить оружие');
        WRITELN('N - Клавиша "Идет начальник" (отбой тревоги - "P")');
        WRITELN('O - Выключить фонарь');
        WRITELN('P - Надеть доспехи');
        WRITELN('Q - Пить');
        WRITELN('R - Читать папирус');
        WRITELN('S - Свистнуть');
        WRITELN('T - Взять предмет, на клтором стоишь');
        WRITE('U - Вызвать джинна (только в безнадежном случае!)...    Дальше? ');
        READCHAR(VAR53);
        if VAR53 = CHR(&15) then begin
            READCHAR(VAR53);
        end;
        WRITELN('');
        WRITELN('V - Снять кольцо');
        WRITELN('W - Взмахнуть волшебной палочкой');
        WRITELN('X - Закончить');
        WRITELN('Y - Зажарить убегающего зверя');
        WRITELN('Z - Перевести деньги в банк на счет пещеры.');
        WRITELN('/ - Переложить вещи в рюкзаке');
        WRITELN('   П Е Р Е Д В И Ж Е Н И Е:');
        WRITELN('');
        WRITELN('    7 8 9');
        WRITELN('    4   6   - Движение по уровню');
        WRITELN('    1 2 3');
        WRITELN('5 - Вниз по лестнице');
        WRITELN('. - Вверх по лестнице');
        WRITELN('0 - Отдыхать.');
        WRITELN('');
        WRITELN('Использовать можно лишь вещи, лежащие в рюкзаке.');
        WRITELN('Примечание:');
        WRITELN('    Волшебная кирка вынесет вас из "комариной плеши" при ударе ей вниз.');
        WRITELN('');
        WRITE('Ну, что, пойдем дальше? ');
        READCHAR(VAR53);
        if VAR53 = CHR(&15) then begin
            READCHAR(VAR53);
        end;
        L03362('J'); { Обновить экран }
        IND[1]:=IND[1] - 2; { Рейтинг }
    end;
'U':begin { вызвать джинна }
        if (not VAR46) <> false then begin
            WRITELN('Что, влип? ладно, попробую тебя  перенести');
            WRITELN('отсюда. только дороговато это встанет...');
            WRITE('Ты готов? ');
            READCHAR(VAR53);
            if VAR53 <> 'N' then begin
                for VAR16:=1 to 5 do begin
                    IND[VAR16]:=IND[VAR16]-RANDOM(0,15);
                end;
                for VAR16:=1 to 6 do begin {L34504}
                    RUKSAK[VAR16]:='.';
                end;
                VAR12:=RANDOM(2,15);
                VAR14:=RANDOM(2,31);
                VAR22:=RANDOM(0,8);
                V11070:=@F[VAR4,VAR12,VAR14];
                V33074:=@F2[VAR4,VAR12,VAR14];
                if VAR22 > VAR4 then
                    VAR22:=VAR4;
                VAR4:=VAR22;
                L03362('J'); { Обновить экран }
            end;
        end else
            WRITELN('Джинн в отгуле');
    end;
else
    WRITELN('Что-что?')
end; { case of } {L35274}
if VAR40 > 0 then begin
    VAR22:=VAR12+RANDOM(1,3)-2;
    VAR24:=VAR14+RANDOM(1,3)-2;
    if (VAR22 in [2..15]) and (VAR24 in [2..31]) then begin
        VAR12:=VAR22;
        VAR14:=VAR24;
        VAR40:=VAR40-1;
    end;
    V11070:=@F[VAR4,VAR12,VAR14];
    V33074:=@F2[VAR4,VAR12,VAR14];
end;
VAR20:=1; {L35650}
VAR26:=VAR26+VAR30; {L35656}
if VAR47 <> false then { Фонарь горит }
    VAR32:=VAR32-1; { Уменьшаем заряд батарей }
VAR2:=VAR2+((IND[3]+IND[4]) div 10);
if VAR2 > 200 then begin
    VAR2:=0;
    IND[1]:=IND[1]+1;
    IND[2]:=IND[2]-1;
end;
if IND[2] < 5 then
    WRITELN('Силы на исходе'); {L35774}
if IND[2] > 50 then IND[2]:=50; {L36032}
if (VAR47 and (VAR32 < 10)) and (VAR32 > 1) then
    WRITELN('Фонарь гаснет'); {L36122}
if VAR32 = 1 then begin
    IND[1]:=IND[1]-1;
    VAR47:=false; { Фонарь не горит }
    VAR32:=0; { Заряд батарей }
end;
if VAR26 > 200 then
    WRITELN('Отдохнуть-бы');
if VAR26 > 215 then begin { Слишком устал } {L36232}
    IND[2]:=IND[2]-3;
    VAR26:=50; {L36254}
end;
for VAR16:=1 to 4 do begin {L36262}
    if V33122[VAR16] <> ' ' then begin {L36322}
        if (V33136[VAR16] <> VAR12) or (V33146[VAR16] <> VAR14) then begin {L36344}
            if VAR12 > V33136[VAR16] then {L36434}
                VAR22:=V33136[VAR16]+1 {L36460}
            else if VAR12 < V33136[VAR16] then {L36506}
                VAR22:=V33136[VAR16]-1; {L36532}
            if VAR14 > V33146[VAR16] then {L36554}
                VAR24:=V33146[VAR16]+1 {L36600}
            else if VAR14 < V33146[VAR16] then {L36626}
                VAR24:=V33146[VAR16]-1; {L36652}
            if (F[VAR4,VAR22,VAR24] in ['!','-']) and ((V33126[VAR16] and 6) = 4) then {L36674}
                V33122[VAR16]:=' '
            else begin
                F[VAR4,V33136[VAR16],V33146[VAR16]]:='.';
                CURSORTO(V33136[VAR16],V33146[VAR16]);
                WRITE('.');
                F[VAR4,VAR22,VAR24]:=V33122[VAR16];
                CURSORTO(VAr22,VAR24);
                WRITE(V33122[VAR16]);
                F2[VAR4,VAR22,VAR24]:=V33126[VAR16];
                V33136[VAR16]:=VAR22;
                V33146[VAR16]:=VAR24;
                CURSORTO(18,0);
                WRITE(CHR(&33), '[K'); { стираем до конца строки }
            end;
        end;
    end;
end;
if IND[2] < 0 then begin { Умер от недостатка Энергии } {L37536}
    VAR16:=RANDOM(10,1000);
    WRITE(' Вот Вы и стали');
    WRITE(VAR16:4);
    WRITELN('-ой жертвой этого подземелья.');
    goto 2; { Выходим из игрового цикла }
end;
{ Энергия пока есть - продолжаем } {L37660}
if VAR4 = 0 then begin
    if VAR46 <> false then begin
        if (VAR12 = 2) and (VAR14 = 2) then begin
            if IND[5] >= 0 then begin { В банке есть деньги? }
                WRITELN('Как, Вы вернулись?! Ну и ну !!!');
                WRITE('А дальше пойдете? ');
                READCHAR(VAR53);
                if VAR53 = 'Y' then begin
                    DUNGEON:=DUNGEON+1; { следующее подземелье }
                    IND[1]:=IND[1]+50; { Рейтинг }
                    goto 1; { Рестарт игры }
                end;
                goto 2; { Выходим из игрового цикла }
            end else
                WRITELN('А расплачиваться кто будет?');
        end;
    end;
end;
{ конец игрового цикла }
end;
2:
{ Расчёт счёта игрока }
VAR16:=IND[1]+((IND[2]+IND[3]+IND[4]+IND[5] div 5) div 3);
WRITE('Ваш счет -');
WRITELN(VAR16:5);
END.
