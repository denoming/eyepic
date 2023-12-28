#pragma once

#include <memory>
#include <tuple>

class NeuralNetwork {
public:
    using Scores = std::tuple<int /* Person score */, int /* Not person score */>;

    /* Score value for error cases */
    inline static Scores kInvalidScores = Scores{-1, -1};

    NeuralNetwork();

    ~NeuralNetwork();

    [[nodiscard]] int8_t*
    input() const;

    [[nodiscard]] bool
    setup();

    [[nodiscard]] Scores
    predict();

private:
    class Impl;
    std::unique_ptr<Impl> _impl;
};