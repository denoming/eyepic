#include "Utils.hpp"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

void
taskDelay(uint32_t millis)
{
    vTaskDelay(pdMS_TO_TICKS(millis));
}

void
taskSuspend()
{
    vTaskSuspend(nullptr);
}