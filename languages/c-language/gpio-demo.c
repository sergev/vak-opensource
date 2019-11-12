#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#define GPIO_ADDR 0x1f860000

struct gpioreg {
    volatile unsigned ansel;            /* Analog select */
    volatile unsigned anselclr;
    volatile unsigned anselset;
    volatile unsigned anselinv;
    volatile unsigned tris;             /* Mask of inputs */
    volatile unsigned trisclr;
    volatile unsigned trisset;
    volatile unsigned trisinv;
    volatile unsigned port;             /* Read inputs, write outputs */
    volatile unsigned portclr;
    volatile unsigned portset;
    volatile unsigned portinv;
    volatile unsigned lat;              /* Read/write outputs */
    volatile unsigned latclr;
    volatile unsigned latset;
    volatile unsigned latinv;
    volatile unsigned odc;              /* Open drain configuration */
    volatile unsigned odcclr;
    volatile unsigned odcset;
    volatile unsigned odcinv;
    volatile unsigned cnpu;             /* Input pin pull-up enable */
    volatile unsigned cnpuclr;
    volatile unsigned cnpuset;
    volatile unsigned cnpuinv;
    volatile unsigned cnpd;             /* Input pin pull-down enable */
    volatile unsigned cnpdclr;
    volatile unsigned cnpdset;
    volatile unsigned cnpdinv;
    volatile unsigned cncon;            /* Interrupt-on-change control */
    volatile unsigned cnconclr;
    volatile unsigned cnconset;
    volatile unsigned cnconinv;
    volatile unsigned cnen;             /* Input change interrupt enable */
    volatile unsigned cnenclr;
    volatile unsigned cnenset;
    volatile unsigned cneninv;
    volatile unsigned cnstat;           /* Change notification status */
    volatile unsigned cnstatclr;
    volatile unsigned cnstatset;
    volatile unsigned cnstatinv;
    volatile unsigned unused[6*4];
};

struct gpioreg *gpio_A, *gpio_B, *gpio_C, *gpio_D, *gpio_E, *gpio_F, *gpio_G, *gpio_H, *gpio_J, *gpio_K;

void print_gpio(struct gpioreg *gpio, const char *label)
{
    printf("Port %s:\n", label);
    printf("    ansel  = %04x\n", gpio->ansel);     /* Analog select */
    printf("    tris   = %04x\n", gpio->tris);      /* Mask of inputs */
    printf("    port   = %04x\n", gpio->port);      /* Read inputs, write outputs */
    printf("    lat    = %04x\n", gpio->lat);       /* Read/write outputs */
//  printf("    odc    = %04x\n", gpio->odc);       /* Open drain configuration */
    printf("    cnpu   = %04x\n", gpio->cnpu);      /* Input pin pull-up enable */
    printf("    cnpd   = %04x\n", gpio->cnpd);      /* Input pin pull-down enable */
//  printf("    cncon  = %04x\n", gpio->cncon);     /* Interrupt-on-change control */
//  printf("    cnen   = %04x\n", gpio->cnen);      /* Input change interrupt enable */
//  printf("    cnstat = %04x\n", gpio->cnstat);    /* Change notification status */
}

int main()
{
    int fd;

    // Obtain handle to physical memory
    fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd < 0) {
        printf("Unable to open /dev/mem: %s\n", strerror(errno));
        return -1;
    }

    // Map a page of memory to gpio address
    int32_t base = (int32_t) mmap(0, 4096, PROT_READ|PROT_WRITE, MAP_SHARED, fd, GPIO_ADDR);
    if (base < 0) {
        printf("Mmap failed: %s\n", strerror(errno));
        return -1;
    }
    gpio_A = (struct gpioreg*) (base + 0x000);  /* Port A */
    gpio_B = (struct gpioreg*) (base + 0x100);  /* Port B */
    gpio_C = (struct gpioreg*) (base + 0x200);  /* Port C */
    gpio_D = (struct gpioreg*) (base + 0x300);  /* Port D */
    gpio_E = (struct gpioreg*) (base + 0x400);  /* Port E */
    gpio_F = (struct gpioreg*) (base + 0x500);  /* Port F */
    gpio_G = (struct gpioreg*) (base + 0x600);  /* Port G */
    gpio_H = (struct gpioreg*) (base + 0x700);  /* Port H */
    gpio_J = (struct gpioreg*) (base + 0x800);  /* Port J */
    gpio_K = (struct gpioreg*) (base + 0x900);  /* Port K */

    print_gpio(gpio_A, "A");
    print_gpio(gpio_B, "B");
    print_gpio(gpio_C, "C");
    print_gpio(gpio_D, "D");
    print_gpio(gpio_E, "E");
    print_gpio(gpio_F, "F");
    print_gpio(gpio_G, "G");
    print_gpio(gpio_H, "H");
    print_gpio(gpio_J, "J");
    print_gpio(gpio_K, "K");
#if 0
    // set RH4 as output
    gpio_H->anselclr = 1 << 4;
    gpio_H->trisclr = 1 << 4;

    // toggle RH4 every second
    while (1) {
        // set the pin high
        gpio_H->latset = 1 << 4;
        sleep(1);

        // set the pin to low
        gpio_H->latclr = 1 << 4;
        sleep(1);
    }
#endif
#if 1
    // set RB15, RH12 and RD15 as input
    gpio_B->anselclr = 1 << 15;
    gpio_B->trisset = 1 << 15;
    gpio_B->cnpuset = 1 << 15;

    gpio_H->anselclr = 1 << 12;
    gpio_H->trisset = 1 << 12;
    gpio_H->cnpuset = 1 << 12;

    gpio_D->anselclr = 1 << 15;
    gpio_D->trisset = 1 << 15;
    gpio_D->cnpuset = 1 << 15;

    // print any pin change
    int last_b15 = (gpio_B->port >> 15) & 1;
    int last_h12 = (gpio_H->port >> 12) & 1;
    int last_d15 = (gpio_D->port >> 15) & 1;
    while (1) {
        usleep(10000);
        int b15 = (gpio_B->port >> 15) & 1;
        int h12 = (gpio_H->port >> 12) & 1;
        int d15 = (gpio_D->port >> 15) & 1;
        if (b15 != last_b15) {
            printf("B15 %s\n", !b15 ? "pressed" : "released");
            last_b15 = b15;
        }
        if (h12 != last_h12) {
            printf("H12 %s\n", !h12 ? "pressed" : "released");
            last_h12 = h12;
        }
        if (d15 != last_d15) {
            printf("D15 %s\n", !d15 ? "pressed" : "released");
            last_d15 = d15;
        }
    }
#endif
    return 0;
}
