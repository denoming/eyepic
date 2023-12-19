#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <esp_log.h>

#include "Application.hpp"

static const char* TAG = "EP<Main>";

[[noreturn]] int
main()
{
    Application app;
    if (not app.setup()) {
        ESP_LOGE(TAG, "Unable to setup application");
    } else {
        while (true) {
            app.loop();
        }
    }
}

extern "C" void
app_main()
{
    xTaskCreate((TaskFunction_t) &main, "main", 4 * 1024, nullptr, 8, nullptr);
    vTaskDelete(nullptr);
}