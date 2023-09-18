//
// Figure out write masks of system registers:
//
// f3ef 8000 mrs   r0, APSR
// f3ef 8001 mrs   r0, IAPSR
// f3ef 8002 mrs   r0, EAPSR
// f3ef 8003 mrs   r0, XPSR
// f3ef 8004 mrs   r0, <unknown>
// f3ef 8005 mrs   r0, IPSR
// f3ef 8006 mrs   r0, EPSR
// f3ef 8007 mrs   r0, IEPSR
//
#include <stdio.h>
#include <pico/stdlib.h>

//
// Read system registers.
//
static unsigned inline __attribute__ ((always_inline))
get_apsr(void)
{
    unsigned v;
    asm volatile ("mrs %0, APSR" : "=r" (v));
    return v;
}

static unsigned inline __attribute__ ((always_inline))
get_iapsr(void)
{
    unsigned v;
    asm volatile ("mrs %0, IAPSR" : "=r" (v));
    return v;
}

static unsigned inline __attribute__ ((always_inline))
get_eapsr(void)
{
    unsigned v;
    asm volatile ("mrs %0, EAPSR" : "=r" (v));
    return v;
}

static unsigned inline __attribute__ ((always_inline))
get_xpsr(void)
{
    unsigned v;
    asm volatile ("mrs %0, XPSR" : "=r" (v));
    return v;
}

static unsigned inline __attribute__ ((always_inline))
get_ipsr(void)
{
    unsigned v;
    asm volatile ("mrs %0, IPSR" : "=r" (v));
    return v;
}

static unsigned inline __attribute__ ((always_inline))
get_epsr(void)
{
    unsigned v;
    asm volatile ("mrs %0, EPSR" : "=r" (v));
    return v;
}

static unsigned inline __attribute__ ((always_inline))
get_iepsr(void)
{
    unsigned v;
    asm volatile ("mrs %0, IEPSR" : "=r" (v));
    return v;
}

static unsigned inline __attribute__ ((always_inline))
get_msp(void)
{
    unsigned v;
    asm volatile ("mrs %0, MSP" : "=r" (v));
    return v;
}

static unsigned inline __attribute__ ((always_inline))
get_psp(void)
{
    unsigned v;
    asm volatile ("mrs %0, PSP" : "=r" (v));
    return v;
}

static unsigned inline __attribute__ ((always_inline))
get_primask(void)
{
    unsigned v;
    asm volatile ("mrs %0, PRIMASK" : "=r" (v));
    return v;
}

static unsigned inline __attribute__ ((always_inline))
get_control(void)
{
    unsigned v;
    asm volatile ("mrs %0, CONTROL" : "=r" (v));
    return v;
}

//
// Write system registers.
//
static void inline __attribute__ ((always_inline))
set_apsr(unsigned v)
{
    asm volatile ("msr APSR_nzcvq, %0" : : "r" (v));
}

static void inline __attribute__ ((always_inline))
set_iapsr(unsigned v)
{
    asm volatile ("msr IAPSR_nzcvq, %0" : : "r" (v));
}

static void inline __attribute__ ((always_inline))
set_eapsr(unsigned v)
{
    asm volatile ("msr EAPSR_nzcvq, %0" : : "r" (v));
}

static void inline __attribute__ ((always_inline))
set_xpsr(unsigned v)
{
    asm volatile ("msr XPSR_nzcvq, %0" : : "r" (v));
}

static void inline __attribute__ ((always_inline))
set_ipsr(unsigned v)
{
    asm volatile ("msr IPSR, %0" : : "r" (v));
}

static void inline __attribute__ ((always_inline))
set_epsr(unsigned v)
{
    asm volatile ("msr EPSR, %0" : : "r" (v));
}

static void inline __attribute__ ((always_inline))
set_iepsr(unsigned v)
{
    asm volatile ("msr IEPSR, %0" : : "r" (v));
}

static void inline __attribute__ ((always_inline))
set_msp(unsigned v)
{
    asm volatile ("msr MSP, %0" : : "r" (v));
}

static void inline __attribute__ ((always_inline))
set_psp(unsigned v)
{
    asm volatile ("msr PSP, %0" : : "r" (v));
}

static void inline __attribute__ ((always_inline))
set_primask(unsigned v)
{
    asm volatile ("msr PRIMASK, %0" : : "r" (v));
}

static void inline __attribute__ ((always_inline))
set_control(unsigned v)
{
    asm volatile ("msr CONTROL, %0" : : "r" (v));
}

void show_sysreg()
{
    unsigned apsr = get_apsr();
    unsigned iapsr = get_iapsr();
    unsigned eapsr = get_eapsr();
    unsigned xpsr = get_xpsr();
    unsigned ipsr = get_ipsr();
    unsigned epsr = get_epsr();
    unsigned iepsr = get_iepsr();
    unsigned msp = get_msp();
    unsigned psp = get_psp();
    unsigned primask = get_primask();
    unsigned control = get_control();
    printf("apsr    = %08x\n", apsr);
    printf("iapsr   = %08x\n", iapsr);
    printf("eapsr   = %08x\n", eapsr);
    printf("xpsr    = %08x\n", xpsr);
    printf("ipsr    = %08x\n", ipsr);
    printf("epsr    = %08x\n", epsr);
    printf("iepsr   = %08x\n", iepsr);
    printf("msp     = %08x\n", msp);
    printf("psp     = %08x\n", psp);
    printf("primask = %08x\n", primask);
    printf("control = %08x\n", control);
}

void probe_apsr()
{
    unsigned initial = get_apsr();
    set_apsr(0);
    unsigned w0 = get_apsr();
    set_apsr(~0);
    unsigned w1 = get_apsr();
    set_apsr(initial);
    printf("apsr initial = %08x\n", initial);
    printf("write zeroes = %08x\n", w0);
    printf("write ones   = %08x\n", w1);
}

void probe_iapsr()
{
    unsigned initial = get_iapsr();
    set_iapsr(0);
    unsigned w0 = get_iapsr();
    set_iapsr(~0);
    unsigned w1 = get_iapsr();
    set_iapsr(initial);
    printf("iapsr initial = %08x\n", initial);
    printf("write zeroes  = %08x\n", w0);
    printf("write ones    = %08x\n", w1);
}

void probe_eapsr()
{
    unsigned initial = get_eapsr();
    set_eapsr(0);
    unsigned w0 = get_eapsr();
    set_eapsr(~0);
    unsigned w1 = get_eapsr();
    set_eapsr(initial);
    printf("eapsr initial = %08x\n", initial);
    printf("write zeroes  = %08x\n", w0);
    printf("write ones    = %08x\n", w1);
}

void probe_xpsr()
{
    unsigned initial = get_xpsr();
    set_xpsr(0);
    unsigned w0 = get_xpsr();
    set_xpsr(~0);
    unsigned w1 = get_xpsr();
    set_xpsr(initial);
    printf("xpsr initial = %08x\n", initial);
    printf("write zeroes = %08x\n", w0);
    printf("write ones   = %08x\n", w1);
}

void probe_ipsr()
{
    unsigned initial = get_ipsr();
    set_ipsr(0);
    unsigned w0 = get_ipsr();
    set_ipsr(~0);
    unsigned w1 = get_ipsr();
    set_ipsr(initial);
    printf("ipsr initial = %08x\n", initial);
    printf("write zeroes = %08x\n", w0);
    printf("write ones   = %08x\n", w1);
}

void probe_epsr()
{
    unsigned initial = get_epsr();
    set_epsr(0);
    unsigned w0 = get_epsr();
    set_epsr(~0);
    unsigned w1 = get_epsr();
    set_epsr(initial);
    printf("epsr initial = %08x\n", initial);
    printf("write zeroes = %08x\n", w0);
    printf("write ones   = %08x\n", w1);
}

void probe_iepsr()
{
    unsigned initial = get_iepsr();
    set_iepsr(0);
    unsigned w0 = get_iepsr();
    set_iepsr(~0);
    unsigned w1 = get_iepsr();
    set_iepsr(initial);
    printf("iepsr initial = %08x\n", initial);
    printf("write zeroes  = %08x\n", w0);
    printf("write ones    = %08x\n", w1);
}

void probe_msp()
{
    unsigned initial = get_msp();
    set_msp(0);
    unsigned w0 = get_msp();
    set_msp(~0);
    unsigned w1 = get_msp();
    set_msp(initial);
    printf("msp initial  = %08x\n", initial);
    printf("write zeroes = %08x\n", w0);
    printf("write ones   = %08x\n", w1);
}

void probe_psp()
{
    unsigned initial = get_psp();
    set_psp(0);
    unsigned w0 = get_psp();
    set_psp(~0);
    unsigned w1 = get_psp();
    set_psp(initial);
    printf("psp initial  = %08x\n", initial);
    printf("write zeroes = %08x\n", w0);
    printf("write ones   = %08x\n", w1);
}

void probe_primask()
{
    unsigned initial = get_primask();
    set_primask(0);
    unsigned w0 = get_primask();
    set_primask(~0);
    unsigned w1 = get_primask();
    set_primask(initial);
    printf("primask initial = %08x\n", initial);
    printf("write zeroes    = %08x\n", w0);
    printf("write ones      = %08x\n", w1);
}

void probe_control()
{
    unsigned initial = get_control();
    set_control(0);
    unsigned w0 = get_control();
    set_control(~1);            // avoid bit nPRIV - it blocks the program
    unsigned w1 = get_control();
    set_control(initial);
    printf("control initial = %08x\n", initial);
    printf("write zeroes    = %08x\n", w0);
    printf("write ones      = %08x\n", w1);
}

int main()
{
    unsigned count = 0;

    stdio_init_all();
    for (;;) {
        printf("------------ %u\n", ++count);

        show_sysreg();
        probe_apsr();       // write mask f000_0000
        probe_iapsr();      // write mask f000_0000
        probe_eapsr();      // write mask f000_0000
        probe_xpsr();       // write mask f000_0000
        probe_ipsr();       // write mask 0000_0000
        probe_epsr();       // write mask 0000_0000
        probe_iepsr();      // write mask 0000_0000

        probe_msp();        // write mask ffff_fffc
        probe_psp();        // write mask ffff_fffc, initial ffff_fffc
        probe_primask();    // write mask 0000_0001
        probe_control();    // write mask 0000_0003

        sleep_ms(1000);
    }
}
