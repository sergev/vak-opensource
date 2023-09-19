// From https://www.digikey.com/en/maker/projects/raspberry-pi-pico-rp2040-sd-card-example-with-micropython-and-cc/e472c7f578734bfd96d437e68e670050
#include <stdio.h>
#include <alloca.h>
#include "fs.h"
#include "pico/stdlib.h"
#include "sd_card.h"
#include "fatfs.h"

#include "hardware/rtc.h"

void setup_date_time()
{
    // Start the real time clock.
    rtc_init();

    //TODO: get time/date from battery backed RTC or from filesystem
    datetime_t t = {
        .year  = 2023,
        .month = 02,
        .day   = 18,
        .dotw  = 6, // 0 is Sunday
        .hour  = 15,
        .min   = 02,
        .sec   = 30,
    };
    rtc_set_datetime(&t);
    sleep_us(64);
}

//
// Get date and time (local).
//
void get_datetime(int *year, int *month, int *day, int *dotw, int *hour, int *min, int *sec)
{
    datetime_t now;
    rtc_get_datetime(&now);

    *year = now.year;
    *month = now.month;
    *day = now.day;
    *dotw = now.dotw;
    *hour = now.hour;
    *min = now.min;
    *sec = now.sec;
}

int get_dotw(int year, int month, int day)
{
    // Solution by Tomohiko Sakamoto.
    static const int offset[] = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };

    year -= (month < 3);
    return (year + year/4 - year/100 + year/400 + offset[month - 1] + day) % 7;
}

int main()
{
    setup_date_time();

    fs_result_t fr;
    filesystem_t *fs = alloca(f_sizeof_filesystem_t());
    file_t *fil = alloca(f_sizeof_file_t());
    int ret;
    char buf[100];
    char filename[] = "test02.txt";

    // Initialize chosen serial port
    stdio_init_all();
    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }

    // Wait for user to press 'enter' to continue
    printf("\r\nSD card test. Press 'enter' to start.\r\n");
    while (true) {
        buf[0] = getchar();
        if ((buf[0] == '\r') || (buf[0] == '\n')) {
            break;
        }
    }

    // Initialize SD card
    if (!sd_init_driver()) {
        printf("ERROR: Could not initialize SD card\r\n");
        while (true);
    }

    // Mount drive
    fr = f_mount(fs, "0:", 1);
    if (fr != FR_OK) {
        printf("ERROR: Could not mount filesystem (%d)\r\n", fr);
        while (true);
    }

    // Open file for writing ()
    fr = f_open(fil, filename, FA_WRITE | FA_CREATE_ALWAYS);
    if (fr != FR_OK) {
        printf("ERROR: Could not open file (%d)\r\n", fr);
        while (true);
    }

    // Write something to file
    ret = f_printf(fil, "This is another test\r\n");
    if (ret < 0) {
        printf("ERROR: Could not write to file (%d)\r\n", ret);
        f_close(fil);
        while (true);
    }
    ret = f_printf(fil, "of writing to an SD card.\r\n");
    if (ret < 0) {
        printf("ERROR: Could not write to file (%d)\r\n", ret);
        f_close(fil);
        while (true);
    }

    // Close file
    fr = f_close(fil);
    if (fr != FR_OK) {
        printf("ERROR: Could not close file (%d)\r\n", fr);
        while (true);
    }

    // Open file for reading
    fr = f_open(fil, filename, FA_READ);
    if (fr != FR_OK) {
        printf("ERROR: Could not open file (%d)\r\n", fr);
        while (true);
    }

    // Print every line in file over serial
    printf("Reading from file '%s':\r\n", filename);
    printf("---\r\n");
    while (f_gets(buf, sizeof(buf), fil)) {
        printf(buf);
    }
    printf("\r\n---\r\n");

    // Close file
    fr = f_close(fil);
    if (fr != FR_OK) {
        printf("ERROR: Could not close file (%d)\r\n", fr);
        while (true);
    }

    // Unmount drive
    f_unmount("0:");

    // Loop forever doing nothing
    while (true) {
        sleep_ms(1000);
    }
}
