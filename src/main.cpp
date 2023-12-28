#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <esp_log.h>

#include "Application.hpp"
#include "Utils.hpp"

static const char* TAG = "EP<Main>";

void
xmain(void* /*param*/)
{
    Application app;
    if (not app.setup()) {
        ESP_LOGE(TAG, "Unable to setup application");
        taskSuspend();
    } else {
        bool result;
        do {
            result = app.loop();
            if (result) {
                taskDelay(100);
            } else {
                ESP_LOGE(TAG, "Unable to loop application");
                taskSuspend();
            }
        }
        while (result);
    }
}

extern "C" void
app_main()
{
    xTaskCreate((TaskFunction_t) &xmain, "xmain", 6 * 1024, nullptr, 8, nullptr);
    vTaskDelete(nullptr);
}