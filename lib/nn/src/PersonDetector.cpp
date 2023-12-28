#include "nn/PersonDetector.hpp"

#include "camera/ImageProvider.hpp"
#include "nn/Model.hpp"

#include <esp_log.h>

static const char* TAG = "EP<PersonDetector>";

void
PersonDetector::onStatusUpdate(std::function<OnStateUpdate> callback)
{
    _onStateUpdate = std::move(callback);
}

bool
PersonDetector::setup()
{
    if (_nn = std::make_unique<NeuralNetwork>(); not _nn->setup()) {
        ESP_LOGE(TAG, "Unable to setup neural network");
        return false;
    }

    assert(_nn);
    _imageInput = _nn->input();
    if (_imageInput == nullptr) {
        ESP_LOGE(TAG, "Unable to get image input");
        return false;
    }

    return true;
}

void
PersonDetector::detect()
{
    if (not ImageProvider::getImage(kImageCols, kImageRows, kImageChannels, _imageInput)) {
        ESP_LOGE(TAG, "Unable to get image");
        return;
    }

    const auto [pScore, _] = _nn->predict();
    setState((pScore >= kScoreThreshold) ? DetectionResult::Person : DetectionResult::NotPerson);
}

void
PersonDetector::setState(DetectionResult newResult)
{
    if (_result != newResult) {
        _result = newResult;
        if (_onStateUpdate) {
            _onStateUpdate(newResult);
        }
    }
}