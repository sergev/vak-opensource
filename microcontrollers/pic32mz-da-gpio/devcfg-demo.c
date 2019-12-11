#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#define DEVCFG_ADDR 0x1fc0f000
#define DEVSN_ADDR  0x1fc54000

int main()
{
    // Obtain handle to physical memory
    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd < 0) {
        printf("Unable to open /dev/mem: %s\n", strerror(errno));
        return -1;
    }

    // Map a page of memory to the physical address
    int32_t devcfg_base = (int32_t) mmap(0, 4096, PROT_READ|PROT_WRITE, MAP_SHARED, fd, DEVCFG_ADDR);
    int32_t devsn_base = (int32_t) mmap(0, 4096, PROT_READ|PROT_WRITE, MAP_SHARED, fd, DEVSN_ADDR);
    if (devcfg_base < 0 || devsn_base < 0) {
        printf("Mmap failed: %s\n", strerror(errno));
        return -1;
    }

    uint32_t devcfg4 = *(uint32_t*) (devcfg_base + 0xfbc);
    uint32_t devcfg3 = *(uint32_t*) (devcfg_base + 0xfc0);
    uint32_t devcfg2 = *(uint32_t*) (devcfg_base + 0xfc4);
    uint32_t devcfg1 = *(uint32_t*) (devcfg_base + 0xfc8);
    uint32_t devcfg0 = *(uint32_t*) (devcfg_base + 0xfcc);

    uint32_t devsn0 = *(uint32_t*) (devsn_base + 0x020);
    uint32_t devsn1 = *(uint32_t*) (devsn_base + 0x024);
    uint32_t devsn2 = *(uint32_t*) (devsn_base + 0x028);
    uint32_t devsn3 = *(uint32_t*) (devsn_base + 0x02c);

    printf("    devcfg0 = %04x\n", devcfg0);
    printf("    devcfg1 = %04x\n", devcfg1);
    printf("    devcfg2 = %04x\n", devcfg2);
    printf("    devcfg3 = %04x\n", devcfg3);
    printf("    devcfg4 = %04x\n", devcfg4);

    printf("    devsn0  = %04x\n", devsn0);
    printf("    devsn1  = %04x\n", devsn1);
    printf("    devsn2  = %04x\n", devsn2);
    printf("    devsn3  = %04x\n", devsn3);

    return 0;
}
