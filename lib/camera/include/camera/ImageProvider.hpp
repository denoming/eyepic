#pragma once

#include <esp_types.h>

class ImageProvider {
public:
    static bool
    getImage(int width, int height, int channels, int8_t* output);
};