#pragma once

#include "nn/NeuralNetwork.hpp"

#include <memory>

class Application {
public:
    bool
    setup();

    bool
    loop();

private:
    std::unique_ptr<NeuralNetwork> _nn;
};