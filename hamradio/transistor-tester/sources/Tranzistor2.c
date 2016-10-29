/*
 * Tranzistor.c
 *
 * Created: 23.02.2013 10:31:48
 *  Author: Nickita
 */ 

#define F_CPU 16000000UL //16 MHz
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>
#include "global.h"	
#include "a2d.h"
#include "i2c.h"
//Создаём константы	
#define LENGTH_CEL 12 //Длина массива array_cel
#define LENGTH_CEL2 11 //Длина массива array_cel
#define LENGTH_DISPLAY 14 //Длина массива array_display
#define WIDTH 12 //Ширина массивов array_display и array_cel
#define LENGTH_SYM_H21 5 //Длина массива sym_h21
#define	LENGTH_SYM_MOSFET 8 //Длина массива sym_MOSFET

	//Создеём массивы для хранения названий транзисторов array_display и array_cel
	//Массив который хранит названия транзисторов типа: З-С-И N-канал и З-С-И P-канал
	unsigned char array_display [WIDTH][LENGTH_DISPLAY] ={
	//Некоторые буквы имеют свой код: к-123; а-225; н-124; л-91; З-72; И-73; С-латиницей
	{0b01000000, 72, '-'+128, 'C'+128, '-'+128, 73, ' '+128, 'N'+128, '-'+128, 123, 225, 124, 225, 91},
	{0b01000000, 72, '-'+128, 73, '-'+128, 'C'+128, ' '+128, 'P'+128, '-'+128, 123, 225, 124, 225, 91},
	{0b01000000, 72, '-'+128, 73, '-'+128, 'C'+128, ' '+128, 'N'+128, '-'+128, 123, 225, 124, 225, 91},
	{0b01000000, 72, '-'+128, 'C'+128, '-'+128, 73, ' '+128, 'P'+128, '-'+128, 123, 225, 124, 225, 91},
	{0b01000000, 'C'+128, '-'+128, 72, '-'+128, 73, ' '+128, 'N'+128, '-'+128, 123, 225, 124, 225, 91},
	{0b01000000, 73, '-'+128, 72, '-'+128, 'C'+128, ' '+128, 'P'+128, '-'+128, 123, 225, 124, 225, 91},
	{0b01000000, 73, '-'+128, 72, '-'+128, 'C'+128, ' '+128, 'N'+128, '-'+128, 123, 225, 124, 225, 91},
	{0b01000000, 'C'+128, '-'+128, 72, '-'+128, 73, ' '+128, 'P'+128, '-'+128, 123, 225, 124, 225, 91},
	{0b01000000, 'C'+128, '-'+128, 73, '-'+128, 72, ' '+128, 'N'+128, '-'+128, 123, 225, 124, 225, 91},
	{0b01000000, 73, '-'+128, 'C'+128, '-'+128, 72, ' '+128, 'P'+128, '-'+128, 123, 225, 124, 225, 91},
	{0b01000000, 73, '-'+128, 'C'+128, '-'+128, 72, ' '+128, 'N'+128, '-'+128, 123, 225, 124, 225, 91},
	{0b01000000, 'C'+128, '-'+128, 73, '-'+128, 72, ' '+128, 'P'+128, '-'+128, 123, 225, 124, 225, 91}};
	
	//Массив который хранит названия транзисторов типа: К-Б-Э N-P-N
	unsigned char array_cel [WIDTH] [LENGTH_CEL] ={
	//Некоторые буквы имеют свой код: Б-68; К-203; Э-104
	{0b01000000,68, '-'+128, 203, '-'+128, 104, ' '+128, 'N'+128, '-'+128, 'P'+128, '-'+128, 'N'+128},//1
	{0b01000000,68, '-'+128, 104, '-'+128, 203, ' '+128, 'P'+128, '-'+128, 'N'+128, '-'+128, 'P'+128},//2
	{0b01000000,68, '-'+128, 104, '-'+128, 203, ' '+128, 'N'+128, '-'+128, 'P'+128, '-'+128, 'N'+128},//3
	{0b01000000,68, '-'+128, 203, '-'+128, 104, ' '+128, 'P'+128, '-'+128, 'N'+128, '-'+128, 'P'+128},//4
	{0b01000000,203, '-'+128, 68, '-'+128, 104, ' '+128, 'N'+128, '-'+128, 'P'+128, '-'+128, 'N'+128},//5
	{0b01000000,104, '-'+128, 68, '-'+128, 203, ' '+128, 'P'+128, '-'+128, 'N'+128, '-'+128, 'P'+128},//6
	{0b01000000,104, '-'+128, 68, '-'+128, 203, ' '+128, 'N'+128, '-'+128, 'P'+128, '-'+128, 'N'+128},//7	
	{0b01000000,203, '-'+128, 68, '-'+128, 104, ' '+128, 'P'+128, '-'+128, 'N'+128, '-'+128, 'P'+128},//8
	{0b01000000,203, '-'+128, 104, '-'+128, 68, ' '+128, 'N'+128, '-'+128, 'P'+128, '-'+128, 'N'+128},//9
	{0b01000000,104, '-'+128, 203, '-'+128, 68, ' '+128, 'P'+128, '-'+128, 'N'+128, '-'+128, 'P'+128},//10
	{0b01000000,104, '-'+128, 203, '-'+128, 68, ' '+128, 'N'+128, '-'+128, 'P'+128, '-'+128, 'N'+128},//11
	{0b01000000,203, '-'+128, 104, '-'+128, 68, ' '+128, 'P'+128, '-'+128, 'N'+128, '-'+128, 'P'+128}};//12

	unsigned char sym_MOSFET[LENGTH_SYM_MOSFET] = {0b01000000, 'M'+128, 'O'+128, 'S'+128, 'F'+128, 'E'+128, 'T'+128, ' '+128};
	unsigned char sym_h21[LENGTH_SYM_H21] = {0b01000000, 'h'+128, '2'+128, '1'+128, '='+128};
	//Сохраняем номера партов с которыми будем работать в зависимости от типа транзистора
	unsigned char pins_bmm[WIDTH] = {3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5};
	unsigned char pins_bm[WIDTH] = {7, 7, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5};
	unsigned char pins_bs[WIDTH] = {0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2};
	//Сохраняем какой контакт на каком круге будем использовать
	unsigned char pins_polev[WIDTH]	= {1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3};
	
	int h21_1 = 0, h21_2 = 0;//Значение ячеек памяти cell1 и cell2
	int diode (unsigned char, unsigned char );				
	unsigned char  step(unsigned char, unsigned char ,unsigned char,char *, char *);//Функция определения транзистора
	int Unit1 (unsigned char ,unsigned char ); //Функция расчёта H21
	
//Функция инициализации дисплея
void InitLCD() 
{
	unsigned char initseq[11]	={0b00000000, 0b00110100, 0b00001100, 0b00000110, 
							  0b00110101, 0b00000011, 0b00000100, 0b00001000,
							  0b00010000, 0b01000010, 0b10000000 | 23};
	i2cInit();
	_delay_us(100);
	sbi(DDRD,7);
	sbi(PORTD, 7);
	_delay_ms(1);
	cbi(PORTD, 7);
	_delay_ms(1);
	i2cSetBitrate(400);	
	i2cMasterSendNI(0x76,11,initseq);
};
//Функция вывода числа на дисплей
void INTtoLCD(int val)
{
	char val_char[]={"_____"};
	itoa(val,val_char,10); //Преобразуем число в чаровый массив
	
	unsigned char lenght_number; 
	lenght_number = strlen(val_char);//Вычисляем его длину
	
	unsigned char sym[6]	={0b01000000, val_char[0]+128, val_char[1]+128, val_char[2]+128, 
							val_char[3]+128, val_char[4]+128};
	i2cMasterSendNI(0x76,lenght_number+1,sym); //Выводим нужное количество символов
}

//Функция вывода числа на дисплей
void DOUBLEtoLCD(double val)
{
	char val_char[]={"____"};
	dtostrf(val,4,1,val_char); //Преобразуем число в чаровый массив
	
	unsigned char lenght_number; 
	lenght_number = strlen(val_char);//Вычисляем его длину
	
	unsigned char sym[6]	={0b01000000, val_char[0]+128, val_char[1]+128, val_char[2]+128, 
							val_char[3]+128, val_char[4]+128};
	i2cMasterSendNI(0x76,lenght_number+1,sym); //Выводим нужное количество символов
}

void TwoDOUBLEtoLCD(double val1,double val2)
{
	char val_char1[]={"____"};
	dtostrf(val1,4,2,val_char1); //Преобразуем число в чаровый массив

	unsigned char lenght_number1; 
	lenght_number1 = strlen(val_char1);//Вычисляем его длину
	
	char val_char2[]={"____"};
	dtostrf(val2,4,2,val_char2); //Преобразуем число в чаровый массив

	unsigned char lenght_number2; 
	lenght_number2 = strlen(val_char2);//Вычисляем его длину
	
	unsigned char sym[16]	={0b01000000, 0xD5, 0xB1, 0xBD, val_char1[0]+128, val_char1[1]+128, val_char1[2]+128, val_char1[3]+128,
							32, 
							0xD5, 0xB2, 0xBD, val_char2[0]+128,val_char2[1]+128,val_char2[2]+128,val_char2[3]+128};
	i2cMasterSendNI(0x76,16,sym); //Выводим нужное количество символов
}


//Функция перехода на вторую строку дисплея
void LCD_Address(){
	unsigned char sym[3] = {0b00000000,0b00110100,0b10000000 | 0x40};
	i2cMasterSendNI(0x76,3,sym);
}

int main(void)
{
	

	// отключаем подтяжки на всех пинах контроллера
	SFIOR|=(1<<PUD);
	//Настройка работы АЦП
	a2dInit();
	a2dSetPrescaler(ADC_PRESCALE_DIV32);
	a2dSetReference(ADC_REFERENCE_AVCC);
	//Создание массива импульсов
	unsigned char outs[12];
	outs[0]=35;//1,0,0,0,1,1
	outs[1]=35;//1,0,0,0,1,1
	outs[2]=44;//1,0,1,1,0,0
	outs[3]=44;//1,0,1,1,0,0
	outs[4]=11;//0,0,1,0,1,1
	outs[5]=11;//0,0,1,0,1,1
	outs[6]=56;//1,1,1,0,0,0
	outs[7]=56;//1,1,1,0,0,0
	outs[8]=14;//0,0,1,1,1,0
	outs[9]=14;//0,0,1,1,1,0
	outs[10]=50;//1,1,0,0,1,0
	outs[11]=50;//1,1,0,0,1,0

	char cell1[LENGTH_CEL], cell2[LENGTH_CEL];//Ячейки памяти
	unsigned char InSB;//Флаг проверки нажатия кнопки
	unsigned char in1, in2;//В эти переменные считываем значения с А(4) А(5)
	
	while(1)
    {
		//Обнуление
		h21_1 = 0; 
		h21_2 = 0;
		InSB = 0;
		// устанавливаем OutRd в 0
		DDRC |= (1<<5);
		PORTC &= ~(1<<5);
		// переводим OutBxx и OutCxx в Z-состояние
		DDRD = 0;
		PORTD = 0;
		DDRB = 0;
		PORTB = 0;
		DDRA=0;
		//Проверка нажатия клавиш
		in1 = (PINA & 0b10000)>>4;
		in2 = (PINA & 0b100000)>>5;
		
		if ((in1 == 0) && (in2 == 0))
		{
			DDRD |= (1<<3);
			PORTD &= ~(1<<3); 
			InSB = 0; //мало мощный
		}
		if ((in1 == 1) && (in2 == 1))
		{
			DDRD |= (1<<0);
			PORTD &= ~(1<<0); 
			InSB = 1; //мощный
		}
		if ((in1 == 1) && (in2 != 1))
		{
			DDRB |= (1<<7);
			PORTB &= ~(1<<7);
			InSB = 2; //составной
		}
		//Конец проверки нажатия клавиш	
		//Начало повторения листа
		unsigned char i = 0; // Счётчик пройденных кругов
		unsigned char type = 0; // type=1 - Полевой, type=3 - JFET, type=0 - не определили, если h21_1!=0 или h21_2!=0, то биполярный

		while ((i < 12) && (type != 1)&& (type != 3))
		//while ((i < 12) && (type == 0))
		{
			type = step(outs[i],i,InSB,cell1,cell2);
			i++;
		}			

		//Биполярный
		if(h21_1!=0)
		{
			if(h21_1 > h21_2) //Из ячеек 1 и 2 выбираем большее h21 и выводим на дисплэй
			{
					InitLCD();//Инициализация или очищение экрана
					i2cMasterSendNI(0x76,LENGTH_CEL,cell1);		
					LCD_Address();//Переходим на вторую строку дисплея
					i2cMasterSendNI(0x76,LENGTH_SYM_H21,sym_h21);
					INTtoLCD(h21_1);
			}
			else
			{				
					InitLCD();//Инициализация или очищение экрана
					i2cMasterSendNI(0x76,LENGTH_CEL,cell2);
					LCD_Address();//Переходим на вторую строку дисплея
					i2cMasterSendNI(0x76,LENGTH_SYM_H21,sym_h21);
					INTtoLCD(h21_2);			
			}
			
		}
		else
		{
			if(!((type==1)||(type==3)))
			{
				i = 0;
				type = 0;
				unsigned char j = 0;
				while ((i < 6) && (type != 1))
				{
					type = diode(outs [j], i);//Функция для диодов . 
					i ++;
					j=j+2;
				}
				// не определен
				if (type == 0)//if (type == 0) ... было до добавления диодов
				{
					//Выводим на дисплей Транзистор не определён
					InitLCD();//Инициализация или очищение экрана
					//Некоторые буквы имеют свой код: Н-200; е-229; о-239; п-109; р-240; д-86; л-91; ё-63; н-124
					unsigned char sym[13] = {0b01000000, 200, 229, ' '+128, 239, 109, 240, 229, 86, 229, 91, 63, 124};
					i2cMasterSendNI(0x76,13,sym);				
				}
			}
		}			
		_delay_ms(2000);//пауза 2c
    }//Конец повторения листов
}


void setOutABC_TriState()
{
	//outC_BM
	DDRB&=~(1<<PB5);
	PORTB&=~(1<<PB5);
	//outB_BM
	DDRB&=~(1<<PB6);
	PORTB&=~(1<<PB6);
	//outA_BM
	DDRB&=~(1<<PB7);
	PORTB&=~(1<<PB7);
	
	//outA_BS
	DDRD&=~(1<<PD0);
	PORTD&=~(1<<PD0);
	//outB_BS	
	DDRD&=~(1<<PD1);
	PORTD&=~(1<<PD1);
	//outC_BS
	DDRD&=~(1<<PD2);
	PORTD&=~(1<<PD2);
	
	//outA_BMM
	DDRD&=~(1<<PD3);
	PORTD&=~(1<<PD3);
	//outB_BMM
	DDRD&=~(1<<PD4);
	PORTD&=~(1<<PD4);
	//outC_BMM
	DDRD&=~(1<<PD5);
	PORTD&=~(1<<PD5);	
}

unsigned char step(unsigned char outs, unsigned char i, unsigned char InSB, char cell1[40], char cell2[40])
{
	setOutABC_TriState();
	
	//Подаём импульсы
	DDRB |= (1<<4); //1.1
	if((outs&0b00100000)>>5)
		PORTB |= (1<<4); //1.1
	else
		PORTB &= ~(1<<4); //1.1
	
	DDRC |= (1<<2); //1.2
	if((outs&0b00010000)>>4)
		PORTC |= (1<<2); //1.2
	else
		PORTC &= ~(1<<2); //1.2
	
	DDRB |= (1<<1); //2.1
	if((outs&0b00001000)>>3)
		PORTB |= (1<<1); //2.1
	else
		PORTB &= ~(1<<1); //2.1
	
	DDRC |= (1<<3);
	if((outs&0b00000100)>>2)
		PORTC |= (1<<3); //2.2
	else
		PORTC &= ~(1<<3); //2.2
	
	DDRB |= (1<<0); //3.1
	if ((outs&0b00000010)>>1)
		PORTB |= (1<<0); //3.1
	else
		PORTB &= ~(1<<0); //3.1
		
	DDRC |= (1<<4);
	if((outs&0b00000001)>>0)
		PORTC |= (1<<4); //3.2
	else
		PORTC &= ~(1<<4); //3.2
		
	unsigned char IObn1, IObn2, Polev;
	//Считываем напряжение на  PORTA(6)
	_delay_ms(8);
	IObn1 = a2dConvert8bit(0);
	_delay_ms(8);

	switch (InSB)//Выбираем на какую ногу подавать импульс
	{	
		case 0://м.мощн
		{
			// Out A Переводим в Z Пройдя 4 раза
			if (i == 4)
			{
				DDRD &= ~(1<<3); 
				PORTD &= ~(1<<3); 
			}
			// Out B Переводим в Z Пройдя 8 раз
			if (i == 8)
			{
				DDRD &= ~(1<<4); 
				PORTD &= ~(1<<4);  
			}
			
			DDRD |= (1 << pins_bmm[i]);
			_delay_ms(1);
			
			if (i%2)
			{	
				PORTD &= ~(1 << pins_bmm[i]);
			} 
			else
			{
				PORTD |= (1 << pins_bmm[i]);
			}
			break;	
		}
		case 1://мощ
		{
			// Out A Переводим в Z
			if (i == 4)
			{
				DDRB &= ~(1<<7); 
				PORTB &= ~(1<<7);   
			}
			// Out B Переводим в Z
			if (i == 8)
			{
				DDRB &= ~(1<<6);
				PORTB &= ~(1<<6); 
			}
			
			_delay_ms(1);
			DDRB |= (1 << pins_bm[i]);
			
			if (i%2)
			{
				PORTB &= ~(1 << pins_bm[i]);
			} 
			else
			{
				PORTB |= (1 << pins_bm[i]);
			}
			break;
		}
		case 2://сост
		{
			// Out A Переводим в Z
			if (i == 4)
			{
				DDRD &= ~(1<<0); 
				PORTD &= ~(1<<0);  
			}
			// Out B Переводим в Z
			if (i == 8)
			{
				DDRD &= ~(1<<1); 
				PORTD &= ~(1<<1); 
			}
			
			_delay_ms(1);
			DDRD |= (1<<pins_bs[i]);
			
			if (i%2)
			{
				PORTD &= ~(1 << pins_bs[i]);
			} 
			else
			{
				PORTD |= (1 << pins_bs[i]);
			} 
			break;
		}
	}
	//Считываем напряжение на PORTA(6) 2й раз
	_delay_ms(2);//2мс
	IObn2 = a2dConvert8bit(0);
	_delay_ms(2);
	//Проверка на обнаружение транзистора
	if ((int)(IObn2 - IObn1) < 12.8)//0.5 = 25,6 //0.25В = 12.8
		return 0; // транзистор на данной итерации не обнаружен
		
	//Считываем напряжение на PORTA(1 или 2 или 3)
	Polev = a2dConvert8bit(pins_polev[i]);
	
	/*
	_delay_ms(1000);
	InitLCD();//Инициализация или очищение экрана
	INTtoLCD(Polev);
	_delay_ms(1000);
	*/
		
	if (i%2)
	{
		if (Polev < 20)//20 = 0.4v
		{				
			InitLCD();//Инициализация или очищение экрана
			i2cMasterSendNI(0x76,LENGTH_DISPLAY,array_display[i]);
			LCD_Address();//Переходим на вторую строку дисплея	
			i2cMasterSendNI(0x76,LENGTH_SYM_MOSFET,sym_MOSFET);
			//вывод на дисплей полярных транзисторов
			if(IObn1 > 0)
			{
				//Некоторые буквы имеют свой код: н-124
				unsigned char sym_Inah[6] = {0b01000000, 'I'+128, 124, '='+128};
				i2cMasterSendNI(0x76,4,sym_Inah);
				DOUBLEtoLCD(IObn1*0.029);
				//Некоторые буквы имеют свой код: м-108; А- транслитом
				unsigned char sym_mA[3] = {0b01000000, 108, 'A'+128};
				i2cMasterSendNI(0x76,3,sym_mA);					
			}
			return 1;
		}
	}	
	else
	{
		if (Polev > 250)// 250 = 4.9v
		{	
			InitLCD();//Инициализация или очищение экрана
			i2cMasterSendNI(0x76,LENGTH_DISPLAY,array_display[i]);
			LCD_Address();//Переходим на вторую строку дисплея	
			i2cMasterSendNI(0x76,LENGTH_SYM_MOSFET,sym_MOSFET);	
			if(IObn1 > 0)
			{
				//Некоторые буквы имеют свой код: н-124
				unsigned char sym_Inah[6] = {0b01000000, 'I'+128, 124, '='+128};
				i2cMasterSendNI(0x76,4,sym_Inah);
				DOUBLEtoLCD(IObn1*0.029);
				//Некоторые буквы имеют свой код: м-108; А- транслитом
				unsigned char sym_mA[3] = {0b01000000, 108, 'A'+128};
				i2cMasterSendNI(0x76,3,sym_mA);					
			}
			return 1;
		}
	}
	//!!!
	if(IObn1 > 10) // 10 = 0.2v
	{	
			//JFET
			InitLCD();//Инициализация или очищение экрана
			i2cMasterSendNI(0x76,LENGTH_DISPLAY,array_display[i]);
			LCD_Address();//Переходим на вторую строку дисплея	
			unsigned char sym_JFET[6] = {0b01000000, 'J'+128, 'F'+128, 'E'+128, 'T'+128, ' '+128};
			i2cMasterSendNI(0x76,6,sym_JFET);	
			//Некоторые буквы имеют свой код: н-124
			unsigned char sym_Inah[6] = {0b01000000, 'I'+128, 124, '='+128};
			i2cMasterSendNI(0x76,4,sym_Inah);
			DOUBLEtoLCD(IObn1*0.029);
			//Некоторые буквы имеют свой код: м-108; А- транслитом
			unsigned char sym_mA[3] = {0b01000000, 108, 'A'+128};
			i2cMasterSendNI(0x76,3,sym_mA);				
			return 3;
	}				
	
	//Смотрим не пуста ли ячейка памяти 1 если нет то пишем в 2
	if (h21_1 != 0)
	{
		h21_2 = h21_1;
		//Записываем в ячейку памяти структуру
		for (unsigned char j = 0; j < LENGTH_CEL; j++)
			cell2[j] = cell1[j];
	}		
	//Записываем в ячейку памяти структуру
	for (unsigned char j = 0; j < LENGTH_CEL; j++)
		cell1[j] = array_cel[i][j];	
	h21_1 = Unit1 (i, InSB);	
	return 2;
}

int Unit1 (unsigned char i ,unsigned char InSB)
{
	//Out подкл. Rд 
	DDRC |= (1<<5);
	PORTC |= (1<<5);
	int InIzm; 
	int h21 = 0;
	_delay_us(100);
	// Читаем ADC0 в переменную InIzm 
	InIzm=a2dConvert10bit(0);
	//Out подкл. Rд 
	PORTC &= ~(1<<5);
	//Начинаем измерять h21 
	InIzm = InIzm*5;  // пересчет в мВ
	if ((i+1)%2==1)
	{
		 //вычисления только для NPN
		switch (InSB)
		{	
			case 0://м.мощн
			{
				h21 = 4400 * (InIzm / (4000.0f - InIzm)); 
				break;
			}
			case 1://мощ
			{
				h21 = 4300 * (InIzm / (3950.0f - InIzm));break;
			}
			case 2://сост
			{
				h21 = 39000 * (InIzm / (3350.0f - InIzm));break;
			}	
		}						
	}
	else
	{
		//вычисления только для PNP
		h21 = InIzm;
		//Если нажата кнопка SB1
		if (InSB == 2)//Составной транзистор (лог 1 на in1 и in 2)
			h21 *= 10;
	}
	//Закончили измерять h21 
	return h21;
}

int diode (unsigned char outs, unsigned char i)
{
	//Подаём импульсы
	DDRB |= (1<<4); //1.1
	if((outs&0b00100000)>>5)
		PORTB |= (1<<4); //1.1
	else
		PORTB &= ~(1<<4); //1.1
	
	DDRC |= (1<<2); //1.1
	if((outs&0b00010000)>>4)
		PORTC |= (1<<2); //1.2
	else
		PORTC &= ~(1<<2); //1.2
	
	DDRB |= (1<<1); //2.1
	if((outs&0b00001000)>>3)
		PORTB |= (1<<1); //2.1
	else
		PORTB &= ~(1<<1); //2.1
	
	DDRC |= (1<<3);
	if((outs&0b00000100)>>2)
		PORTC |= (1<<3); //2.2
	else
		PORTC &= ~(1<<3); //2.2
	
	DDRB |= (1<<0); //3.1
	if ((outs&0b00000010)>>1)
		PORTB |= (1<<0); //3.1
	else
		PORTB &= ~(1<<0); //3.1
		
	DDRC |= (1<<4);
	if((outs&0b00000001)>>0)
		PORTC |= (1<<4); //3.2
	else
		PORTC &= ~(1<<4); //3.2
	int IObn;	
	
	_delay_ms(2);
	IObn = a2dConvert8bit(0);	
	_delay_ms(2);
	
	

	
/*	_delay_ms(1000);
	InitLCD();//Инициализация или очищение экрана
	INTtoLCD(IObn);
	_delay_ms(1000);
	*/
	if (IObn < 26) //25,6 = 0.5В
	{
		return 0;
	}
	
/*
	InitLCD();//Инициализация или очищение экрана
	INTtoLCD(123);
	_delay_ms(1000);
	*/
	unsigned int	UXS1 = 0, UXS2 = 0, U1 = 0, U2 = 0;
	unsigned char DIOD_UXS [6][2] = {{2, 3}, {2, 3}, {1, 3}, {1, 3}, {1, 2}, {1, 2}};	
			
	//измеряем UXS1  UXS2
	UXS1 = a2dConvert10bit(DIOD_UXS[i][0]);
	UXS2 = a2dConvert10bit(DIOD_UXS[i][1]);
	U1 = abs(UXS1 - UXS2);

	
	PORTC |= (1<<5);//Устанавливаем логический 1 на Out Rд
	//измеряем UXS1  UXS2
	UXS1 = a2dConvert10bit(DIOD_UXS[i][0]);
	UXS2 = a2dConvert10bit(DIOD_UXS[i][1]);
	
	PORTC &= ~(1<<5);//Устанавливаем логический 0 на Out Rд
	U2 = abs(UXS1 - UXS2);
	
	unsigned char diod [2] [LENGTH_CEL2] ={
	//Некоторые буквы имеют свой код: 
	{0b01000000,  203, 225, 125, 239, 86,'-'+128, 193, 124, 239, 86},//Анод-Катод
	{0b01000000,  193, 124, 239, 86,'-'+128, 203, 225, 125, 239, 86}};//Катод-Анод

	
	InitLCD();//Инициализация или очищение экрана
	//Выводим на дисплэй
	if (i%2)
		i2cMasterSendNI(0x76,LENGTH_CEL2,diod[0]);
	else
		i2cMasterSendNI(0x76,LENGTH_CEL2,diod[1]);
	
	LCD_Address();//Переходим на вторую строку дисплея	
	TwoDOUBLEtoLCD(U1*0.0048,U2*0.0048);
	
	
	return 1;
}