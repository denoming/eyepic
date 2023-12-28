#include "nn/NeuralNetwork.hpp"

#include "nn/Model.hpp"

#include <tensorflow/lite/micro/micro_interpreter.h>
#include <tensorflow/lite/micro/micro_mutable_op_resolver.h>
#include <tensorflow/lite/schema/schema_generated.h>

#include <esp_heap_caps.h>
#include <esp_log.h>

#include <cmath>

static const char* TAG = "EP[NeuralNetwork]";

using namespace tflite;

namespace {

#ifdef CONFIG_IDF_TARGET_ESP32S3
const int32_t kScratchBufSize = 39 * 1024;
#else
const int32_t kScratchBufSize = 0;
#endif
const int32_t kTensorArenaSize = 81 * 1024 + kScratchBufSize;

} // namespace

class NeuralNetwork::Impl {
public:
    using Resolver = MicroMutableOpResolver<5>;
    using Interpreter = MicroInterpreter;

    Impl() = default;

    ~Impl()
    {
        delete _interpreter;
        delete _resolver;

        if (_tensorArena != nullptr) {
            heap_caps_free(_tensorArena);
        }
    }

    [[nodiscard]] int8_t*
    input() const
    {
        if (_interpreter != nullptr) {
            TfLiteTensor* input = _interpreter->input(0);
            return input->data.int8;
        }
        return nullptr;
    }

    bool
    setup()
    {
        /* Map model into usable structure */
        _model = GetModel(gPersonModelData);
        if (_model->version() != TFLITE_SCHEMA_VERSION) {
            ESP_LOGE(TAG,
                     "Given model is schema version %lu not equal to supported version %lu",
                     _model->version(),
                     uint32_t(TFLITE_SCHEMA_VERSION));
            return false;
        }

        /* Allocate memory for tensor */
        if (_tensorArena == nullptr) {
            const uint32_t kMemoryCaps{MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT};
            _tensorArena = static_cast<uint8_t*>(heap_caps_malloc(kTensorArenaSize, kMemoryCaps));
            if (_tensorArena == nullptr) {
                ESP_LOGE(TAG, "Unable to allocate tensor arena memory");
                return false;
            }
        }

        /* Create resolver */
        if (_resolver == nullptr) {
            _resolver = new (std::nothrow) Resolver;
            _resolver->AddAveragePool2D();
            _resolver->AddConv2D();
            _resolver->AddDepthwiseConv2D();
            _resolver->AddReshape();
            _resolver->AddSoftmax();
        }

        /* Create interpreter */
        if (_interpreter == nullptr) {
            _interpreter = new (std::nothrow)
                Interpreter(_model, *_resolver, _tensorArena, kTensorArenaSize);
            if (TfLiteStatus status = _interpreter->AllocateTensors(); status != kTfLiteOk) {
                ESP_LOGE(TAG, "Unable to allocate tensors");
                return false;
            }
        }

        return true;
    }

    [[nodiscard]] Scores
    predict()
    {
        if (_interpreter == nullptr) {
            ESP_LOGE(TAG, "Neural network is not initialized");
            return kInvalidScores;
        }

        if (_interpreter->Invoke() != kTfLiteOk) {
            ESP_LOGE(TAG, "Prediction has failed");
            return kInvalidScores;
        }

        return {score(kPersonIndex), score(kNotPersonIndex)};
    }

private:
    [[nodiscard]] int
    score(size_t index) const
    {
        TfLiteTensor* output = _interpreter->output(0);
        if (output != nullptr) {
            const auto nScore = static_cast<int8_t>(output->data.uint8[index]);
            const auto fScore = float(nScore - output->params.zero_point) * output->params.scale;
            return lround(fScore * 100);
        }
        return 0;
    }

private:
    uint8_t* _tensorArena{nullptr};
    const tflite::Model* _model{nullptr};
    Resolver* _resolver{nullptr};
    Interpreter* _interpreter{nullptr};
};

NeuralNetwork::NeuralNetwork()
    : _impl{std::make_unique<Impl>()}
{
}

NeuralNetwork::~NeuralNetwork() = default;

[[nodiscard]] int8_t*
NeuralNetwork::input() const
{
    assert(_impl);
    return _impl->input();
}

[[nodiscard]] bool
NeuralNetwork::setup()
{
    assert(_impl);
    return _impl->setup();
}

[[nodiscard]] NeuralNetwork::Scores
NeuralNetwork::predict()
{
    assert(_impl);
    return _impl->predict();
}
