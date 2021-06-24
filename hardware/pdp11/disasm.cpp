#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <string>

/*
 * PDP-11 opcode types.
 */
#define OPCODE_NO_OPS		0
#define OPCODE_REG		1	/* register */
#define OPCODE_OP		2	/* generic operand */
#define OPCODE_REG_OP		3	/* register and generic operand */
#define OPCODE_REG_OP_REV	4	/* register and generic operand,
					   reversed syntax */
#define OPCODE_AC_FOP		5	/* fpu accumulator and generic float
					   operand */
#define OPCODE_OP_OP		6	/* two generic operands */
#define OPCODE_DISPL		7	/* pc-relative displacement */
#define OPCODE_REG_DISPL	8	/* redister and pc-relative
					   displacement */
#define OPCODE_IMM8		9	/* 8-bit immediate */
#define OPCODE_IMM6		10	/* 6-bit immediate */
#define OPCODE_IMM3		11	/* 3-bit immediate */
#define OPCODE_ILLEGAL		12	/* illegal instruction */
#define OPCODE_FOP_AC		13	/* generic float argument, then fpu
					   accumulator */
#define OPCODE_FOP		14	/* generic float operand */
#define OPCODE_AC_OP		15	/* fpu accumulator and generic int
					   operand */
#define OPCODE_OP_AC		16	/* generic int argument, then fpu
					   accumulator */

/*
 * PDP-11 instruction set extensions.
 */
#define NONE		0	/* not in instruction set */
#define BASIC		1	/* basic instruction set (11/20 etc) */
#define CSM		2	/* commercial instruction set */
#define CIS		3	/* commercial instruction set */
#define EIS		4	/* extended instruction set (11/45 etc) */
#define FIS		5	/* KEV11 floating-point instructions */
#define FPP		6	/* FP-11 floating-point instructions */
#define LEIS		7	/* limited extended instruction set (11/40 etc) */
#define MFPT		8	/* move from processor type */
#define MPROC		9	/* multiprocessor instructions: tstset, wrtlck */
#define MXPS		10	/* move from/to processor status */
#define SPL		11	/* set priority level */
#define UCODE		12	/* microcode instructions: ldub, med, xfc */
#define EXT_NUM		13	/* total number of extension types */

struct opcode {
    const char *name;
    unsigned opcode;
    unsigned mask;
    int type;
    int extension;
};

static const struct opcode op[] = {
  /* name,	pattern, mask,	opcode type,		insn type,    alias */
  { "halt",	0000000, 0177777, OPCODE_NO_OPS,	BASIC },
  { "wait",	0000001, 0177777, OPCODE_NO_OPS,	BASIC },
  { "rti",	0000002, 0177777, OPCODE_NO_OPS,	BASIC },
  { "bpt",	0000003, 0177777, OPCODE_NO_OPS,	BASIC },
  { "iot",	0000004, 0177777, OPCODE_NO_OPS,	BASIC },
  { "reset",	0000005, 0177777, OPCODE_NO_OPS,	BASIC },
  { "rtt",	0000006, 0177777, OPCODE_NO_OPS,	LEIS },
  { "mfpt",	0000007, 0177777, OPCODE_NO_OPS,	MFPT },
  { "jmp",	0000100, 0177700, OPCODE_OP,		BASIC },
  { "rts",	0000200, 0177770, OPCODE_REG,		BASIC },
  { "",		0000210, 0177770, OPCODE_ILLEGAL,	NONE },
  { "",		0000220, 0177770, OPCODE_ILLEGAL,	NONE },
  { "spl",	0000230, 0177770, OPCODE_IMM3,		SPL },
  { "nop",	0000240, 0177777, OPCODE_NO_OPS,	BASIC },
  { "clc",	0000241, 0177777, OPCODE_NO_OPS,	BASIC },
  { "clv",	0000242, 0177777, OPCODE_NO_OPS,	BASIC },
  { "cl_3",	0000243, 0177777, OPCODE_NO_OPS,	BASIC },
  { "clz",	0000244, 0177777, OPCODE_NO_OPS,	BASIC },
  { "cl_5",	0000245, 0177777, OPCODE_NO_OPS,	BASIC },
  { "cl_6",	0000246, 0177777, OPCODE_NO_OPS,	BASIC },
  { "cl_7",	0000247, 0177777, OPCODE_NO_OPS,	BASIC },
  { "cln",	0000250, 0177777, OPCODE_NO_OPS,	BASIC },
  { "cl_9",	0000251, 0177777, OPCODE_NO_OPS,	BASIC },
  { "cl_a",	0000252, 0177777, OPCODE_NO_OPS,	BASIC },
  { "cl_b",	0000253, 0177777, OPCODE_NO_OPS,	BASIC },
  { "cl_c",	0000254, 0177777, OPCODE_NO_OPS,	BASIC },
  { "cl_d",	0000255, 0177777, OPCODE_NO_OPS,	BASIC },
  { "cl_e",	0000256, 0177777, OPCODE_NO_OPS,	BASIC },
  { "ccc",	0000257, 0177777, OPCODE_NO_OPS,	BASIC },
  { "se_0",	0000260, 0177777, OPCODE_NO_OPS,	BASIC },
  { "sec",	0000241, 0177777, OPCODE_NO_OPS,	BASIC },
  { "sev",	0000262, 0177777, OPCODE_NO_OPS,	BASIC },
  { "se_3",	0000263, 0177777, OPCODE_NO_OPS,	BASIC },
  { "sez",	0000264, 0177777, OPCODE_NO_OPS,	BASIC },
  { "se_5",	0000265, 0177777, OPCODE_NO_OPS,	BASIC },
  { "se_6",	0000266, 0177777, OPCODE_NO_OPS,	BASIC },
  { "se_7",	0000267, 0177777, OPCODE_NO_OPS,	BASIC },
  { "sen",	0000270, 0177777, OPCODE_NO_OPS,	BASIC },
  { "se_9",	0000271, 0177777, OPCODE_NO_OPS,	BASIC },
  { "se_a",	0000272, 0177777, OPCODE_NO_OPS,	BASIC },
  { "se_b",	0000273, 0177777, OPCODE_NO_OPS,	BASIC },
  { "se_c",	0000274, 0177777, OPCODE_NO_OPS,	BASIC },
  { "se_d",	0000275, 0177777, OPCODE_NO_OPS,	BASIC },
  { "se_e",	0000276, 0177777, OPCODE_NO_OPS,	BASIC },
  { "scc",	0000277, 0177777, OPCODE_NO_OPS,	BASIC },
  { "swab",	0000300, 0177700, OPCODE_OP,		BASIC },
  { "br",	0000400, 0177400, OPCODE_DISPL,		BASIC },
  { "bne",	0001000, 0177400, OPCODE_DISPL,		BASIC },
  { "beq",	0001400, 0177400, OPCODE_DISPL,		BASIC },
  { "bge",	0002000, 0177400, OPCODE_DISPL,		BASIC },
  { "blt",	0002400, 0177400, OPCODE_DISPL,		BASIC },
  { "bgt",	0003000, 0177400, OPCODE_DISPL,		BASIC },
  { "ble",	0003400, 0177400, OPCODE_DISPL,		BASIC },
  { "jsr",	0004000, 0177000, OPCODE_REG_OP,	BASIC },
  { "clr",	0005000, 0177700, OPCODE_OP,		BASIC },
  { "com",	0005100, 0177700, OPCODE_OP,		BASIC },
  { "inc",	0005200, 0177700, OPCODE_OP,		BASIC },
  { "dec",	0005300, 0177700, OPCODE_OP,		BASIC },
  { "neg",	0005400, 0177700, OPCODE_OP,		BASIC },
  { "adc",	0005500, 0177700, OPCODE_OP,		BASIC },
  { "sbc",	0005600, 0177700, OPCODE_OP,		BASIC },
  { "tst",	0005700, 0177700, OPCODE_OP,		BASIC },
  { "ror",	0006000, 0177700, OPCODE_OP,		BASIC },
  { "rol",	0006100, 0177700, OPCODE_OP,		BASIC },
  { "asr",	0006200, 0177700, OPCODE_OP,		BASIC },
  { "asl",	0006300, 0177700, OPCODE_OP,		BASIC },
  { "mark",	0006400, 0177700, OPCODE_IMM6,		LEIS },
  { "mfpi",	0006500, 0177700, OPCODE_OP,		BASIC },
  { "mtpi",	0006600, 0177700, OPCODE_OP,		BASIC },
  { "sxt",	0006700, 0177700, OPCODE_OP,		LEIS },
  { "csm",	0007000, 0177700, OPCODE_OP,		CSM },
  { "tstset",	0007100, 0177700, OPCODE_OP,		MPROC },
  { "wrtlck",	0007200, 0177700, OPCODE_OP,		MPROC },
/*{ "",		0007300, 0177740, OPCODE_ILLEGAL,	NONE },*/
  { "mov",	0010000, 0170000, OPCODE_OP_OP,		BASIC },
  { "cmp",	0020000, 0170000, OPCODE_OP_OP,		BASIC },
  { "bit",	0030000, 0170000, OPCODE_OP_OP,		BASIC },
  { "bic",	0040000, 0170000, OPCODE_OP_OP,		BASIC },
  { "bis",	0050000, 0170000, OPCODE_OP_OP,		BASIC },
  { "add",	0060000, 0170000, OPCODE_OP_OP,		BASIC },
  { "mul",	0070000, 0177000, OPCODE_REG_OP_REV,	EIS },
  { "div",	0071000, 0177000, OPCODE_REG_OP_REV,	EIS },
  { "ash",	0072000, 0177000, OPCODE_REG_OP_REV,	EIS },
  { "ashc",	0073000, 0177000, OPCODE_REG_OP_REV,	EIS },
  { "xor",	0074000, 0177000, OPCODE_REG_OP,	LEIS },
  { "fadd",	0075000, 0177770, OPCODE_REG,		FIS },
  { "fsub",	0075010, 0177770, OPCODE_REG,		FIS },
  { "fmul",	0075020, 0177770, OPCODE_REG,		FIS },
  { "fdiv",	0075030, 0177770, OPCODE_REG,		FIS },
/*{ "",		0075040, 0177740, OPCODE_ILLEGAL,	NONE },*/
/*{ "",		0075100, 0177700, OPCODE_ILLEGAL,	NONE },*/
/*{ "",		0075200, 0177600, OPCODE_ILLEGAL,	NONE },*/
/*{ "",		0075400, 0177740, OPCODE_ILLEGAL,	NONE },*/
  { "l2dr",	0076020, 0177770, OPCODE_REG,		CIS },	/*l2d*/
  { "movc",	0076030, 0177777, OPCODE_NO_OPS,	CIS },
  { "movrc",	0076031, 0177777, OPCODE_NO_OPS,	CIS },
  { "movtc",	0076032, 0177777, OPCODE_NO_OPS,	CIS },
  { "locc",	0076040, 0177777, OPCODE_NO_OPS,	CIS },
  { "skpc",	0076041, 0177777, OPCODE_NO_OPS,	CIS },
  { "scanc",	0076042, 0177777, OPCODE_NO_OPS,	CIS },
  { "spanc",	0076043, 0177777, OPCODE_NO_OPS,	CIS },
  { "cmpc",	0076044, 0177777, OPCODE_NO_OPS,	CIS },
  { "matc",	0076045, 0177777, OPCODE_NO_OPS,	CIS },
  { "addn",	0076050, 0177777, OPCODE_NO_OPS,	CIS },
  { "subn",	0076051, 0177777, OPCODE_NO_OPS,	CIS },
  { "cmpn",	0076052, 0177777, OPCODE_NO_OPS,	CIS },
  { "cvtnl",	0076053, 0177777, OPCODE_NO_OPS,	CIS },
  { "cvtpn",	0076054, 0177777, OPCODE_NO_OPS,	CIS },
  { "cvtnp",	0076055, 0177777, OPCODE_NO_OPS,	CIS },
  { "ashn",	0076056, 0177777, OPCODE_NO_OPS,	CIS },
  { "cvtln",	0076057, 0177777, OPCODE_NO_OPS,	CIS },
  { "l3dr",	0076060, 0177770, OPCODE_REG,		CIS },	/*l3d*/
  { "addp",	0076070, 0177777, OPCODE_NO_OPS,	CIS },
  { "subp",	0076071, 0177777, OPCODE_NO_OPS,	CIS },
  { "cmpp",	0076072, 0177777, OPCODE_NO_OPS,	CIS },
  { "cvtpl",	0076073, 0177777, OPCODE_NO_OPS,	CIS },
  { "mulp",	0076074, 0177777, OPCODE_NO_OPS,	CIS },
  { "divp",	0076075, 0177777, OPCODE_NO_OPS,	CIS },
  { "ashp",	0076076, 0177777, OPCODE_NO_OPS,	CIS },
  { "cvtlp",	0076077, 0177777, OPCODE_NO_OPS,	CIS },
  { "movci",	0076130, 0177777, OPCODE_NO_OPS,	CIS },
  { "movrci",	0076131, 0177777, OPCODE_NO_OPS,	CIS },
  { "movtci",	0076132, 0177777, OPCODE_NO_OPS,	CIS },
  { "locci",	0076140, 0177777, OPCODE_NO_OPS,	CIS },
  { "skpci",	0076141, 0177777, OPCODE_NO_OPS,	CIS },
  { "scanci",	0076142, 0177777, OPCODE_NO_OPS,	CIS },
  { "spanci",	0076143, 0177777, OPCODE_NO_OPS,	CIS },
  { "cmpci",	0076144, 0177777, OPCODE_NO_OPS,	CIS },
  { "matci",	0076145, 0177777, OPCODE_NO_OPS,	CIS },
  { "addni",	0076150, 0177777, OPCODE_NO_OPS,	CIS },
  { "subni",	0076151, 0177777, OPCODE_NO_OPS,	CIS },
  { "cmpni",	0076152, 0177777, OPCODE_NO_OPS,	CIS },
  { "cvtnli",	0076153, 0177777, OPCODE_NO_OPS,	CIS },
  { "cvtpni",	0076154, 0177777, OPCODE_NO_OPS,	CIS },
  { "cvtnpi",	0076155, 0177777, OPCODE_NO_OPS,	CIS },
  { "ashni",	0076156, 0177777, OPCODE_NO_OPS,	CIS },
  { "cvtlni",	0076157, 0177777, OPCODE_NO_OPS,	CIS },
  { "addpi",	0076170, 0177777, OPCODE_NO_OPS,	CIS },
  { "subpi",	0076171, 0177777, OPCODE_NO_OPS,	CIS },
  { "cmppi",	0076172, 0177777, OPCODE_NO_OPS,	CIS },
  { "cvtpli",	0076173, 0177777, OPCODE_NO_OPS,	CIS },
  { "mulpi",	0076174, 0177777, OPCODE_NO_OPS,	CIS },
  { "divpi",	0076175, 0177777, OPCODE_NO_OPS,	CIS },
  { "ashpi",	0076176, 0177777, OPCODE_NO_OPS,	CIS },
  { "cvtlpi",	0076177, 0177777, OPCODE_NO_OPS,	CIS },
  { "med",	0076600, 0177777, OPCODE_NO_OPS,	UCODE },
  { "xfc",	0076700, 0177700, OPCODE_IMM6,		UCODE },
  { "sob",	0077000, 0177000, OPCODE_REG_DISPL,	LEIS },
  { "bpl",	0100000, 0177400, OPCODE_DISPL,		BASIC },
  { "bmi",	0100400, 0177400, OPCODE_DISPL,		BASIC },
  { "bhi",	0101000, 0177400, OPCODE_DISPL,		BASIC },
  { "blos",	0101400, 0177400, OPCODE_DISPL,		BASIC },
  { "bvc",	0102000, 0177400, OPCODE_DISPL,		BASIC },
  { "bvs",	0102400, 0177400, OPCODE_DISPL,		BASIC },
  { "bcc",	0103000, 0177400, OPCODE_DISPL,		BASIC },/*bhis*/
  { "bcs",	0103400, 0177400, OPCODE_DISPL,		BASIC },/*blo*/
  { "emt",	0104000, 0177400, OPCODE_IMM8,		BASIC },
  { "sys",	0104400, 0177400, OPCODE_IMM8,		BASIC },/*trap*/
  { "clrb",	0105000, 0177700, OPCODE_OP,		BASIC },
  { "comb",	0105100, 0177700, OPCODE_OP,		BASIC },
  { "incb",	0105200, 0177700, OPCODE_OP,		BASIC },
  { "decb",	0105300, 0177700, OPCODE_OP,		BASIC },
  { "negb",	0105400, 0177700, OPCODE_OP,		BASIC },
  { "adcb",	0105500, 0177700, OPCODE_OP,		BASIC },
  { "sbcb",	0105600, 0177700, OPCODE_OP,		BASIC },
  { "tstb",	0105700, 0177700, OPCODE_OP,		BASIC },
  { "rorb",	0106000, 0177700, OPCODE_OP,		BASIC },
  { "rolb",	0106100, 0177700, OPCODE_OP,		BASIC },
  { "asrb",	0106200, 0177700, OPCODE_OP,		BASIC },
  { "aslb",	0106300, 0177700, OPCODE_OP,		BASIC },
  { "mtps",	0106400, 0177700, OPCODE_OP,		MXPS },
  { "mfpd",	0106500, 0177700, OPCODE_OP,		BASIC },
  { "mtpd",	0106600, 0177700, OPCODE_OP,		BASIC },
  { "mfps",	0106700, 0177700, OPCODE_OP,		MXPS },
  { "movb",	0110000, 0170000, OPCODE_OP_OP,		BASIC },
  { "cmpb",	0120000, 0170000, OPCODE_OP_OP,		BASIC },
  { "bitb",	0130000, 0170000, OPCODE_OP_OP,		BASIC },
  { "bicb",	0140000, 0170000, OPCODE_OP_OP,		BASIC },
  { "bisb",	0150000, 0170000, OPCODE_OP_OP,		BASIC },
  { "sub",	0160000, 0170000, OPCODE_OP_OP,		BASIC },
  { "cfcc",	0170000, 0177777, OPCODE_NO_OPS,		FPP },
  { "setf",	0170001, 0177777, OPCODE_NO_OPS,		FPP },
  { "seti",	0170002, 0177777, OPCODE_NO_OPS,		FPP },
  { "ldub",	0170003, 0177777, OPCODE_NO_OPS,		UCODE },
  /* fpp trap	0170004..0170010 */
  { "setd",	0170011, 0177777, OPCODE_NO_OPS,		FPP },
  { "setl",	0170012, 0177777, OPCODE_NO_OPS,		FPP },
  /* fpp trap	0170013..0170077 */
  { "ldfps",	0170100, 0177700, OPCODE_OP,		FPP },
  { "stfps",	0170200, 0177700, OPCODE_OP,		FPP },
  { "stst",	0170300, 0177700, OPCODE_OP,		FPP },
  { "clrf",	0170400, 0177700, OPCODE_FOP,		FPP },
  { "tstf",	0170500, 0177700, OPCODE_FOP,		FPP },
  { "absf",	0170600, 0177700, OPCODE_FOP,		FPP },
  { "negf",	0170700, 0177700, OPCODE_FOP,		FPP },
  { "mulf",	0171000, 0177400, OPCODE_FOP_AC,		FPP },
  { "modf",	0171400, 0177400, OPCODE_FOP_AC,		FPP },
  { "addf",	0172000, 0177400, OPCODE_FOP_AC,		FPP },
  { "ldf",	0172400, 0177400, OPCODE_FOP_AC,		FPP },	/*movif*/
  { "subf",	0173000, 0177400, OPCODE_FOP_AC,		FPP },
  { "cmpf",	0173400, 0177400, OPCODE_FOP_AC,		FPP },
  { "stf",	0174000, 0177400, OPCODE_AC_FOP,		FPP },	/*movfi*/
  { "divf",	0174400, 0177400, OPCODE_FOP_AC,		FPP },
  { "stexp",	0175000, 0177400, OPCODE_AC_OP,		FPP },
  { "stcfi",	0175400, 0177400, OPCODE_AC_OP,		FPP },
  { "stcff",	0176000, 0177400, OPCODE_AC_FOP,		FPP },	/* ? */
  { "ldexp",	0176400, 0177400, OPCODE_OP_AC,		FPP },
  { "ldcif",	0177000, 0177400, OPCODE_OP_AC,		FPP },
  { "ldcff",	0177400, 0177400, OPCODE_FOP_AC,		FPP },	/* ? */
/* This entry MUST be last; it is a "catch-all" entry that will match when no
 * other opcode entry matches during disassembly.
 */
  { "",		0x0000, 0x0000, OPCODE_ILLEGAL,		NONE },
};
#define AFTER_INSTRUCTION " "
#define OPERAND_SEPARATOR ", "

#define JUMP		0x1000	/* flag that this operand is used in a jump */

/* sign-extend a 16-bit number in an int */
#define SIGN_BITS	(8 * sizeof (int) - 16)
#define sign_extend(x)	(((int)(x) << SIGN_BITS) >> SIGN_BITS)

static char buf [60];
static char *bufptr;

static void append (const std::string &fmt, ...)
{
    va_list args;

    va_start (args, fmt);
    bufptr += vsprintf (bufptr, fmt.c_str(), args);
    va_end (args);
}

/*
 * Print integer register name.
 */
static void prreg (int reg)
{
    reg &= 7;
    switch (reg) {
    case 0: case 1: case 2: case 3: case 4: case 5:
        append ("r%d", reg);
        break;
    case 6:
        append ("sp");
        break;
    case 7:
        append ("pc");
        break;
    default:
        append ("r?");
        break;
    }
}

static void praddr (unsigned address)
{
/*  if (address < 20 && sign_extend(address) > -20)
        append ("%d", address);
    else */
        append ("%#o", address & 0xffff);
}

/*
 * Print src or dst operand.
 * Return 0 on error.
 */
static void properand (unsigned memaddr, int code, unsigned argcode)
{
    int mode = (code >> 3) & 7;
    int reg = code & 7;

    switch (mode) {
    case 0:
        prreg (reg);
        break;
    case 1:
        append ("(");
        prreg (reg);
        append (")");
        break;
    case 2:
        if (reg == 7) {
            append ("$%#o", sign_extend (argcode));
        } else {
            append ("(");
            prreg (reg);
            append (")+");
        }
        break;
    case 3:
        if (reg == 7) {
            append ("*$%#o", argcode);
        } else {
            append ("*(");
            prreg (reg);
            append (")+");
        }
        break;
    case 4:
        append ("-(");
        prreg (reg);
        append (")");
        break;
    case 5:
        append ("*-(");
        prreg (reg);
        append (")");
        break;
    case 6:
    case 7:
        if (reg == 7) {
            unsigned int address = memaddr + sign_extend (argcode);
            if (mode == 7)
                append ("*");
            append ("$");
            praddr (address);
        } else {
            if (mode == 7)
                append ("*");
            append ("%#o", sign_extend (argcode));
            append ("(");
            prreg (reg);
            append (")");
        }
        break;
    }
}

static void prfoperand (unsigned memaddr, int code, unsigned argcode)
{
    if (((code >> 3) & 7) == 0)
        append ("fr%d", code & 7);
    else
        properand (memaddr, code, argcode);
}

char *disasm (unsigned memaddr, unsigned opcode, unsigned srccode, unsigned dstcode)
{
    int i, src, dst;

    bufptr = buf;

    src = (opcode >> 6) & 0x3f;
    dst = opcode & 0x3f;

    /* Print instruction code.
     * From one to three words are printed, depending on addressing mode. */
    append ("%06o", opcode);
    memaddr += 2;
    for (i=0; op[i].mask; i++)
        if ((opcode & op[i].mask) == op[i].opcode)
            break;
    switch (op[i].type) {
    case OPCODE_OP_OP:
        switch ((src >> 3) & 7) {
        case 2:
        case 3:
            if ((src & 7) != 7)
                break;
            /* fall through */
        case 6:
        case 7:
            /* Print srccode */
            memaddr += 2;
            append (" %06o", srccode);
            break;
        }
        /* fall through */
    case OPCODE_OP:
    case OPCODE_FOP:
    case OPCODE_REG_OP:
    case OPCODE_REG_OP_REV:
    case OPCODE_AC_FOP:
    case OPCODE_FOP_AC:
    case OPCODE_AC_OP:
    case OPCODE_OP_AC:
        switch ((dst >> 3) & 7) {
        case 2:
        case 3:
            if ((dst & 7) != 7)
                break;
            /* fall through */
        case 6:
        case 7:
            /* Print dstcode */
            memaddr += 2;
            append (" %06o", dstcode);
            break;
        }
        break;
    }

    /* Print operands. */
    append ("  ");
    for (i=0; op[i].mask; i++)
        if ((opcode & op[i].mask) == op[i].opcode)
            break;
    switch (op[i].type) {
    case OPCODE_NO_OPS:
        append ("%s", op[i].name);
        break;
    case OPCODE_REG:
        append ("%s" AFTER_INSTRUCTION, op[i].name);
        prreg (dst);
        break;
    case OPCODE_OP:
        append ("%s" AFTER_INSTRUCTION, op[i].name);
        if (strcmp (op[i].name, "jmp") == 0)
                dst |= JUMP;
        properand (memaddr, dst, dstcode);
        break;
    case OPCODE_FOP:
        append ("%s" AFTER_INSTRUCTION, op[i].name);
        if (strcmp (op[i].name, "jmp") == 0)
                dst |= JUMP;
        prfoperand (memaddr, dst, dstcode);
        break;
    case OPCODE_REG_OP:
        append ("%s" AFTER_INSTRUCTION, op[i].name);
        prreg (src);
        append ("%s", OPERAND_SEPARATOR);
        if (strcmp (op[i].name, "jsr") == 0)
                dst |= JUMP;
        properand (memaddr, dst, dstcode);
        break;
    case OPCODE_REG_OP_REV:
        append ("%s" AFTER_INSTRUCTION, op[i].name);
        properand (memaddr, dst, dstcode);
        append ("%s", OPERAND_SEPARATOR);
        prreg (src);
        break;
    case OPCODE_AC_FOP: {
        int ac = (opcode & 0xe0) >> 6;
        append ("%s" AFTER_INSTRUCTION, op[i].name);
        append ("fr%d", ac);
        append ("%s", OPERAND_SEPARATOR);
        prfoperand (memaddr, dst, dstcode);
        break;
    }
    case OPCODE_FOP_AC: {
        int ac = (opcode & 0xe0) >> 6;
        append ("%s" AFTER_INSTRUCTION, op[i].name);
        prfoperand (memaddr, dst, dstcode);
        append ("%s", OPERAND_SEPARATOR);
        append ("fr%d", ac);
        break;
    }
    case OPCODE_AC_OP: {
        int ac = (opcode & 0xe0) >> 6;
        append ("%s" AFTER_INSTRUCTION, op[i].name);
        append ("fr%d", ac);
        append ("%s", OPERAND_SEPARATOR);
        properand (memaddr, dst, dstcode);
        break;
    }
    case OPCODE_OP_AC: {
        int ac = (opcode & 0xe0) >> 6;
        append ("%s" AFTER_INSTRUCTION, op[i].name);
        properand (memaddr, dst, dstcode);
        append ("%s", OPERAND_SEPARATOR);
        append ("fr%d", ac);
        break;
    }
    case OPCODE_OP_OP:
        append ("%s" AFTER_INSTRUCTION, op[i].name);
        properand (memaddr - 2, src, srccode);
        append ("%s", OPERAND_SEPARATOR);
        properand (memaddr, dst, dstcode);
        break;
    case OPCODE_DISPL: {
        int displ = (opcode & 0xff) << 8;
        unsigned int address = memaddr + (sign_extend (displ) >> 7);
        append ("%s" AFTER_INSTRUCTION, op[i].name);
        praddr (address);
        break;
    }
    case OPCODE_REG_DISPL: {
        int displ = (opcode & 0x3f) << 10;
        unsigned int address = memaddr - (displ >> 9);
        append ("%s" AFTER_INSTRUCTION, op[i].name);
        prreg (src);
        append ("%s", OPERAND_SEPARATOR);
        praddr (address);
        break;
    }
    case OPCODE_IMM8: {
        int code = opcode & 0xff;
        append ("%s" AFTER_INSTRUCTION, op[i].name);
        append ("%#o", code);
        break;
    }
    case OPCODE_IMM6: {
        int code = opcode & 0x3f;
        append ("%s" AFTER_INSTRUCTION, op[i].name);
        append ("%#o", code);
        break;
    }
    case OPCODE_IMM3: {
        int code = opcode & 7;
        append ("%s" AFTER_INSTRUCTION, op[i].name);
        append ("%d", code);
        break;
    }
    case OPCODE_ILLEGAL:
        append (".word" AFTER_INSTRUCTION);
        append ("%#o", opcode);
        break;
    default:
        append ("???");
    }
    return buf;
}
