#include <stdio.h>
#include "riscvreg.h"
#include "encoding.h"

void print_reg(unsigned csr, unsigned value, const char *name)
{
    printf("%-10s = %08x\n", name, value);
}

void print_all_regs()
{
    print_reg(CSR_MVENDORID,  csr_read(mvendorid),  "mvendorid");
    print_reg(CSR_MARCHID,    csr_read(marchid),    "marchid");
    print_reg(CSR_MIMPID,     csr_read(mimpid),     "mimpid");
    print_reg(CSR_MHARTID,    csr_read(mhartid),    "mhartid");
    print_reg(CSR_MSTATUS,    csr_read(mstatus),    "mstatus");
    print_reg(CSR_MISA,       csr_read(misa),       "misa");
    print_reg(CSR_MTVEC,      csr_read(mtvec),      "mtvec");
    print_reg(CSR_MSCRATCH,   csr_read(mscratch),   "mscratch");
    print_reg(CSR_MEPC,       csr_read(mepc),       "mepc");
    print_reg(CSR_MCAUSE,     csr_read(mcause),     "mcause");
    print_reg(CSR_MTVAL,      csr_read(mtval),      "mtval");
    print_reg(CSR_PMPCFG0,    csr_read(pmpcfg0),    "pmpcfg0");
    print_reg(CSR_PMPCFG1,    csr_read(pmpcfg1),    "pmpcfg1");
    print_reg(CSR_PMPCFG2,    csr_read(pmpcfg2),    "pmpcfg2");
    print_reg(CSR_PMPCFG3,    csr_read(pmpcfg3),    "pmpcfg3");
    print_reg(CSR_PMPADDR0,   csr_read(pmpaddr0),   "pmpaddr0");
    print_reg(CSR_PMPADDR1,   csr_read(pmpaddr1),   "pmpaddr1");
    print_reg(CSR_PMPADDR2,   csr_read(pmpaddr2),   "pmpaddr2");
    print_reg(CSR_PMPADDR3,   csr_read(pmpaddr3),   "pmpaddr3");
    print_reg(CSR_PMPADDR4,   csr_read(pmpaddr4),   "pmpaddr4");
    print_reg(CSR_PMPADDR5,   csr_read(pmpaddr5),   "pmpaddr5");
    print_reg(CSR_PMPADDR6,   csr_read(pmpaddr6),   "pmpaddr6");
    print_reg(CSR_PMPADDR7,   csr_read(pmpaddr7),   "pmpaddr7");
    print_reg(CSR_PMPADDR8,   csr_read(pmpaddr8),   "pmpaddr8");
    print_reg(CSR_PMPADDR9,   csr_read(pmpaddr9),   "pmpaddr9");
    print_reg(CSR_PMPADDR10,  csr_read(pmpaddr10),  "pmpaddr10");
    print_reg(CSR_PMPADDR11,  csr_read(pmpaddr11),  "pmpaddr11");
    print_reg(CSR_PMPADDR12,  csr_read(pmpaddr12),  "pmpaddr12");
    print_reg(CSR_PMPADDR13,  csr_read(pmpaddr13),  "pmpaddr13");
    print_reg(CSR_PMPADDR14,  csr_read(pmpaddr14),  "pmpaddr14");
    print_reg(CSR_PMPADDR15,  csr_read(pmpaddr15),  "pmpaddr15");
}
