#pragma once

#include <memory>

class NeuralNetwork {
public:
    /**
     * If \c PersonScore >= 60                             => Person
     * If \c PersonScore  < 60 and \c NotPersonScore >= 60 => NotPerson
     * Else                                                => Unknown
     */
    inline static const int kScoreThreshold = 60;

    enum Result { Person, NotPerson, Unknown };

    NeuralNetwork();

    ~NeuralNetwork();

    [[nodiscard]] int8_t*
    input() const;

    [[nodiscard]] bool
    setup();

    [[nodiscard]] Result
    predict();

private:
    class Impl;
    std::unique_ptr<Impl> _impl;
};