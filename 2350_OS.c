#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/platform.h"

#include "FreeRTOS.h"
#include "task.h"

#define TASK_STACK_SIZE   1024
#define TASK_PRIORITY     ( tskIDLE_PRIORITY + 1 )

static void task_a(void *param)
{
    (void)param;
    uint32_t count = 0;
    while (true) {
        printf("[task A] core %u, count %lu\n", get_core_num(), count++);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

static void task_b(void *param)
{
    (void)param;
    uint32_t count = 0;
    while (true) {
        printf("[task B] core %u, count %lu\n", get_core_num(), count++);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

int main()
{
    // On this board stdio already uses uart1 on GP4/GP5 at 115200
    stdio_init_all();
    printf("FreeRTOS %s starting on RP2350\n", tskKERNEL_VERSION_NUMBER);

    xTaskCreate(task_a, "task_a", TASK_STACK_SIZE, NULL, TASK_PRIORITY, NULL);
    xTaskCreate(task_b, "task_b", TASK_STACK_SIZE, NULL, TASK_PRIORITY, NULL);

    vTaskStartScheduler();

    // Only reached if the scheduler could not allocate the idle/timer tasks
    while (true) {
        tight_loop_contents();
    }
}

void vApplicationMallocFailedHook(void)
{
    printf("FreeRTOS: malloc failed\n");
    panic("malloc failed");
}

void vApplicationStackOverflowHook(TaskHandle_t task, char *name)
{
    (void)task;
    panic("stack overflow in task %s", name);
}
