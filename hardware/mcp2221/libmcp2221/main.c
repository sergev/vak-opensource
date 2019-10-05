/*
 * Project: MCP2221 HID Library
 * Author: Zak Kemble, contact@zakkemble.co.uk
 * Copyright: (C) 2015 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/mcp2221-hid-library/
 */
#include <stdio.h>
#include <unistd.h>
#include <hidapi/hidapi.h>

#include "libmcp2221.h"

static mcp2221_error readGPIOs(mcp2221_t* myDev)
{
    puts("~~~~~~~~~~~~");

    mcp2221_gpio_value_t values[MCP2221_GPIO_COUNT];
    mcp2221_error res = mcp2221_readGPIO(myDev, values);
    if (res != MCP2221_SUCCESS)
        return res;

    printf("GPIO 1: %d\n", values[1]);
    printf("GPIO 3: %d\n", values[3]);

    return res;
}

int main(void)
{
    puts("Starting!");

    mcp2221_init();

    // Get list of MCP2221s
    printf("Looking for devices... ");
    int count = mcp2221_find(MCP2221_DEFAULT_VID, MCP2221_DEFAULT_PID, NULL, NULL, NULL);
    printf("found %d devices\n", count);

    // Open whatever device was found first
    printf("Opening device... ");
    mcp2221_t* myDev = mcp2221_open();

    if (!myDev) {
        mcp2221_exit();
        puts("No MCP2221s found");
        return 0;
    }
    puts("done");

    // Configure GPIOs
    printf("Configuring GPIOs... ");
    mcp2221_gpioconfset_t gpioConf = mcp2221_GPIOConfInit();

    // Check GP DESIGNATION TABLE in the datasheet for what the dedicated and alternate functions are for each GPIO pin

    // Configure GPIO 0 and 2 as OUTPUT HIGH
    gpioConf.conf[0].gpios      = MCP2221_GPIO0 | MCP2221_GPIO2;
    gpioConf.conf[0].mode       = MCP2221_GPIO_MODE_GPIO;
    gpioConf.conf[0].direction  = MCP2221_GPIO_DIR_OUTPUT;
    gpioConf.conf[0].value      = MCP2221_GPIO_VALUE_HIGH;

    // Configure GPIO 1 and GPIO 3 as INPUT
    gpioConf.conf[1].gpios      = MCP2221_GPIO1 | MCP2221_GPIO3;
    gpioConf.conf[1].mode       = MCP2221_GPIO_MODE_GPIO;
    gpioConf.conf[1].direction  = MCP2221_GPIO_DIR_INPUT;

    // Apply config
    mcp2221_setGPIOConf(myDev, &gpioConf);

    // Also save config to flash
    mcp2221_saveGPIOConf(myDev, &gpioConf);

    puts("done");

    mcp2221_error res;

    while (1) {
        usleep(200*1000);

        res = readGPIOs(myDev);
        if (res != MCP2221_SUCCESS)
            break;

        // Get GPIO 0 and 2 to LOW
        res = mcp2221_setGPIO(myDev, MCP2221_GPIO0 | MCP2221_GPIO2, MCP2221_GPIO_VALUE_LOW);
        if (res != MCP2221_SUCCESS)
            break;

        usleep(200*1000);

        res = readGPIOs(myDev);
        if (res != MCP2221_SUCCESS)
            break;

        // Set GPIO0 to HIGH
        res = mcp2221_setGPIO(myDev, MCP2221_GPIO0, MCP2221_GPIO_VALUE_HIGH);
        if (res != MCP2221_SUCCESS)
            break;

        usleep(200*1000);

        res = readGPIOs(myDev);
        if (res != MCP2221_SUCCESS)
            break;

        // Set GPIO2 to HIGH
        res = mcp2221_setGPIO(myDev, MCP2221_GPIO2, MCP2221_GPIO_VALUE_HIGH);
        if (res != MCP2221_SUCCESS)
            break;
    }

    switch (res) {
        case MCP2221_SUCCESS:
            puts("No error");
            break;
        case MCP2221_ERROR:
            puts("General error");
            break;
        case MCP2221_INVALID_ARG:
            puts("Invalid argument, probably null pointer");
            break;
        case MCP2221_ERROR_HID:
            printf("USB HID Error: %ls\n", hid_error(myDev->handle));
            break;
        default:
            printf("Unknown error %d\n", res);
            break;
    }

    mcp2221_exit();
    return 0;
}
