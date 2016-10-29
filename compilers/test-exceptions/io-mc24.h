/*
 * Hardware register defines for Elvees MC-24 microcontroller.
 */
#ifndef IOMC24_H_
#define IOMC24_H_

#define MC_HAVE_SPORT

/*--------------------------------------
 * Coprocessor 0 registers.
 */
#define C0_INDEX	$0	/* индекс доступа к TLB */
#define C0_RANDOM	$1	/* индекс TLB для команды Write Random */
#define C0_ENTRYLO0	$2	/* строки для чётных страниц TLB */
#define C0_ENTRYLO1	$3	/* строки для нечётных страниц TLB */
#define C0_CONTEXT	$4	/* указатель на таблицу PTE */
#define C0_PAGEMASK	$5	/* маска размера страниц TLB */
#define C0_WIRED	$6	/* граница привязанных строк TLB */
#define C0_BADVADDR	$8	/* виртуальный адрес последнего исключения */
#define C0_COUNT	$9	/* таймер */
#define C0_ENTRYHI	$10	/* информация соответствия виртуального адреса */
#define C0_COMPARE	$11	/* предельное значение для прерывания по таймеру */
#define C0_STATUS	$12	/* режимы функционирования процессора */
#define C0_CAUSE	$13	/* причина последнего исключения */
#define C0_EPC		$14	/* адрес возврата из исключения */
#define C0_PRID		$15	/* идентификатор процессора */
#define C0_CONFIG	$16	/* информация о возможностях процессора */
#define C0_LLADDR	$17	/* физический адрес последней команды LL */
#define C0_ERROREPC	$30	/* адрес возврата из исключения ошибки */

/*
 * Status register.
 */
#define ST_IE		0x00000001	/* разрешение прерываний */
#define ST_EXL		0x00000002	/* уровень исключения */
#define ST_ERL		0x00000004	/* уровень ошибки */
#define ST_UM		0x00000010	/* режим пользователя */
#define ST_IM_SW0	0x00000100	/* программное прерывание 0 */
#define ST_IM_SW1	0x00000200	/* программное прерывание 1 */
#define ST_IM_IRQ0	0x00000400	/* внешнее прерывание 0 */
#define ST_IM_IRQ1	0x00000800	/* внешнее прерывание 1 */
#define ST_IM_IRQ2	0x00001000	/* внешнее прерывание 2 */
#define ST_IM_IRQ3	0x00002000	/* внешнее прерывание 3 */
#define ST_IM_MCU	0x00008000	/* от внутренних устройств микроконтроллера */
#define ST_NMI		0x00080000	/* причина сброса - NMI */
#define ST_TS		0x00200000	/* TLB-закрытие системы */
#define ST_BEV		0x00400000	/* размещение векторов: начальная загрузка */

#define ST_CU0		0x10000000	/* разрешение сопроцессора 0 */
#define ST_CU1		0x20000000	/* разрешение сопроцессора 1 (FPU) */

/*
 * Сause register.
 */
#define CA_EXC_CODE	0x0000007c	/* код исключения */
#define CA_Int		0		/* прерывание */
#define CA_Mod		(1 << 2)	/* TLB-исключение модификации */
#define CA_TLBL		(2 << 2)	/* TLB-исключение, загрузка или вызов команды */
#define CA_TLBS		(3 << 2)	/* TLB-исключение, сохранение */
#define CA_AdEL		(4 << 2)	/* ошибка адресации, загрузка или вызов команды */
#define CA_AdES		(5 << 2)	/* ошибка адресации, сохранение */
#define CA_Sys		(8 << 2)	/* системное исключение */
#define CA_Bp		(9 << 2)	/* breakpoint */
#define CA_RI		(10 << 2)	/* зарезервированная команда */
#define CA_CpU		(11 << 2)	/* недоступность сопроцессора */
#define CA_Ov		(12 << 2)	/* целочисленное переполнение */
#define CA_Tr		(13 << 2)	/* trap */
#define CA_MCheck	(24 << 2)	/* аппаратный контроль */

#define CA_ID		0x00000080	/* прерывание от блока отладки OnCD */
#define CA_IP_SW0	0x00000100	/* программное прерывание 0 */
#define CA_IP_SW1	0x00000200	/* программное прерывание 1 */
#define CA_IP_IRQ0	0x00000400	/* внешнее прерывание 0 */
#define CA_IP_IRQ1	0x00000800	/* внешнее прерывание 1 */
#define CA_IP_IRQ2	0x00001000	/* внешнее прерывание 2 */
#define CA_IP_IRQ3	0x00002000	/* внешнее прерывание 3 */
#define CA_IP_MCU	0x00008000	/* от внутренних устройств микроконтроллера */
#define CA_IV		0x00800000	/* 1 - используется спец.вектор 0x200 */
#define CA_BD		0x80000000	/* исключение в слоте задержки перехода */


/*--------------------------------------
 * Регистры управления периферией Elvees Multicore.
 */
#define MC_R(a)		(0xB82F0000 + (a))

/*
 * Системные регистры
 */
#define MC_MASKR	MC_R (0x4000)	/* Регистр маски */
#define MC_QSTR		MC_R (0x4004)	/* Регистр заявок */
#define MC_CSR		MC_R (0x4008)	/* Регистр управления */
#define MC_MASKR_DSP	MC_R (0x4010)	/* Регистр маски DSP */
#define MC_QSTR_DSP	MC_R (0x4014)	/* Регистр заявок DSP */
#define MC_MASKR_HM	MC_R (0x4018)	/* Регистр маски кода Хемминга */
#define MC_QSTR_HM	MC_R (0x401C)	/* Регистр заявок кода Хемминга */
#define MC_CSR_CRAM	MC_R (0x4410)	/* Регистр управления CRAM */
#define MC_AERROR_CRAM	MC_R (0x4414)	/* FIFO ошибочных адресов CRAM */
#define MC_CSR_ICACHE	MC_R (0x4800)	/* Регистр управления CRAM */
#define MC_AERROR_ICACHE MC_R (0x4814)	/* FIFO ошибочных адресов CRAM */

/*
 * Регистры порта внешней памяти
 */
#define MC_CSCON0	MC_R (0x1000)	/* Регистр конфигурации 0 */
#define MC_CSCON1	MC_R (0x1004)	/* Регистр конфигурации 1 */
#define MC_CSCON2	MC_R (0x1008)	/* Регистр конфигурации 2 */
#define MC_CSCON3	MC_R (0x100C)	/* Регистр конфигурации 3 */
#define MC_CSCON4	MC_R (0x1010)	/* Регистр конфигурации 4 */
#define MC_SDRCON	MC_R (0x1014)	/* Регистр конфигурации памяти SDRAM */
#define MC_CKE_CTR	MC_R (0x1018)	/* Регистр управления состоянием вывода CKE микросхемы */
#define MC_CSR_EXT	MC_R (0x101C)	/* Регистр управления и состояния внешней памяти */
#define MC_AER_ROR_EXT	MC_R (0x1020)	/* FIFO ошибочных адресов внешней памяти */

/*
 * Регистры UART
 */
#define MC_RBR		MC_R (0x3000)	/* Приемный буферный регистр */
#define MC_THR		MC_R (0x3000)	/* Передающий буферный регистр */
#define MC_IER		MC_R (0x3004)	/* Регистр разрешения прерываний */
#define MC_IIR		MC_R (0x3008)	/* Регистр идентификации прерывания */
#define MC_FCR		MC_R (0x3008)	/* Регистр управления FIFO */
#define MC_LCR		MC_R (0x300C)	/* Регистр управления линией */
#define MC_MCR		MC_R (0x3010)	/* Регистр управления модемом */
#define MC_LSR		MC_R (0x3014)	/* Регистр состояния линии */
#define MC_MSR		MC_R (0x3018)	/* Регистр состояния модемом */
#define MC_SPR		MC_R (0x301C)	/* Регистр Scratch Pad */
#define MC_DLL		MC_R (0x3000)	/* Регистр делителя младший */
#define MC_DLM		MC_R (0x3004)	/* Регистр делителя старший */
#define MC_SCLR		MC_R (0x3014)	/* Регистр предделителя (scaler) */

/*
 * Регистры интервального таймера (IT)
 */
#define MC_ITCSR	MC_R (0xD000)	/* Регистр управления */
#define MC_ITPERIOD	MC_R (0xD004)	/* Регистр периода работы таймера */
#define MC_ITCOUNT	MC_R (0xD008)	/* Регистр счетчика */
#define MC_ITSCALE	MC_R (0xD00C)	/* Регистр предделителя */

/*
 * Регистры WDT
 */
#define MC_WTCSR	MC_R (0xD010)	/* Регистр управления */
#define MC_WTPERIOD	MC_R (0xD014)	/* Регистр периода работы таймера */
#define MC_WTCOUNT	MC_R (0xD018)	/* Регистр счетчика */
#define MC_WTSCALE	MC_R (0xD01C)	/* Регистр предделителя */

/*
 * Регистры RTT
 */
#define MC_RTCSR	MC_R (0xD020)	/* Регистр управления */
#define MC_RTPERIOD	MC_R (0xD024)	/* Регистр периода работы таймера */
#define MC_RTCOUNT	MC_R (0xD028)	/* Регистр счетчика */

/*
 * Регистры линковых портов LPORT0...LPORT3
 */
#define MC_LTX(n)	MC_R (0x7000+(n<<12))	/* Буфер передачи */
#define MC_LRX(n)	MC_R (0x7000+(n<<12))	/* Буфер приема */
#define MC_LCSR(n)	MC_R (0x7004+(n<<12))	/* Регистр управления и состояния */
#define MC_LDIR(n)	MC_R (0x7008+(n<<12))	/* Регистр управления */
#define MC_LDR(n)	MC_R (0x700C+(n<<12))	/* Регистр данных */

/*
 * Регистры портов обмена последовательным кодом SPORT0, SPORT1
 */
#ifdef MC_HAVE_SPORT
#define MC_STX(n)	MC_R (0x5000+(n<<12))	/* Буфер передачи данных */
#define MC_RX(n)	MC_R (0x5000+(n<<12))	/* Буфер приема данных */
#define MC_STCTL(n)	MC_R (0x5004+(n<<12))	/* Регистр управления передачей данных */
#define MC_SRCTL(n)	MC_R (0x5008+(n<<12))	/* Регистр управления приемом данных */
#define MC_TDIV(n)	MC_R (0x500C+(n<<12))	/* Регистр коэффициентов деления при передаче */
#define MC_RDIV(n)	MC_R (0x5010+(n<<12))	/* Регистр коэффициентов деления при приеме */
#define MC_MTCS(n)	MC_R (0x5014+(n<<12))	/* Выбор канала передачи в многоканальном режиме */
#define MC_MRCS(n)	MC_R (0x5018+(n<<12))	/* Выбор канала приема в многоканальном режиме */
#define MC_KEYWD(n)	MC_R (0x501C+(n<<12))	/* Регистр кода сравнения */
#define MC_KEYMASK(n)	MC_R (0x5020+(n<<12))	/* Регистр маски сравнения */
#define MC_MRCE(n)	MC_R (0x5024+(n<<12))	/* Выбор канала для сравнения принимаемых данных */
#endif

#ifdef MC_HAVE_SWIC
TODO
#endif

/*
 * Регистры DMA
 */
/* Каналы SpTx0, SpTx1 */
#define MC_CSR_SPTX(n)	MC_R (0x0000+(n<<9))	/* Регистр управления и состояния */
#define MC_CP_SPTX(n)	MC_R (0x0008+(n<<9))	/* Регистр указателя цепочки */
#define MC_IR_SPTX(n)	MC_R (0x000C+(n<<9))	/* Индексный регистр памяти */
#define MC_OR_SPTX(n)	MC_R (0x0010+(n<<9))	/* Регистр смещения памяти */
#define MC_Y_SPTX(n)	MC_R (0x0014+(n<<9))	/* Регистр параметров направления Y при
							 * двухмерной адресации памяти */
/* Каналы SpRx0, SpRx1 */
#define MC_CSR_SPRX(n)	MC_R (0x0100+(n<<9))	/* Регистр управления и состояния */
#define MC_CP_SPRX(n)	MC_R (0x0108+(n<<9))	/* Регистр указателя цепочки */
#define MC_IR_SPRX(n)	MC_R (0x010C+(n<<9))	/* Индексный регистр памяти */
#define MC_OR_SPRX(n)	MC_R (0x0110+(n<<9))	/* Регистр смещения памяти */
#define MC_Y_SPRX(n)	MC_R (0x0114+(n<<9))	/* Регистр параметров направления Y при
							 * двухмерной адресации памяти */
/* Каналы LpCh0...LpCh3 */
#define MC_CSR_LPCH(n)	MC_R (0x0400+(n<<8))	/* Регистр управления и состояния */
#define MC_CP_LPCH(n)	MC_R (0x0408+(n<<8))	/* Регистр указателя цепочки */
#define MC_IR_LPCH(n)	MC_R (0x040C+(n<<8))	/* Индексный регистр памяти */
#define MC_OR_LPCH(n)	MC_R (0x0410+(n<<8))	/* Регистр смещения памяти */
#define MC_Y_LPCH(n)	MC_R (0x0414+(n<<8))	/* Регистр параметров направления Y при
							 * двухмерной адресации памяти */
/* Каналы MemCh0...MemCh3 */
#define MC_CSR_MEMCH(n)	MC_R (0x0800+(n<<8))	/* Регистр управления и состояния */
#define MC_IOR_MEMCH(n)	MC_R (0x0804+(n<<8))	/* Регистр индекса и смещения внутренней памяти */
#define MC_CP_MEMCH(n)	MC_R (0x0808+(n<<8))	/* Регистр указателя цепочки */
#define MC_IR_MEMCH(n)	MC_R (0x080C+(n<<8))	/* Индексный регистр внешней памяти */
#define MC_OR_MEMCH(n)	MC_R (0x0810+(n<<8))	/* Регистр смещения внешней памяти */
#define MC_Y_MEMCH(n)	MC_R (0x0814+(n<<8))	/* Регистр параметров направления Y при
							 * двухмерной адресации внешней памяти */
#define MC_RUN(n)	MC_R (0x0818+(n<<8))	/* Псевдорегистр управления состоянием бита RUN */

/*--------------------------------------
 * Системный регистр CSR
 */
#define MC_CSR_FM		0x00000001	/* Fixed mapping */
#define MC_CSR_CLK(n)		(n << 4)	/* PLL clock multiply, 1..31, 0=1/16 */
#define MC_CSR_TST_CACHE	0x00000800	/* random access to cache */
#define MC_CSR_FLUSH_I		0x00001000	/* instruction cache invalidate */
#define MC_CSR_FLUSH_D		0x00004000	/* data cache invalidate */
#define MC_CSR_CLKEN		0x00010000	/* PLL clock enable */

/*
 * Системный регистр MASKR
 */
#define MC_MASKR_SRX0		0x00000001	/* SPORT0 receive */
#define MC_MASKR_STX0		0x00000002	/* SPORT0 transmit */
#define MC_MASKR_SRX1		0x00000004	/* SPORT0 receive */
#define MC_MASKR_STX1		0x00000008	/* SPORT0 transmit */
#define MC_MASKR_UART		0x00000010	/* UART */
#define MC_MASKR_LTRX0		0x00000080	/* LPORT0 data */
#define MC_MASKR_LSRQ0		0x00000100	/* LPORT0 service */
#define MC_MASKR_LTRX1		0x00000200	/* LPORT1 data */
#define MC_MASKR_LSRQ1		0x00000400	/* LPORT1 service */
#define MC_MASKR_LTRX2		0x00000800	/* LPORT2 data */
#define MC_MASKR_LSRQ2		0x00001000	/* LPORT2 service */
#define MC_MASKR_LTRX3		0x00002000	/* LPORT3 data */
#define MC_MASKR_LSRQ3		0x00004000	/* LPORT3 service */
#define MC_MASKR_COMPARE	0x00080000	/* CPU timer */
#define MC_MASKR_MEMCH0		0x00200000	/* DMA MemCh0 */
#define MC_MASKR_MEMCH1		0x00400000	/* DMA MemCh0 */
#define MC_MASKR_MEMCH2		0x00800000	/* DMA MemCh0 */
#define MC_MASKR_MEMCH3		0x01000000	/* DMA MemCh0 */
#define MC_MASKR_TIMER		0x20000000	/* timers IT, WDT, RTT */
#define MC_MASKR_PI		0x40000000	/* DSP software interrupt */
#define MC_MASKR_SBS		0x80000000	/* DSP status */

/*
 * Регистры порта внешней памяти CSCONi
 */
#define MC_CSCON_CSMASK(addr)	(addr >> 24 & 0xff)
						/* Address mask bits 31:24 */
#define MC_CSCON_CSBA(addr)	(addr >> 16 & 0xff00)
						/* Base address bits 31:24 */
#define MC_CSCON_WS(n)		(n << 16)	/* Wait states for async memory */
#define MC_CSCON_E		(1 << 20)	/* Enable for nCS0, nCS1, nCS2 */
#define MC_CSCON_T		(1 << 21)	/* Sync memory flag (only nCS0, nCS1) */
#define MC_CSCON_AE		(1 << 22)	/* Wait for nACK */
#define MC_CSCON_W64		(1 << 23)	/* 64-bit data width */
#define MC_CSCON3_BYTE		(1 << 23)	/* 8-bit data width for nCS3 */
#define MC_CSCON3_OVER		(1 << 24)	/* Status: no nACK for 256 CLK periods */
#define MC_CSCON3_ADDR(addr)	((addr & 3) << 20)
						/* Address bits 1:0 for 8-bit memory access */

/*
 * Регистр конфигурации синхронной динамической памяти SDRCON
 */
#define MC_SDRCON_PS_512	(0 << 0)	/* Page size 512 */
#define MC_SDRCON_PS_1024	(1 << 0)	/* Page size 1024 */
#define MC_SDRCON_PS_2048	(2 << 0)	/* Page size 2048 */
#define MC_SDRCON_PS_4096	(3 << 0)	/* Page size 4096 */
#define MC_SDRCON_RFR(nsec,khz)	(((nsec*khz+999999)/1000000) << 4)
						/* Refresh period */
#define MC_SDRCON_BL_1		(0 << 16)	/* Bursh length 1 */
#define MC_SDRCON_BL_2		(1 << 16)	/* Bursh length 2 */
#define MC_SDRCON_BL_4		(2 << 16)	/* Bursh length 4 */
#define MC_SDRCON_BL_8		(3 << 16)	/* Bursh length 8 */
#define MC_SDRCON_BL_PAGE	(7 << 16)	/* Bursh full page */

#define MC_SDRCON_WBM		(1 << 19)	/* Write burst mode - single write */
#define MC_SDRCON_CL		(1 << 20)	/* CAS latency: 0 - 2, 1 - 3 cycles */
#define MC_SDRCON_INIT		(1 << 31)	/* Initialize SDRAM, 2 usec */


/*--------------------------------------
 * Интервальный таймер, регистр управления ITCSR
 */
#define MC_ITCSR_EN		0x00000001	/* разрешение работы таймера */
#define MC_ITCSR_INT		0x00000002	/* признак срабатывания таймера */

/*--------------------------------------
 * Сторожевой таймер, регистр управления WTCSR
 */
#define MC_WTCSR_KEY1		0x000000A0	/* первый ключ */
#define MC_WTCSR_KEY2		0x000000F5	/* второй ключ */
#define MC_WTCSR_EN		0x00000100	/* разрешение работы таймера */
#define MC_WTCSR_INT		0x00000200	/* признак срабатывания таймера */
#define MC_WTCSR_MODE_ITM	0x00000400	/* режим обычного таймера */
#define MC_WTCSR_RLD		0x00000800	/* периодическая перезагрузка */

#define MC_WTCSR_INT_CTR	0x00003000	/* управление типом прерывания */
#define MC_WTCSR_INT_DISABLE	0x00000000	/* прерывание не формируется */
#define MC_WTCSR_INT_TIMER	0x00001000	/* обычное прерывание QSTR[29] */
#define MC_WTCSR_INT_NMI	0x00002000	/* немаскируемое прерывание */
#define MC_WTCSR_INT_EXTSIG	0x00003000	/* внешний сигнал WDT */

/*--------------------------------------
 * UART.
 */
/*
 * Line control register
 */
#define MC_LCR_5BITS            0x00    /* character length: 5 bits */
#define MC_LCR_6BITS            0x01    /* character length: 6 bits */
#define MC_LCR_7BITS            0x02    /* character length: 7 bits */
#define MC_LCR_8BITS            0x03    /* character length: 8 bits */

#define MC_LCR_STOPB            0x04    /* use 2 stop bits */
#define MC_LCR_PENAB            0x08    /* parity enable */
#define MC_LCR_PEVEN            0x10    /* even parity */
#define MC_LCR_PFORCE           0x20    /* force parity */
#define MC_LCR_SBREAK           0x40    /* break control */
#define MC_LCR_DLAB             0x80    /* divisor latch access bit */

/*
 * FIFO control register
 */
#define MC_FCR_ENABLE		0x01	/* enable FIFO */
#define MC_FCR_RCV_RST		0x02	/* clear receive FIFO */
#define MC_FCR_XMT_RST		0x04	/* clear transmit FIFO */

#define MC_FCR_TRIGGER_1	0x00	/* receive FIFO level: 1/4 byte */
#define MC_FCR_TRIGGER_4	0x40	/* receive FIFO level: 4/16 bytes */
#define MC_FCR_TRIGGER_8	0x80	/* receive FIFO level: 8/56 bytes */
#define MC_FCR_TRIGGER_14	0xc0	/* receive FIFO level: 14/60 bytes */

/*
 * Line status register
 */
#define MC_LSR_RXRDY		0x01	/* receiver ready */
#define MC_LSR_OE		0x02	/* overrun error */
#define MC_LSR_PE		0x04	/* parity error */
#define MC_LSR_FE		0x08	/* framing error */
#define MC_LSR_BI		0x10	/* break interrupt */
#define MC_LSR_TXRDY		0x20	/* transmitter holding register empty */
#define MC_LSR_TEMT		0x40	/* transmitter empty */
#define MC_LSR_FIFOE		0x80	/* error in receive FIFO */

/*
 * Interrupt enable register
 */
#define MC_IER_ERXRDY		0x01	/* enable receive data/timeout intr */
#define MC_IER_ETXRDY		0x02	/* enable transmitter interrupts */
#define MC_IER_ERLS		0x04	/* enable receive line status intr */
#define MC_IER_EMSC		0x08	/* enable modem status interrupts */

/*
 * Interrupt identification register
 */
#define MC_IIR_NOPEND		0x01	/* no interrupt pending */
#define MC_IIR_IMASK		0x0e	/* interrupt type mask */
#define MC_IIR_FENAB		0xc0	/* set if FIFOs are enabled */

#define MC_IIR_RLS		0x06	/* receiver line status */
#define MC_IIR_RXRDY		0x04	/* receiver data available */
#define MC_IIR_RXTOUT		0x0c	/* character timeout indication */
#define MC_IIR_TXRDY		0x02	/* transmitter holding reg empty */
#define MC_IIR_MLSC		0x00	/* modem status */

/*
 * Modem control register
 */
#define MC_MCR_DTR		0x01	/* control DTR output */
#define MC_MCR_RTS		0x02	/* control RTS output */
#define MC_MCR_OUT1		0x04	/* control OUT1 output */
#define MC_MCR_OUT2		0x08	/* control OUT2 output, used as
					 * global interrupt enable in PCs */
#define MC_MCR_LOOPBACK		0x10	/* set local loopback mode */

/*
 * Modem status register
 */
#define MC_MSR_DCTS		0x01	/* CTS changed */
#define MC_MSR_DDSR		0x02	/* DSR changed */
#define MC_MSR_TERI		0x04	/* RI changed from 0 to 1 */
#define MC_MSR_DDCD		0x08	/* DCD changed */
#define MC_MSR_CTS		0x10	/* CTS input */
#define MC_MSR_DSR		0x20	/* DSR input */
#define MC_MSR_RI		0x40	/* RI input */
#define MC_MSR_DCD		0x80	/* DCD input */

/*
 * Compute the 16-bit baud rate divisor, given
 * the oscillator frequency and baud rate.
 * Round to the nearest integer.
 */
#define MC_DL_BAUD(fr,bd)	((fr/8 + (bd)) / (bd) / 2)

/*--------------------------------------
 * Coprocessor 1 (FPU) registers.
 */
#define C1_FIR		$0	/* implementation and revision */
#define C1_FCCR		$25	/* condition codes */
#define C1_FEXR		$26	/* exceptions */
#define C1_FENR		$28	/* enables */
#define C1_FCSR		$31	/* control/status */

/*
 * FPU control/status register
 */
#define FCSR_ROUND	0x00000003	/* round mode */
#define	FCSR_ROUND_N	0x00000000	/* round to nearest */
#define	FCSR_ROUND_Z	0x00000001	/* round toward zero */
#define	FCSR_ROUND_P	0x00000002	/* round toward positive infinity */
#define	FCSR_ROUND_M	0x00000003	/* round toward negative infinity */
#define	FCSR_FLAG_I	0x00000004	/* flag: inexact result */
#define	FCSR_FLAG_U	0x00000008	/* flag: underflow */
#define	FCSR_FLAG_O	0x00000010	/* flag: overflow */
#define	FCSR_FLAG_Z	0x00000020	/* flag: divide by zero */
#define	FCSR_FLAG_V	0x00000040	/* flag: invalid operation */
#define FCSR_ENABLES	0x00000f80	/* enables */
#define	FCSR_ENABLE_I	0x00000080	/* enables: inexact result */
#define	FCSR_ENABLE_U	0x00000100	/* enables: underflow */
#define	FCSR_ENABLE_O	0x00000200	/* enables: overflow */
#define	FCSR_ENABLE_Z	0x00000400	/* enables: divide by zero */
#define	FCSR_ENABLE_V	0x00000800	/* enables: invalid operation */
#define	FCSR_CAUSE_I	0x00001000	/* cause: inexact result */
#define	FCSR_CAUSE_U	0x00002000	/* cause: underflow */
#define	FCSR_CAUSE_O	0x00004000	/* cause: overflow */
#define	FCSR_CAUSE_Z	0x00008000	/* cause: divide by zero */
#define	FCSR_CAUSE_V	0x00010000	/* cause: invalid operation */
#define	FCSR_CAUSE_E	0x00020000	/* cause: unimplemented */
#define FCSR_FS		0x01000000	/* flush to zero */
#define	FCSR_COND_0	0x00800000	/* condition code 0 */
#define	FCSR_COND_1	0x02000000	/* condition code 1 */
#define	FCSR_COND_2	0x04000000	/* condition code 2 */
#define	FCSR_COND_3	0x08000000	/* condition code 3 */
#define	FCSR_COND_4	0x10000000	/* condition code 4 */
#define	FCSR_COND_5	0x20000000	/* condition code 5 */
#define	FCSR_COND_6	0x40000000	/* condition code 6 */
#define	FCSR_COND_7	0x80000000	/* condition code 7 */

#endif /* IOMC24_H_ */
