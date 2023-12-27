#include "Application.hpp"

#include <esp_log.h>

#include "camera/Camera.hpp"
#include "camera/ImageProvider.hpp"

#include "nn/Model.hpp"

static const char* TAG = "EP<App>";

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

bool
Application::loop()
{
    int8_t* input = _nn->input();
    if (input == nullptr) {
        ESP_LOGE(TAG, "Unable to get image");
        return false;
    }

    if (not ImageProvider::getImage(kModelImageCols, kModelImageRows, kModelImageChannels, input)) {
        ESP_LOGE(TAG, "Unable to get image");
        return false;
    }

    if (auto result = _nn->predict(); result == NeuralNetwork::Result::Person) {
        ESP_LOGI(TAG, "Person");
    } else {
        ESP_LOGI(TAG, "NOT person");
    }

    return true;
}