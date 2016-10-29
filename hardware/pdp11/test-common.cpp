#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <verilated.h>		// Defines common routines
#include "opcode.h"		// From Verilating "alu.v"

unsigned main_time = 0;         // Current simulation time

double sc_time_stamp ()
{                               // Called by $time in Verilog
    return main_time;
}

const char *opname (unsigned op)
{
    switch (op & ~077) {
    case MOV:  return "MOV";
    case MOVB: return "MOVB";
    case CMP:  return "CMP";
    case CMPB: return "CMPB";
    case ADD:  return "ADD";
    case SUB:  return "SUB";
    case BIT:  return "BIT";
    case BITB: return "BITB";
    case BIC:  return "BIC";
    case BICB: return "BICB";
    case BIS:  return "BIS";
    case BISB: return "BISB";
    }
    switch (op & ~07) {
    case ASH:  return "ASH";
    case ASHC: return "ASHC";
    case MUL:  return "MUL";
    case DIV:  return "DIV";
    case XOR:  return "XOR";
    }
    switch (op) {
    case CLR:  return "CLR";
    case CLRB: return "CLRB";
    case COM:  return "COM";
    case COMB: return "COMB";
    case INC:  return "INC";
    case INCB: return "INCB";
    case DEC:  return "DEC";
    case DECB: return "DECB";
    case NEG:  return "NEG";
    case NEGB: return "NEGB";
    case TST:  return "TST";
    case TSTB: return "TSTB";
    case ASR:  return "ASR";
    case ASRB: return "ASRB";
    case ASL:  return "ASL";
    case ASLB: return "ASLB";
    case ROR:  return "ROR";
    case RORB: return "RORB";
    case ROL:  return "ROL";
    case ROLB: return "ROLB";
    case SWAB: return "SWAB";
    case ADC:  return "ADC";
    case ADCB: return "ADCB";
    case SBC:  return "SBC";
    case SBCB: return "SBCB";
    case SXT:  return "SXT";
    case MFPS: return "MFPS";
    case MTPS: return "MTPS";
    case INC2: return "INC2";
    case DEC2: return "DEC2";
    }
    return "?OP?";
}

void load_file (const char *name, unsigned addr, unsigned short memory[])
{
    int fd = open (name, 0);
    if (fd < 0) {
        perror (name);
        exit (-1);
    }

    unsigned char buf [16];
    int n = read (fd, buf, 16);
    if (n != 16) {
rerr:   fprintf (stderr, "%s: read error\n", name);
        close (fd);
        exit (-1);
    }

    unsigned magic = buf[0] | buf[1] << 8;
    if (magic != 0407) {
        fprintf (stderr, "%s: invalid magic %04o\n", name, magic);
        close (fd);
        exit (-1);
    }

    unsigned tsize = buf[2] | buf[3] << 8;
    while (tsize > 0) {
        n = read (fd, buf, 2);
        if (n != 2)
            goto rerr;

        unsigned val = buf[0] | buf[1] << 8;
//printf ("%06o := %06o\n", addr, val);
        memory [addr>>1] = val;
        addr += 2;
        tsize -= 2;
    }
    close (fd);
}
