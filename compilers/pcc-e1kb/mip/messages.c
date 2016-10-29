/*	@(#)messages.c	1.4	*/
#include	"messages.h"


/* msgtext is an array of pointers to printf format strings used in calls to
 * error message printing functions
 * the index into msgtext of each string is "hardwired" hence when adding new
 * messages they should be appended to the end of msgtext and when deleting
 * messages, they should be replaced by empty strings
 *
 * NUMMSGS is defined to be the number of entries is msgtext and it too is a
 * hardwired constant, defined in messages.h
 */

#ifdef FLEXNAMES
char	*msgtext[ NUMMSGS ] = {
/* [0] */	"%s evaluation order undefined",	/* werror::name */
/* [1] */	"%s may be used before set",	/* werror::name */
/* [2] */	"%s redefinition hides earlier one",	/* uerror::name */
/* [3] */	"%s set but not used in function %s",	/* werror::name,name */
/* [4] */	"%s undefined",	/* uerror::name */
/* [5] */	"bad structure offset",	/* uerror */
/* [6] */	"%s unused in function %s",	/* werror::name,name */
/* [7] */	"& before array or function: ignored",	/* werror */
/* [8] */	"=<%c illegal",	/* uerror::character */
/* [9] */	"=>%c illegal",	/* uerror::character */
/* [10] */	"BCD constant exceeds 6 characters",	/* uerror */
/* [11] */	"a function is declared as an argument",	/* werror */
/* [12] */	"ambiguous assignment: simple assign, unary op assumed",	/* werror */
/* [13] */	"argument %s unused in function %s",	/* werror::name,name */
/* [14] */	"array of functions is illegal",	/* uerror */
/* [15] */	"assignment of different structures",	/* uerror */
/* [16] */	"bad asm construction",	/* uerror */
/* [17] */	"bad scalar initialization",	/* uerror */
/* [18] */	"can't take & of %s",	/* uerror::name */
/* [19] */	"cannot initialize extern or union",	/* uerror */
/* [20] */	"case not in switch",	/* uerror */
/* [21] */	"comparison of unsigned with negative constant",	/* werror */
/* [22] */	"constant argument to NOT",	/* werror */
/* [23] */	"constant expected",	/* uerror */
/* [24] */	"constant in conditional context",	/* werror */
/* [25] */	"constant too big for cross-compiler",	/* uerror */
/* [26] */	"conversion from long may lose accuracy",	/* werror */
/* [27] */	"conversion to long may sign-extend incorrectly",	/* werror */
/* [28] */	"declared argument %s is missing",	/* uerror::name */
/* [29] */	"default not inside switch",	/* uerror */
/* [30] */	"degenerate unsigned comparison",	/* werror */
/* [31] */	"division by 0",	/* uerror */
/* [32] */	"division by 0.",	/* uerror */
/* [33] */	"duplicate case in switch, %d",	/* uerror::number */
/* [34] */	"duplicate default in switch",	/* uerror */
/* [35] */	"empty array declaration",	/* werror */
/* [36] */	"empty character constant",	/* uerror */
/* [37] */	"enumeration type clash, operator %s",	/* werror::operator */
/* [38] */	"field outside of structure",	/* uerror */
/* [39] */	"field too big",	/* uerror */
/* [40] */	"fortran declaration must apply to function",	/* uerror */
/* [41] */	"fortran function has wrong type",	/* uerror */
/* [42] */	"fortran keyword nonportable",	/* werror */
/* [43] */	"function %s has return(e); and return;",	/* werror::name */
/* [44] */	"function declaration in bad context",	/* uerror */
/* [45] */	"function has illegal storage class",	/* uerror */
/* [46] */	"function illegal in structure or union",	/* uerror */
/* [47] */	"function returns illegal type",	/* uerror */
/* [48] */	"gcos BCD constant illegal",	/* uerror */
/* [49] */	"illegal array size combination",	/* werror */
/* [50] */	"illegal break",	/* uerror */
/* [51] */	"illegal character: %03o (octal)",	/* uerror::number */
/* [52] */	"illegal class",	/* uerror */
/* [53] */	"illegal combination of pointer and integer, op %s",	/* werror::operator */
/* [54] */	"illegal comparison of enums",	/* uerror */
/* [55] */	"illegal continue",	/* uerror */
/* [56] */	"illegal field size",	/* uerror */
/* [57] */	"illegal field type",	/* uerror */
/* [58] */	"illegal function",	/* uerror */
/* [59] */	"illegal hex constant",	/* uerror */
/* [60] */	"illegal indirection",	/* uerror */
/* [61] */	"illegal initialization",	/* uerror */
/* [62] */	"illegal lhs of assignment operator",	/* uerror */
/* [63] */	"illegal member use: %s",	/* uerror::name */
/* [64] */	"illegal member use: %s",	/* werror::name */
/* [65] */	"illegal member use: perhaps %s.%s",	/* werror::name,name */
/* [66] */	"illegal pointer combination",	/* werror */
/* [67] */	"illegal pointer subtraction",	/* uerror */
/* [68] */	"illegal register declaration",	/* uerror */
/* [69] */	"illegal structure pointer combination",	/* werror */
/* [70] */	"illegal type combination",	/* uerror */
/* [71] */	"illegal types in :",	/* uerror */
/* [72] */	"illegal use of field",	/* uerror */
/* [73] */	"illegal zero sized structure member: %s",	/* werror::name */
/* [74] */	"illegal {",	/* uerror */
/* [75] */	"loop not entered at top",	/* werror */
/* [76] */	"member of structure or union required",	/* uerror */
/* [77] */	"newline in BCD constant",	/* uerror */
/* [78] */	"newline in string or char constant",	/* uerror */
/* [79] */	"no automatic aggregate initialization",	/* uerror */
/* [80] */	"non-constant case expression",	/* uerror */
/* [81] */	"non-null byte ignored in string initializer",	/* werror */
/* [82] */	"nonportable character comparison",	/* werror */
/* [83] */	"nonportable field type",	/* uerror */
/* [84] */	"nonunique name demands struct/union or struct/union pointer",	/* uerror */
/* [85] */	"null dimension",	/* uerror */
/* [86] */	"null effect",	/* werror */
/* [87] */	"old-fashioned assignment operator",	/* werror */
/* [88] */	"old-fashioned initialization: use =",	/* werror */
/* [89] */	"operands of %s have incompatible types",	/* uerror::operator */
/* [90] */	"pointer required",	/* uerror */
/* [91] */	"possible pointer alignment problem",	/* werror */
/* [92] */	"precedence confusion possible: parenthesize!",	/* werror */
/* [93] */	"precision lost in assignment to (sign-extended?) field",	/* werror */
/* [94] */	"precision lost in field assignment",	/* werror */
/* [95] */	"questionable conversion of function pointer",	/* werror */
/* [96] */	"redeclaration of %s",	/* uerror::name */
/* [97] */	"redeclaration of formal parameter, %s",	/* uerror::name */
/* [98] */  "pointer casts may be troublesome",	/* werror */
/* [99] */	"sizeof returns value less than or equal to zero", /* werror */
/* [100] */	"statement not reached",	/* werror */
/* [101] */	"static variable %s unused",	/* werror::name */
/* [102] */	"struct/union %s never defined",	/* werror::name */
/* [103] */	"struct/union or struct/union pointer required",	/* werror */
/* [104] */	"structure %s never defined",	/* werror::name */
/* [105] */	"structure reference must be addressable",	/* uerror */
/* [106] */	"structure typed union member must be named",	/* werror */
/* [107] */	"too many characters in character constant",	/* uerror */
/* [108] */	"too many initializers",	/* uerror */
/* [109] */	"type clash in conditional",	/* uerror */
/* [110] */	"unacceptable operand of &",	/* uerror */
/* [111] */	"undeclared initializer name %s",	/* werror::name */
/* [112] */	"undefined structure or union",	/* uerror */
/* [113] */	"unexpected EOF",	/* uerror */
/* [114] */	"unknown size",	/* uerror */
/* [115] */	"unsigned comparison with 0?",	/* werror */
/* [116] */	"void function %s cannot return value",	/* uerror::name */
/* [117] */	"void type for %s",	/* uerror::name */
/* [118] */	"void type illegal in expression",	/* uerror */
/* [119] */	"zero or negative subscript",	/* werror */
/* [120] */	"zero size field",	/* uerror */
/* [121] */	"zero sized structure",	/* uerror */
/* [122] */	"} expected",	/* uerror */
/* [123] */	"long in case or switch statement may be truncated",	/* werror */
/* [124] */	"bad octal digit %c",	/* werror */
/* [125] */	"floating point constant folding causes exception",	/* uerror */
/* [126] */	"old style assign-op causes syntax error",	/* werror */
/* [127] */	"main() returns random value to invocation environment",	/* werror */
/* [128] */	"`%s' may be indistinguishable from `%s' due to internal name truncation"	/* werror::name,name */
};
#else
char	*msgtext[ NUMMSGS ] = {
/* [0] */	"%.8s evaluation order undefined",	/* werror::name */
/* [1] */	"%.8s may be used before set",	/* werror::name */
/* [2] */	"%.8s redefinition hides earlier one",	/* uerror::name */
/* [3] */	"%.8s set but not used in function %.8s",	/* werror::name,name */
/* [4] */	"%.8s undefined",	/* uerror::name */
/* [5] */	"bad structure offset",	/* uerror */
/* [6] */	"%.8s unused in function %.8s",	/* werror::name,name */
/* [7] */	"& before array or function: ignored",	/* werror */
/* [8] */	"=<%c illegal",	/* uerror::character */
/* [9] */	"=>%c illegal",	/* uerror::character */
/* [10] */	"BCD constant exceeds 6 characters",	/* uerror */
/* [11] */	"a function is declared as an argument",	/* werror */
/* [12] */	"ambiguous assignment: simple assign, unary op assumed",	/* werror */
/* [13] */	"argument %.8s unused in function %.8s",	/* werror::name,name */
/* [14] */	"array of functions is illegal",	/* uerror */
/* [15] */	"assignment of different structures",	/* uerror */
/* [16] */	"bad asm construction",	/* uerror */
/* [17] */	"bad scalar initialization",	/* uerror */
/* [18] */	"can't take & of %.8s",	/* uerror::name */
/* [19] */	"cannot initialize extern or union",	/* uerror */
/* [20] */	"case not in switch",	/* uerror */
/* [21] */	"comparison of unsigned with negative constant",	/* werror */
/* [22] */	"constant argument to NOT",	/* werror */
/* [23] */	"constant expected",	/* uerror */
/* [24] */	"constant in conditional context",	/* werror */
/* [25] */	"constant too big for cross-compiler",	/* uerror */
/* [26] */	"conversion from long may lose accuracy",	/* werror */
/* [27] */	"conversion to long may sign-extend incorrectly",	/* werror */
/* [28] */	"declared argument %.8s is missing",	/* uerror::name */
/* [29] */	"default not inside switch",	/* uerror */
/* [30] */	"degenerate unsigned comparison",	/* werror */
/* [31] */	"division by 0",	/* uerror */
/* [32] */	"division by 0.",	/* uerror */
/* [33] */	"duplicate case in switch, %d",	/* uerror::number */
/* [34] */	"duplicate default in switch",	/* uerror */
/* [35] */	"empty array declaration",	/* werror */
/* [36] */	"empty character constant",	/* uerror */
/* [37] */	"enumeration type clash, operator %s",	/* werror::operator */
/* [38] */	"field outside of structure",	/* uerror */
/* [39] */	"field too big",	/* uerror */
/* [40] */	"fortran declaration must apply to function",	/* uerror */
/* [41] */	"fortran function has wrong type",	/* uerror */
/* [42] */	"fortran keyword nonportable",	/* werror */
/* [43] */	"function %.8s has return(e); and return;",	/* werror::name */
/* [44] */	"function declaration in bad context",	/* uerror */
/* [45] */	"function has illegal storage class",	/* uerror */
/* [46] */	"function illegal in structure or union",	/* uerror */
/* [47] */	"function returns illegal type",	/* uerror */
/* [48] */	"gcos BCD constant illegal",	/* uerror */
/* [49] */	"illegal array size combination",	/* werror */
/* [50] */	"illegal break",	/* uerror */
/* [51] */	"illegal character: %03o (octal)",	/* uerror::number */
/* [52] */	"illegal class",	/* uerror */
/* [53] */	"illegal combination of pointer and integer, op %s",	/* werror::operator */
/* [54] */	"illegal comparison of enums",	/* uerror */
/* [55] */	"illegal continue",	/* uerror */
/* [56] */	"illegal field size",	/* uerror */
/* [57] */	"illegal field type",	/* uerror */
/* [58] */	"illegal function",	/* uerror */
/* [59] */	"illegal hex constant",	/* uerror */
/* [60] */	"illegal indirection",	/* uerror */
/* [61] */	"illegal initialization",	/* uerror */
/* [62] */	"illegal lhs of assignment operator",	/* uerror */
/* [63] */	"illegal member use: %.8s",	/* uerror::name */
/* [64] */	"illegal member use: %.8s",	/* werror::name */
/* [65] */	"illegal member use: perhaps %.8s.%.8s",	/* werror::name,name */
/* [66] */	"illegal pointer combination",	/* werror */
/* [67] */	"illegal pointer subtraction",	/* uerror */
/* [68] */	"illegal register declaration",	/* uerror */
/* [69] */	"illegal structure pointer combination",	/* werror */
/* [70] */	"illegal type combination",	/* uerror */
/* [71] */	"illegal types in :",	/* uerror */
/* [72] */	"illegal use of field",	/* uerror */
/* [73] */	"illegal zero sized structure member: %.8s",	/* werror::name */
/* [74] */	"illegal {",	/* uerror */
/* [75] */	"loop not entered at top",	/* werror */
/* [76] */	"member of structure or union required",	/* uerror */
/* [77] */	"newline in BCD constant",	/* uerror */
/* [78] */	"newline in string or char constant",	/* uerror */
/* [79] */	"no automatic aggregate initialization",	/* uerror */
/* [80] */	"non-constant case expression",	/* uerror */
/* [81] */	"non-null byte ignored in string initializer",	/* werror */
/* [82] */	"nonportable character comparison",	/* werror */
/* [83] */	"nonportable field type",	/* uerror */
/* [84] */	"nonunique name demands struct/union or struct/union pointer",	/* uerror */
/* [85] */	"null dimension",	/* uerror */
/* [86] */	"null effect",	/* werror */
/* [87] */	"old-fashioned assignment operator",	/* werror */
/* [88] */	"old-fashioned initialization: use =",	/* werror */
/* [89] */	"operands of %s have incompatible types",	/* uerror::operator */
/* [90] */	"pointer required",	/* uerror */
/* [91] */	"possible pointer alignment problem",	/* werror */
/* [92] */	"precedence confusion possible: parenthesize!",	/* werror */
/* [93] */	"precision lost in assignment to (sign-extended?) field",	/* werror */
/* [94] */	"precision lost in field assignment",	/* werror */
/* [95] */	"questionable conversion of function pointer",	/* werror */
/* [96] */	"redeclaration of %.8s",	/* uerror::name */
/* [97] */	"redeclaration of formal parameter, %.8s",	/* uerror::name */
/* [98] */  "pointer casts may be troublesome",	/* werror */
/* [99] */	"sizeof returns value less than or equal to zero", /* werror */
/* [100] */	"statement not reached",	/* werror */
/* [101] */	"static variable %.8s unused",	/* werror::name */
/* [102] */	"struct/union %.8s never defined",	/* werror::name */
/* [103] */	"struct/union or struct/union pointer required",	/* werror */
/* [104] */	"structure %.8s never defined",	/* werror::name */
/* [105] */	"structure reference must be addressable",	/* uerror */
/* [106] */	"structure typed union member must be named",	/* werror */
/* [107] */	"too many characters in character constant",	/* uerror */
/* [108] */	"too many initializers",	/* uerror */
/* [109] */	"type clash in conditional",	/* uerror */
/* [110] */	"unacceptable operand of &",	/* uerror */
/* [111] */	"undeclared initializer name %.8s",	/* werror::name */
/* [112] */	"undefined structure or union",	/* uerror */
/* [113] */	"unexpected EOF",	/* uerror */
/* [114] */	"unknown size",	/* uerror */
/* [115] */	"unsigned comparison with 0?",	/* werror */
/* [116] */	"void function %.8s cannot return value",	/* uerror::name */
/* [117] */	"void type for %.8s",	/* uerror::name */
/* [118] */	"void type illegal in expression",	/* uerror */
/* [119] */	"zero or negative subscript",	/* werror */
/* [120] */	"zero size field",	/* uerror */
/* [121] */	"zero sized structure",	/* uerror */
/* [122] */	"} expected",	/* uerror */
/* [123] */	"long in case or switch statement may be truncated",	/* werror */
/* [124] */	"bad octal digit %c",	/* werror */
/* [125] */	"floating point constant folding causes exception",	/* uerror */
/* [126] */	"old style assign-op causes syntax error",	/* werror */
/* [127] */	"main() returns random value to invocation environment"	/* werror */
};
#endif
