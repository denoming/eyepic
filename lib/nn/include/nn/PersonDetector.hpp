#pragma once

#include "nn/Types.hpp"
#include "nn/NeuralNetwork.hpp"

#include <functional>

class PersonDetector {
public:
    inline static const int kScoreThreshold = 60;

    /**
     * If \c PersonScore >= kScoreThreshold Then
     *      => DetectionResult::Person
     * If \c PersonScore  < kScoreThreshold and \c NotPersonScore >= kScoreThreshold Then
     *      => DetectionResult::NotPerson
     * Else
     *      => DetectionResult::Unknown
     */
    using OnStateUpdate = void(DetectionResult);

    PersonDetector() = default;

    void
    onStatusUpdate(std::function<OnStateUpdate> callback);

    [[nodiscard]] bool
    setup();

    void
    detect();

private:
    void
    setState(DetectionResult newResult);

private:
    DetectionResult _result{DetectionResult::Unknown};
    int8_t* _imageInput{};
    std::function<OnStateUpdate> _onStateUpdate;
    std::unique_ptr<NeuralNetwork> _nn;
};