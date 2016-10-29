/*	@(#)msgbuf2.c	1.4	*/
/*	3.0 SID #	1.2	*/
# include	"lerror.h"

char	*msg2text[ NUM2MSGS ] = {
/* [0] */	"name used but not defined",	/* name */
	/* name in filename( lineno ) */
/* [1] */	"name defined but never used",	/* name */
	/* name in filename( lineno ) */
/* [2] */	"name declared but never used or defined",	/* name */
	/* name in filename( lineno ) */
/* [3] */	"name multiply declared",	/* name */
/* [4] */	"value type used inconsistently",	/* name */
/* [5] */	"value type declared inconsistently",	/* name */
/* [6] */	"function argument ( number ) used inconsistently",	/* name and number */
	/* function( argnumber )  filename( lineno ) :: filename( lineno ) */
/* [7] */	"function called with variable number of arguments",	/* name */
/* [8] */	"function value type must be declared before use",	/* name */
/* [9] */	"function value is used, but none returned",	/* name */
/* [10] */	"function returns value which is always ignored",	/* name */
/* [11] */	"function returns value which is sometimes ignored"	/* name */
};


short	msg2type[ NUM2MSGS ] = {
/* [0] */	NMFNLN,
/* [1] */	NMFNLN,
/* [2] */	NMFNLN,
/* [3] */	NM2FNLN,
/* [4] */	NM2FNLN,
/* [5] */	NM2FNLN,
/* [6] */	ND2FNLN,
/* [7] */	NM2FNLN,
/* [8] */	NM2FNLN,
/* [9] */	NMONLY,
/* [10] */	NMONLY,
/* [11] */	NMONLY
};
