//
// Ignoring heap_end for now, the linker complains that some functions cannot be
// found. These functions are the system calls that must be implemented depending
// on the hardware. One example of a minimal implementation of these functions is here.
//
// The "hardware" where I will make the program run is the QEMU emulation of the
// Versatile Platform Baseboard Cortex-A8 platform. According to the RealView
// Platform Baseboard for Cortex-A8 User Guide, there is a memory-mapped UART
// serial port at address 0x10009000, and my intention is to write output and read
// input using this UART.
//
// The _sbrk function is used to increase the memory allocated by the program;
// modifying slightly the minimal implementation I use the memory space between the
// heap_low and the heap_top symbols that I defined inside the linker script. This
// is my implementation of syscalls.c:
//
#include <unistd.h>
#include <sys/stat.h>

enum {
    UART_FR_RXFE = 0x10,
    UART_FR_TXFF = 0x20,
    UART0_ADDR = 0x10009000,
};

#define UART_DR(baseaddr) (*(unsigned int *)(baseaddr))
#define UART_FR(baseaddr) (*(((unsigned int *)(baseaddr))+6))

void _exit(int status)
{
    for (;;)
        continue;
}

int _close(int file)
{
    return -1;
}

int _fstat(int file, struct stat* st)
{
    st->st_mode = S_IFCHR;
    return 0;
}

int _isatty(int file)
{
    return 1;
}

int _lseek(int file, int ptr, int dir)
{
    return 0;
}

int _open(const char* name, int flags, int mode)
{
    return -1;
}

int _read(int file, char* ptr, int len)
{
    int todo;

    if (len == 0) {
        return 0;
    }

    while (UART_FR(UART0_ADDR) & UART_FR_RXFE);

    *ptr++ = UART_DR(UART0_ADDR);

    for (todo = 1; todo < len; todo++) {
        if (UART_FR(UART0_ADDR) & UART_FR_RXFE) {
            break;
        }

        *ptr++ = UART_DR(UART0_ADDR);
    }

    return todo;
}

//char* heap_end = 0;

caddr_t _sbrk(int incr)
{
#if 1
    return 0;
#else
    extern char heap_low; /* Defined by the linker */
    extern char heap_top; /* Defined by the linker */
    char* prev_heap_end;

    if (heap_end == 0) {
        heap_end = &heap_low;
    }

    prev_heap_end = heap_end;

    if (heap_end + incr > &heap_top) {
        /* Heap and stack collision */
        return (caddr_t)0;
    }

    heap_end += incr;
    return (caddr_t) prev_heap_end;
#endif
}

int _write(int file, char* ptr, int len)
{
    int todo;

    for (todo = 0; todo < len; todo++) {
        UART_DR(UART0_ADDR) = *ptr++;
    }

    return len;
}
