#include "camera/ImageProvider.hpp"

#include <esp_camera.h>

bool
ImageProvider::getImage(int width, int height, int channels, int8_t* output)
{
    camera_fb_t* fb = esp_camera_fb_get();
    if (fb == nullptr) {
        return false;
    }
    for (size_t i = 0; i < width * height; ++i) {
        output[i] = int8_t(fb->buf[i] ^ 0x80);
    }
    esp_camera_fb_return(fb);
    return true;
}