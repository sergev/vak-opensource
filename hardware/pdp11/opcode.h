//
// Single-operand instructions.
//
#define	CLR		00050	// d = 0
#define	CLRB		01050
#define	COM		00051	// d = ~b
#define	COMB		01051
#define	INC		00052	// d = b + 1
#define	INCB		01052
#define	DEC		00053	// d = b - 1
#define	DECB		01053
#define	NEG		00054	// d = 0 - b
#define	NEGB		01054
#define	TST		00057	// d = b
#define	TSTB		01057
#define	ASR		00062	// d|c = b >> 1
#define	ASRB		01062
#define	ASL		00063	// c|d = b << 1
#define	ASLB		01063
#define	ROR		00060	// d|c = c|b
#define	RORB		01060
#define	ROL		00061	// c|d = b|c
#define	ROLB		01061
#define	SWAB		00003	// d = swab (b)
#define	ADC		00055	// d = b + c
#define	ADCB		01055
#define	SBC		00056	// d = b - c
#define	SBCB		01056
#define	SXT		00067	// d = n ? -1 : 0
#define	MFPS		01067	// d = ps
#define	MTPS		01064	// ps = b

// Nonstandard commands.
#define	INC2		00072	// d = b + 2
#define	DEC2		00073	// d = b - 2

//
// Double-operand instructions.
//
#define	MOV		00100	// d = a
#define	MOVB		01100
#define	CMP		00200	// d = a - b (no register store)
#define	CMPB		01200
#define	ADD		00600	// d = a + b
#define	SUB		01600	// d = b - a
#define	ASH		00720	// d = a>0 ? b<<a | b>>(-a)
#define	ASHC		00730	// TODO
#define	MUL		00700	// TODO
#define	DIV		00710	// TODO
#define	BIT		00300	// d = a & b (no register store)
#define	BITB		01300
#define	BIC		00400	// d = ~a & b
#define	BICB		01400
#define	BIS		00500	// d = a | b
#define	BISB		01500
#define	XOR		00740	// d = a ^ b

#define N (1 << 3)
#define Z (1 << 2)
#define V (1 << 1)
#define C (1 << 0)

extern unsigned main_time;      // Current simulation time

const char *opname (unsigned op);

void load_file (const char *name, unsigned addr, unsigned short memory[]);

extern "C" {
    char *disasm (unsigned memaddr, unsigned opcode, unsigned srccode, unsigned dstcode);
};
