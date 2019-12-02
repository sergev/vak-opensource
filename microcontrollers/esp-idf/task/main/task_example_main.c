#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static void task_example(void *arg)
{
    // Get task name.
    const char *task_name = pcTaskGetTaskName(NULL);

    for (;;) {
        unsigned msec = xTaskGetTickCount() * portTICK_PERIOD_MS;

        // Print task name and current time.
        printf("Hello from `%s'! msec = %d\n", task_name, msec);

        // Pause for 100 msec.
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void app_main(void)
{
    // Start task.
    printf("Testing tasks and timer.\n");
    xTaskCreate(task_example, "task_example", 2048, NULL, 10, NULL);
}
