/*
 * S-функция для Simulink - эмуляция шины Manchester/RS-485.
 *
 * Автор: Сергей Вакуленко, ИТМиВТ 2008.
 */
#define S_FUNCTION_NAME bus
#define S_FUNCTION_LEVEL 2

#include "simstruc.h"
#include "mex.h"
#include "bus.h"

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
	if (! ssSetNumInputPorts (S, 4))
		return;
	set_input_port (S, 0, SS_BOOLEAN);	/* rn0 */
	set_input_port (S, 1, SS_BOOLEAN);	/* rn1 */
	set_input_port (S, 2, SS_BOOLEAN);	/* rn2 */
	set_input_port (S, 3, SS_BOOLEAN);	/* rn3 */

	/*
	 * Выходы.
	 */
	if (! ssSetNumOutputPorts (S, 1))
		return;
	set_output_port (S, 0, SS_BOOLEAN);	/* tx */

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
	bus_t *c;
	char *name;

	c = bus_alloc ();
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
	bus_t *c = (bus_t*) ssGetPWorkValue(S,0);

	/* Входные порты */
	c->rn0    = *(boolean_T*) ssGetInputPortSignal (S,0);
	c->rn1    = *(boolean_T*) ssGetInputPortSignal (S,1);
	c->rn2    = *(boolean_T*) ssGetInputPortSignal (S,2);
	c->rn3    = *(boolean_T*) ssGetInputPortSignal (S,3);

	bus_step (c);

	/* Выходные порты */
	*(boolean_T*) ssGetOutputPortSignal (S,0) = c->tx;
}

/*
 * In this function, you should perform any actions that are necessary
 * at the termination of a simulation.  For example, if memory was
 * allocated in mdlStart, this is the place to free it.
 */
static void mdlTerminate (SimStruct *S)
{
	bus_t *c = (bus_t*) ssGetPWorkValue(S,0);

	bus_free (c);
}

#ifdef  MATLAB_MEX_FILE    /* Is this file being compiled as a MEX-file? */
#include "simulink.c"      /* MEX-file interface mechanism */
#else
#include "cg_sfun.h"       /* Code generation registration function */
#endif
