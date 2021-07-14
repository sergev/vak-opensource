#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void print_all_regs();

void app_main()
{
    print_all_regs();
    fflush(stdout);

    for (;;) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
