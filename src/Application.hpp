#pragma once

#include "nn/PersonDetector.hpp"
#include "net/Publisher.hpp"

#include <memory>

class Application {
public:
    bool
    setup();

    void
    loop();

private:
    std::unique_ptr<PersonDetector> _detector;
    std::unique_ptr<Publisher> _publisher;
};