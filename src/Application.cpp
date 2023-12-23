#include "Application.hpp"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <esp_log.h>

#include "camera/Camera.hpp"
#include "camera/ImageProvider.hpp"

#include "nn/Model.hpp"

static const char* TAG = "EP<App>";

namespace {

inline void
taskDelay()
{
    vTaskDelay(pdMS_TO_TICKS(100));
}

} // namespace

bool
Application::setup()
{
    if (not Camera::init()) {
        ESP_LOGE(TAG, "Unable to init camera");
        return false;
    }

    _nn = std::make_unique<NeuralNetwork>();
    if (not _nn->setup()) {
        ESP_LOGE(TAG, "Unable to setup neural network");
        return false;
    }

    ESP_LOGI(TAG, "Application is ready");

    return true;
}

void
Application::loop()
{
    int8_t* input = _nn->input();
    if (input == nullptr) {
        ESP_LOGE(TAG, "Unable to get image");
        taskDelay();
        return;
    }

    if (not ImageProvider::getImage(kModelImageCols, kModelImageRows, kModelImageChannels, input)) {
        ESP_LOGE(TAG, "Unable to get image");
        taskDelay();
        return;
    }

    if (auto result = _nn->predict(); result == NeuralNetwork::Result::Person) {
        ESP_LOGI(TAG, "Person");
    } else {
        ESP_LOGI(TAG, "NOT person");
    }

    taskDelay();
}