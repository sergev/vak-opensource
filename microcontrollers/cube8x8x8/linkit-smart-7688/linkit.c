/*
 * Interface to LED cube 8x8x8.
 * The cube is connected to the Linkit Smart 7688 board,
 * pins P10-P13, P26-P31.
 *
 * Cube  Function Linkit MRAA   Function
 * -------------------------------------------
 * P1/1   +5V     5V     ---
 * P1/2   Y0      P10    gpio2  \
 * P1/3   Y1      P11    gpio3  | Layer select
 * P1/4   Y2      P13    gpio1  /
 * P1/5   Y3      P25    gpio6  - Upper backlight
 * P1/6   Y4      P26    gpio18 - Lower backlight
 * P1/7   SDI     P27    gpio19 - Serial data   \
 * P1/8   CLK     P28    gpio17 - Clock         | to shift registers
 * P1/9   /LE     P29    gpio16 - Latch enable  |
 * P1/10  /OE     P30    gpio15 - Output enable /
 * P1/11  EXT     P31    gpio14 - Unknown
 * P1/12  GND     GND    ---
 * ---    Sensor  P21    gpio4    - Sensor input
 */
#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <mraa.h>
#include "cube.h"

static mraa_gpio_context y0, y1, y2;    /* Layer select */
static mraa_gpio_context y3;            /* Upper backlight */
static mraa_gpio_context y4;            /* Lower backlight */
static mraa_gpio_context sdi;           /* Serial data */
static mraa_gpio_context clk;           /* Clock */
static mraa_gpio_context le;            /* Latch enable */
static mraa_gpio_context oe;            /* Output enable */
static mraa_gpio_context ext;           /* Unknown */
static mraa_gpio_context sensor;        /* Input sensor */

/*
 * Configure GPIO pin as output.
 * Print warning in case of error.
 */
static void set_output(mraa_gpio_context pin)
{
    mraa_result_t r;

    r = mraa_gpio_dir(pin, MRAA_GPIO_OUT);
    if (r != MRAA_SUCCESS) {
        mraa_result_print(r);
    }
    r = mraa_gpio_write(pin, 0);
    if (r != MRAA_SUCCESS) {
        mraa_result_print(r);
    }
}

/*
 * Configure GPIO pin as input.
 * Print warning in case of error.
 */
static void set_input(mraa_gpio_context pin)
{
    mraa_result_t r;

    r = mraa_gpio_dir(pin, MRAA_GPIO_IN);
    if (r != MRAA_SUCCESS) {
        mraa_result_print(r);
    }
}

/*
 * Initialize MRAA library.
 * Configure all pins as appropriate.
 */
void gpio_init()
{
    mraa_init();
    printf("MRAA Version: %s\n", mraa_get_version());

    /* Assign GPIO pins. */
    y0  = mraa_gpio_init(2);    /* Y0  - P10 - gpio2 */
    y1  = mraa_gpio_init(3);    /* Y1  - P11 - gpio3 */
    y2  = mraa_gpio_init(1);    /* Y2  - P13 - gpio1 */
    y3  = mraa_gpio_init(6);    /* Y3  - P25 - gpio6 */
    y4  = mraa_gpio_init(18);   /* Y4  - P26 - gpio18 */
    sdi = mraa_gpio_init(19);   /* SDI - P27 - gpio19 */
    clk = mraa_gpio_init(17);   /* CLK - P28 - gpio17 */
    le  = mraa_gpio_init(16);   /* /LE - P29 - gpio16 */
    oe  = mraa_gpio_init(15);   /* /OE - P30 - gpio15 */
    ext = mraa_gpio_init(14);   /* EXT - P31 - gpio14 */
    sensor = mraa_gpio_init(4); /* Sensor - P21 - gpio4 */
    if (!y0 || !y1 || !y2 || !y3 || !y4 || !sdi || !clk || !le || !oe || !ext || !sensor) {
        fprintf(stderr, "Cannot assign GPIO pins!");
        exit(-1);
    }

    /* Configure pins as output. */
    set_output(y0);
    set_output(y1);
    set_output(y2);
    set_output(y3);
    set_output(y4);
    set_output(sdi);
    set_output(clk);
    set_output(le);
    set_output(oe);
    set_output(ext);

    /* Configure inputs. */
    set_input(sensor);
}

void gpio_ext(int on)
{
    /* EXT signal. */
    mraa_gpio_write(ext, on & 1);
}

void gpio_oe(int active)
{
    /* /OE signal, active low. */
    mraa_gpio_write(oe, !active);
}

void gpio_le(int active)
{
    /* /LE signal, active low. */
    mraa_gpio_write(le, !active);
}

void gpio_backlight_upper(int on)
{
    /* Y4 signal. */
    mraa_gpio_write(y4, on & 1);
}

void gpio_backlight_lower(int on)
{
    /* Y3 signal. */
    mraa_gpio_write(y3, on & 1);
}

void gpio_layer(int z)
{
    /* Y0-Y2 signals. */
    mraa_gpio_write(y0, ~z & 1);
    mraa_gpio_write(y1, (~z >> 1) & 1);
    mraa_gpio_write(y2, (~z >> 2) & 1);
}

void gpio_plane(unsigned char *data)
{
    int i, n, val;

    /* Send 8 bytes of data to the shift registers. */
    for (i=0; i<8; i+=2) {
        val = *data++;
        for (n=0; n<8; n++) {
            /* SDI signal. */
            mraa_gpio_write(sdi, (val >> 7) & 1);

            /* CLK signal. */
            mraa_gpio_write(clk, 1);
            mraa_gpio_write(clk, 0);

            val <<= 1;
        }
        val = *data++;
        for (n=0; n<8; n++) {
            /* SDI signal. */
            mraa_gpio_write(sdi, val & 1);

            /* CLK signal. */
            mraa_gpio_write(clk, 1);
            mraa_gpio_write(clk, 0);

            val >>= 1;
        }
    }
}

int gpio_sensor()
{
    /* Sensor signal. */
    return mraa_gpio_read(sensor);
}
