#include <stdio.h>
#include "riscvreg.h"
#include "encoding.h"

#define CSR_TCONTROL    0x7a5   // Global Trigger Control
#define CSR_DSCRATCH1   0x7b3   // Debug Scratch Register 1
#define CSR_MPCER       0x7e0   // Machine Performance Counter Event
#define CSR_MPCMR       0x7e1   // Machine Performance Counter Mode
#define CSR_MPCCR       0x7e2   // Machine Performance Counter Count

#define print_reg_ro(index, asm_id, name) { \
        unsigned _a = csr_read(asm_id); \
        printf("%-10s [%03x] = %08x read only\n", name, index, _a); \
    }

#define print_reg_rw(index, asm_id, name) { \
        unsigned _a = csr_read(asm_id); \
        csr_write(asm_id, 0xffffffff); \
        unsigned _b = csr_read(asm_id); \
        csr_write(asm_id, 0); \
        unsigned _c = csr_read(asm_id); \
        csr_write(asm_id, _a); \
        printf("%-10s [%03x] = %08x mask %08x (max %08x min %08x)\n", name, index, _a, _b^_c, _b, _c); \
    }

void print_all_regs()
{
    // Machine Information CSRs
    print_reg_ro(CSR_MVENDORID,  mvendorid,   "mvendorid");
    print_reg_ro(CSR_MARCHID,    marchid,     "marchid");
    print_reg_ro(CSR_MIMPID,     mimpid,      "mimpid");
    print_reg_ro(CSR_MHARTID,    mhartid,     "mhartid");

    // Machine Trap Setup CSRs
    print_reg_rw(CSR_MSTATUS,    mstatus,     "mstatus");
    print_reg_rw(CSR_MISA,       misa,        "misa");
    print_reg_rw(CSR_MTVEC,      mtvec,       "mtvec");

    // Machine Trap Handling CSRs
    print_reg_rw(CSR_MSCRATCH,   mscratch,    "mscratch");
    print_reg_rw(CSR_MEPC,       mepc,        "mepc");
    print_reg_rw(CSR_MCAUSE,     mcause,      "mcause");
    print_reg_rw(CSR_MTVAL,      mtval,       "mtval");

    // Physical Memory Protection (PMP) CSRs
    print_reg_rw(CSR_PMPCFG0,    pmpcfg0,     "pmpcfg0");
    print_reg_rw(CSR_PMPCFG1,    pmpcfg1,     "pmpcfg1");
    print_reg_rw(CSR_PMPCFG2,    pmpcfg2,     "pmpcfg2");
    print_reg_rw(CSR_PMPCFG3,    pmpcfg3,     "pmpcfg3");
    print_reg_rw(CSR_PMPADDR0,   pmpaddr0,    "pmpaddr0");
    print_reg_rw(CSR_PMPADDR1,   pmpaddr1,    "pmpaddr1");
    print_reg_rw(CSR_PMPADDR2,   pmpaddr2,    "pmpaddr2");
    print_reg_rw(CSR_PMPADDR3,   pmpaddr3,    "pmpaddr3");
    print_reg_rw(CSR_PMPADDR4,   pmpaddr4,    "pmpaddr4");
    print_reg_rw(CSR_PMPADDR5,   pmpaddr5,    "pmpaddr5");
    print_reg_rw(CSR_PMPADDR6,   pmpaddr6,    "pmpaddr6");
    print_reg_rw(CSR_PMPADDR7,   pmpaddr7,    "pmpaddr7");
    print_reg_rw(CSR_PMPADDR8,   pmpaddr8,    "pmpaddr8");
    print_reg_rw(CSR_PMPADDR9,   pmpaddr9,    "pmpaddr9");
    print_reg_rw(CSR_PMPADDR10,  pmpaddr10,   "pmpaddr10");
    print_reg_rw(CSR_PMPADDR11,  pmpaddr11,   "pmpaddr11");
    print_reg_rw(CSR_PMPADDR12,  pmpaddr12,   "pmpaddr12");
    print_reg_rw(CSR_PMPADDR13,  pmpaddr13,   "pmpaddr13");
    print_reg_rw(CSR_PMPADDR14,  pmpaddr14,   "pmpaddr14");
    print_reg_rw(CSR_PMPADDR15,  pmpaddr15,   "pmpaddr15");

    // Trigger Module CSRs (shared with Debug Mode)
    print_reg_rw(CSR_TSELECT,    tselect,     "tselect");     // Trigger Select Register
    print_reg_rw(CSR_TDATA1,     tdata1,      "tdata1");      // Trigger Abstract Data 1
    print_reg_rw(CSR_TDATA2,     tdata2,      "tdata2");      // Trigger Abstract Data 2
    print_reg_rw(CSR_TCONTROL,   0x7a5,       "tcontrol");    // Global Trigger Control

    // Debug Mode CSRs
#if 0
    // These cause Illegal Instruction exception.
    print_reg_rw(CSR_DCSR,       dcsr,        "dcsr");        // Debug Control and Status
    print_reg_rw(CSR_DPC,        dpc,         "dpc");         // Debug PC
    print_reg_rw(CSR_DSCRATCH,   dscratch,    "dscratch0");   // Debug Scratch Register 0
    print_reg_rw(CSR_DSCRATCH1,  0x7b3,       "dscratch1");   // Debug Scratch Register 1
#endif

    // Performance Counter CSRs (Custom) 4
    print_reg_rw(CSR_MPCER,      0x7e0,       "mpcer");       // Machine Performance Counter Event
    print_reg_rw(CSR_MPCMR,      0x7e1,       "mpcmr");       // Machine Performance Counter Mode
    print_reg_rw(CSR_MPCCR,      0x7e2,       "mpccr");       // Machine Performance Counter Count

    // User Custom CSRs
    print_reg_rw(0x800,          0x800,       "0x800");
    print_reg_rw(0x801,          0x801,       "0x801");
    print_reg_rw(0x802,          0x802,       "0x802");
    print_reg_rw(0x803,          0x803,       "0x803");
    print_reg_rw(0x804,          0x804,       "0x804");
    print_reg_rw(0x805,          0x805,       "0x805");
}
