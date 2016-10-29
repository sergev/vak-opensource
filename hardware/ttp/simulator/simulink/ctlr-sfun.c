/*
 * S-функция для Simulink - эмуляция контроллера с 16-битной шиной адрес/данные.
 *
 * Автор: Сергей Вакуленко, ИТМиВТ 2008.
 */
#define S_FUNCTION_NAME ctlr
#define S_FUNCTION_LEVEL 2

#include "simstruc.h"
#include "mex.h"
#include "ctlr.h"

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
	if (! ssSetNumInputPorts (S, 7))
		return;
	set_input_port (S, 0, SS_UINT16);	/* datain */
	set_input_port (S, 1, SS_UINT16);	/* addr */
	set_input_port (S, 2, SS_BOOLEAN);	/* rd */
	set_input_port (S, 3, SS_BOOLEAN);	/* wrh */
	set_input_port (S, 4, SS_BOOLEAN);	/* wrl */
	set_input_port (S, 5, SS_BOOLEAN);	/* rx0 */
	set_input_port (S, 6, SS_BOOLEAN);	/* rx1 */

	/*
	 * Выходы.
	 */
	if (! ssSetNumOutputPorts (S, 4))
		return;
	set_output_port (S, 0, SS_UINT16);	/* dataout */
	set_output_port (S, 1, SS_BOOLEAN);	/* ack */
	set_output_port (S, 2, SS_BOOLEAN);	/* tx0 */
	set_output_port (S, 3, SS_BOOLEAN);	/* tx1 */

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

#define MDL_START  /* Change to #undef to remove function */
#if defined(MDL_START)
/*
 * This function is called once at start of model execution. If you
 * have states that should be initialized once, this is the place
 * to do it.
 */
static void mdlStart (SimStruct *S)
{
	controller_t *c;
	char *name;

	c = ctlr_alloc ();
	ssSetPWorkValue (S, 0, c);

	name = strrchr (ssGetPath (S), '/');
	if (name)
		c->name = name + 1;
	else
		c->name = ssGetPath (S);
}
#endif /* MDL_START */

/*
 *
 */
static void mdlOutputs (SimStruct *S, int_T tid)
{
	controller_t *c = (controller_t*) ssGetPWorkValue(S,0);

	/* Входные порты */
	c->datain = *(uint16_T*)  ssGetInputPortSignal (S,0);
	c->addr   = *(uint16_T*)  ssGetInputPortSignal (S,1);
	c->rd     = *(boolean_T*) ssGetInputPortSignal (S,2);
	c->wrh    = *(boolean_T*) ssGetInputPortSignal (S,3);
	c->wrl    = *(boolean_T*) ssGetInputPortSignal (S,4);
	c->rx0    = *(boolean_T*) ssGetInputPortSignal (S,5);
	c->rx1    = *(boolean_T*) ssGetInputPortSignal (S,6);

	ctlr_step_cpu (c);
	ctlr_step_rxtx (c);

	/* Выходные порты */
	*(uint16_T*)  ssGetOutputPortSignal (S,0) = c->dataout;
	*(boolean_T*) ssGetOutputPortSignal (S,1) = c->ack;
	*(boolean_T*) ssGetOutputPortSignal (S,2) = c->tx;
	*(boolean_T*) ssGetOutputPortSignal (S,3) = c->tx;
}

/*
 * In this function, you should perform any actions that are necessary
 * at the termination of a simulation.  For example, if memory was
 * allocated in mdlStart, this is the place to free it.
 */
static void mdlTerminate (SimStruct *S)
{
	controller_t *c = (controller_t*) ssGetPWorkValue(S,0);

	ctlr_free (c);
}

#ifdef  MATLAB_MEX_FILE    /* Is this file being compiled as a MEX-file? */
#include "simulink.c"      /* MEX-file interface mechanism */
#else
#include "cg_sfun.h"       /* Code generation registration function */
#endif
