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

    taskDelay(1000);

    if (not Camera::init()) {
        ESP_LOGE(TAG, "Unable to init camera");
        return false;
    }

    taskDelay(1000);

    if (_detector = std::make_unique<PersonDetector>();_detector->setup()) {
        _detector->onStatusUpdate([this](const DetectionResult result) {
            assert(_publisher);
            if (result == DetectionResult::Person) {
                _publisher->publish("status", "Person");
            } else {
                _publisher->publish("status", "NotPerson");
            }
        });
    } else {
        ESP_LOGE(TAG, "Unable to setup detector");
        return false;
    }

    taskDelay(1000);

    if (_publisher = std::make_unique<Publisher>(); _publisher->connect()) {
        _publisher->publish("status", "Unknown");
    } else {
        ESP_LOGE(TAG, "Unable to connect publisher");
        return false;
    }

    ESP_LOGI(TAG, "Application is ready");

    return true;
}

void
Application::loop()
{
    assert(_detector);
    _detector->detect();
}