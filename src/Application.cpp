#include "Application.hpp"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <esp_log.h>

#include "Camera.hpp"

static const char* TAG = "EP<App>";

bool
Application::setup()
{
    if (not Camera::init()) {
        ESP_LOGE(TAG, "Unable to init camera");
        return false;
    }

    ESP_LOGI(TAG, "Application is ready");

    return true;
}

void
Application::loop()
{
    vTaskDelay(pdMS_TO_TICKS(100));
}