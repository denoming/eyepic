#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <esp_log.h>

#include "Application.hpp"
#include "Utils.hpp"

static const char* TAG = "EP<Main>";

[[noreturn]] void
xmain(void* /*param*/)
{
    Application app;

    if (not app.setup()) {
        ESP_LOGE(TAG, "Unable to setup application");
        taskSuspend();
    }

    while (true) {
        app.loop();
        taskDelay(100);
    }
}

extern "C" void
app_main()
{
    xTaskCreatePinnedToCore((TaskFunction_t) &xmain, "xmain", 6 * 1024, nullptr, 8, nullptr, 1);
    vTaskDelete(nullptr);
}