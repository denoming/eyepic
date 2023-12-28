#include "Application.hpp"

#include <esp_log.h>

#include "camera/Camera.hpp"
#include "camera/ImageProvider.hpp"
#include "nn/Model.hpp"
#include "net/WiFi.hpp"
#include "Utils.hpp"

static const char* TAG = "EP<App>";

static WiFi wifi;

bool
Application::setup()
{
    if (not wifi.setup()) {
        ESP_LOGE(TAG, "Unable to setup WiFi");
        return false;
    }

    if (not wifi.connect()) {
        ESP_LOGE(TAG, "Unable to connect WiFi");
        return false;
    }

    taskDelay(1000 /* before init camera wait some delay after WiFi connecting */);

    if (not Camera::init()) {
        ESP_LOGE(TAG, "Unable to init camera");
        return false;
    }

    _nn = std::make_unique<NeuralNetwork>();
    if (not _nn->setup()) {
        ESP_LOGE(TAG, "Unable to setup neural network");
        return false;
    }

    if (_publisher = std::make_unique<Publisher>(); _publisher->connect()) {
        _publisher->publish("status", "Unknown");
    } else {
        ESP_LOGE(TAG, "Unable to connect publisher");
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
        ESP_LOGE(TAG, "Unable to get input");
        return false;
    }

    if (not ImageProvider::getImage(kModelImageCols, kModelImageRows, kModelImageChannels, input)) {
        ESP_LOGE(TAG, "Unable to get image");
        return true;
    }

    if (auto result = _nn->predict(); result == NeuralNetwork::Result::Person) {
        std::ignore = _publisher->publish("status", "Person");
    } else {
        std::ignore = _publisher->publish("status", "NotPerson");
    }

    return true;
}