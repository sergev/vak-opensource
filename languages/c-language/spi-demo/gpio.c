#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "gpio.h"

#define GPIO_DEBUG 0
#if GPIO_DEBUG
    #define DEBUG_PRINT(__info,...) printf("Debug: " __info,##__VA_ARGS__)
#else
    #define DEBUG_PRINT(__info,...)
#endif

#define NUM_MAXBUF  4
#define DIR_MAXSIZ  60

int gpio_export(int pin)
{
    char buffer[NUM_MAXBUF];
    int len;
    int fd;

    fd = open("/sys/class/gpio/export", O_WRONLY);
    if (fd < 0) {
        DEBUG_PRINT( "Export Failed: Pin%d\n", pin);
        return -1;
    }

    len = snprintf(buffer, NUM_MAXBUF, "%d", pin);
    if (write(fd, buffer, len) != len) {
        // EBUSY - already exported.
        //perror(__func__);
        close(fd);
        return -1;
    }

    DEBUG_PRINT( "Export: Pin%d\n", pin);

    close(fd);
    return 0;
}

int gpio_unexport(int pin)
{
    char buffer[NUM_MAXBUF];
    int len;
    int fd;

    fd = open("/sys/class/gpio/unexport", O_WRONLY);
    if (fd < 0) {
        DEBUG_PRINT( "unexport Failed: Pin%d\n", pin);
        return -1;
    }

    len = snprintf(buffer, NUM_MAXBUF, "%d", pin);
    if (write(fd, buffer, len) != len) {
        perror(__func__);
        close(fd);
        return -1;
    }

    DEBUG_PRINT( "Unexport: Pin%d\n", pin);

    close(fd);
    return 0;
}

int gpio_set_mode(int pin, int mode)
{
    const char dir_str[] = "in\0out";
    char path[DIR_MAXSIZ];
    int fd;

    snprintf(path, DIR_MAXSIZ, "/sys/class/gpio/gpio%d/direction", pin);
    fd = open(path, O_WRONLY);
    if (fd < 0) {
        DEBUG_PRINT( "Set Direction failed: Pin%d\n", pin);
        return -1;
    }

    if (write(fd, &dir_str[mode == MODE_INPUT ? 0 : 3], mode == MODE_INPUT ? 2 : 3) < 0) {
        DEBUG_PRINT("failed to set direction!\n");
        return -1;
    }

    if (mode == MODE_INPUT) {
        DEBUG_PRINT("Pin%d: Input\n", pin);
    } else {
        DEBUG_PRINT("Pin%d: Output\n", pin);
    }

    close(fd);
    return 0;
}

int gpio_read(int pin)
{
    char path[DIR_MAXSIZ];
    char value_str[3];
    int fd;

    snprintf(path, DIR_MAXSIZ, "/sys/class/gpio/gpio%d/value", pin);
    fd = open(path, O_RDONLY);
    if (fd < 0) {
        DEBUG_PRINT( "Read failed Pin%d\n", pin);
        return -1;
    }

    if (read(fd, value_str, 3) < 0) {
        DEBUG_PRINT( "failed to read value!\n");
        return -1;
    }

    close(fd);
    return(atoi(value_str));
}

int gpio_write(int pin, int value)
{
    const char s_values_str[] = "01";
    char path[DIR_MAXSIZ];
    int fd;

    snprintf(path, DIR_MAXSIZ, "/sys/class/gpio/gpio%d/value", pin);
    fd = open(path, O_WRONLY);
    if (fd < 0) {
        DEBUG_PRINT( "Write failed : Pin%d,value = %d\n", pin, value);
        return -1;
    }

    if (write(fd, &s_values_str[value == 0 ? 0 : 1], 1) < 0) {
        DEBUG_PRINT( "failed to write value!\n");
        return -1;
    }

    close(fd);
    return 0;
}
