/*
 * S-функция для Simulink - эмуляция 16-битного процессора.
 * При компиляции необходимо определить требуемое имя функции, например:
 *	gcc -DS_FUNCTION_NAME=cpu
 *
 * Автор: Сергей Вакуленко, ИТМиВТ 2008.
 */
#define S_FUNCTION_LEVEL	2

#define STACK_BYTES		64000	/* Размер стека для сопроцесса */

#include "simstruc.h"
#include "mex.h"
#include "cpu.h"

#define CONTEXT_SAVE(c,ret) 	__asm__ volatile (	\
	"movl %0, %%eax \n"				\
	"movl %%ebp, (%%eax) \n"			\
	"movl %%ebx, 4(%%eax) \n"			\
	"movl %%edi, 8(%%eax) \n"			\
	"movl %%esi, 12(%%eax) \n"			\
	"movl %%esp, 16(%%eax) \n"			\
	"movl $"ret", 20(%%eax)"			\
	: : "r" (c))

#define CONTEXT_RESTORE(c) 	__asm__ volatile (	\
	"movl %0, %%eax \n"				\
	"movl (%%eax), %%ebp \n"			\
	"movl 4(%%eax), %%ebx \n"			\
	"movl 8(%%eax), %%edi \n"			\
	"movl 12(%%eax), %%esi \n"			\
	"movl 16(%%eax), %%esp \n"			\
	"movl 20(%%eax), %%edx \n"			\
	"jmp *%%edx "					\
	: : "r" (c))

static void set_input_port (SimStruct *S, int_T num, DTypeId type)
{
	ssSetInputPortWidth (S, num, 1);
	ssSetInputPortDataType (S, num, type);
	ssSetInputPortComplexSignal (S,  num, COMPLEX_NO);
	ssSetInputPortDirectFeedThrough (S, num, 1);
	ssSetInputPortRequiredContiguous (S, num, 1); /*direct input signal access*/
}

static void set_output_port (SimStruct *S, int_T num, DTypeId type)
{
	ssSetOutputPortWidth (S, num, 1);
	ssSetOutputPortDataType (S, num, type);
	ssSetOutputPortComplexSignal (S, num, COMPLEX_NO);
}

/*
 * Setup sizes of the various vectors.
 */
static void mdlInitializeSizes (SimStruct *S)
{
	/* Количество параметров. */
	ssSetNumSFcnParams (S, 0);
	if (ssGetNumSFcnParams (S) != ssGetSFcnParamsCount (S)) {
		/* Simulink сообщит о несоответствии параметров */
		return;
	}

	ssSetNumContStates (S, 0);
	ssSetNumDiscStates (S, 0);

	/*
	 * Входы.
	 */
	if (! ssSetNumInputPorts (S, 3))
		return;
	set_input_port (S, 0, SS_UINT16);	/* datain */
	set_input_port (S, 1, SS_BOOLEAN);	/* ack */
	set_input_port (S, 2, SS_BOOLEAN);	/* rst */

	/*
	 * Выходы.
	 */
	if (! ssSetNumOutputPorts (S, 5))
		return;
	set_output_port (S, 0, SS_UINT16);	/* dataout */
	set_output_port (S, 1, SS_UINT16);	/* addr */
	set_output_port (S, 2, SS_BOOLEAN);	/* rd */
	set_output_port (S, 3, SS_BOOLEAN);	/* wrh */
	set_output_port (S, 4, SS_BOOLEAN);	/* wrl */

	ssSetNumSampleTimes (S, 1);
	ssSetNumRWork (S, 0);
	ssSetNumIWork (S, 0);
	ssSetNumPWork (S, 1);
	ssSetNumModes (S, 0);
	ssSetNumNonsampledZCs (S, 0);

	/* Take care when specifying exception free code - see sfuntmpl_doc.c */
	ssSetOptions (S, SS_OPTION_EXCEPTION_FREE_CODE |
			 SS_OPTION_USE_TLC_WITH_ACCELERATOR |
			 SS_OPTION_WORKS_WITH_CODE_REUSE |
			 SS_OPTION_DISCRETE_VALUED_OUTPUT);
}

/*
 * Specifiy  the sample time.
 */
static void mdlInitializeSampleTimes (SimStruct *S)
{
	ssSetSampleTime (S, 0, INHERITED_SAMPLE_TIME);
	ssSetOffsetTime (S, 0, 0.0);
}

/*
 * Сброс процесора, эмулятор начинает работу с начала.
 */
static void cpu_reset (cpu_t *c)
{
	extern void S_FUNCTION_NAME (cpu_t *c);

	c->entry_ebx = (unsigned) S_FUNCTION_NAME;
	c->entry_edi = (unsigned) c;
	c->entry_esp = (unsigned) c + STACK_BYTES - sizeof (unsigned);

	c->dataout = 0;
	c->addr = 0;
	c->rd = 0;
	c->wrh = 0;
	c->wrl = 0;
}

/*
 * This function is called once at start of model execution. If you
 * have states that should be initialized once, this is the place
 * to do it.
 */
#define MDL_START  /* Change to #undef to remove function */

static void mdlStart (SimStruct *S)
{
	cpu_t *c;
	char *name;

	/*printf ("\n*** %s started\n", ssGetPath (S));*/

	/* Выделение памяти для эмулятора, старт потока. */
	c = (cpu_t*) calloc (1, STACK_BYTES);
	if (! c) {
		printf ("*** %s out of memory\n", ssGetPath (S));
		return;
	}
	ssSetPWorkValue (S, 0, c);

	name = strrchr (ssGetPath (S), '/');
	if (name)
		c->name = name + 1;
	else
		c->name = ssGetPath (S);

	__asm__ volatile (
	"	jmp 2f \n"
	/* Subtask */
	"1:	movl %edi, (%esp) \n"
	"	call *%ebx \n"
	"	jmp 1b \n"
	"2:");

	CONTEXT_SAVE (&c->entry_ebp, "1b");

	/* Caller */
	cpu_reset (c);
}

/*
 * Выполнение одного шага процессора.
 */
static void mdlOutputs (SimStruct *S, int_T tid)
{
	cpu_t *c = (cpu_t*) ssGetPWorkValue(S,0);

	/* Входные порты */
	c->datain = *(uint16_T*)  ssGetInputPortSignal (S,0);
	c->ack    = *(boolean_T*) ssGetInputPortSignal (S,1);
	c->rst    = *(boolean_T*) ssGetInputPortSignal (S,2);

	/*printf ("*** %s step\n", c->name);*/
	if (c->rst) {
		/* Сброс процессора. */
		printf ("*** %s reset\n", c->name);
		cpu_reset (c);
	} else {
		/* Переключение на сопроцесс эмулятора. */
		CONTEXT_SAVE (&c->return_ebp, "1f");
		CONTEXT_RESTORE (&c->entry_ebp);
		__asm__ volatile ("1:");
	}

	/* Выходные порты */
	*(uint16_T*)  ssGetOutputPortSignal (S,0) = c->dataout;
	*(uint16_T*)  ssGetOutputPortSignal (S,1) = c->addr;
	*(boolean_T*) ssGetOutputPortSignal (S,2) = c->rd;
	*(boolean_T*) ssGetOutputPortSignal (S,3) = c->wrh;
	*(boolean_T*) ssGetOutputPortSignal (S,4) = c->wrl;
}

/*
 * In this function, you should perform any actions that are necessary
 * at the termination of a simulation.  For example, if memory was
 * allocated in mdlStart, this is the place to free it.
 */
static void mdlTerminate (SimStruct *S)
{
	cpu_t *c = (cpu_t*) ssGetPWorkValue(S,0);

	/*printf ("*** %s terminated\n\n", c->name);*/

	/* Освобождаем ресурсы. */
	free (c);
}

#ifdef  MATLAB_MEX_FILE    /* Is this file being compiled as a MEX-file? */
#include "simulink.c"      /* MEX-file interface mechanism */
#else
#include "cg_sfun.h"       /* Code generation registration function */
#endif

/*
 * Завершение цикла процессора.
 * Передача управления сопрограмме Simulink.
 * После возврата - начало нового цикла процессора.
 */
void cpu_tick (cpu_t *c)
{
	/* Процессор выполнил один шаг и установил значения выходных сигналов. */
	/*printf ("*** %s datain=%x, ack=%d, dataout=%x, addr=%x, rd=%d, wrh=%d, wrl=%d\n",
		c->name, c->datain, c->ack,
		c->dataout, c->addr, c->rd, c->wrh, c->wrl);*/

	/* Останавливаемся и ждём следующего такта.  */
	CONTEXT_SAVE (&c->entry_ebp, "1f");
	CONTEXT_RESTORE (&c->return_ebp);
	__asm__ volatile ("1:");
}

/*
 * Цикл шины: запись слова в память.
 */
void cpu_write (cpu_t *c, unsigned int addr, unsigned int val)
{
	c->addr = addr;
	c->dataout = val;
	cpu_tick (c);

	c->wrh = c->wrl = 1;
	do {
		cpu_tick (c);
	} while (! c->ack);

	c->wrh = c->wrl = 0;
}

void cpu_write32 (cpu_t *c, unsigned int addr, unsigned long val)
{
	cpu_write (c, addr, (unsigned short) val);
	cpu_write (c, addr + 2, (unsigned short) (val >> 16));
}

/*
 * Цикл шины: чтение слова из памяти.
 */
unsigned int cpu_read (cpu_t *c, unsigned int addr)
{
	unsigned int val;

	c->addr = addr;
	cpu_tick (c);

	c->rd = 1;
	do {
		cpu_tick (c);
	} while (! c->ack);

	val = c->datain;
	c->rd = 0;
	return val;
}
