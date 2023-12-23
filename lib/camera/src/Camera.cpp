#include "camera/Camera.hpp"

#include "camera/Config.hpp"

#include <esp_camera.h>
#include <esp_log.h>

static const char* TAG = "EP<Camera>";

bool
Camera::init()
{
#if CONFIG_EP_CAM_MODULE_AI_THINKER || CONFIG_CAMERA_MODULE_ESP32_CAM_BOARD
    /* IO13, IO14 is designed for JTAG by default,
     * to use it as generalized input,
     * firstly declare it as pull up input */
    gpio_config_t gc;
    gc.mode = GPIO_MODE_INPUT;
    gc.pull_up_en = GPIO_PULLUP_ENABLE;
    gc.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gc.intr_type = GPIO_INTR_DISABLE;
    gc.pin_bit_mask = 1LL << 13;
    gpio_config(&gc);
    gc.pin_bit_mask = 1LL << 14;
    gpio_config(&gc);
#endif

    camera_config_t cc;
    cc.pin_pwdn = EP_CAM_PIN_PWDN;
    cc.pin_reset = EP_CAM_PIN_RESET;
    cc.pin_xclk = EP_CAM_PIN_XCLK;
    cc.pin_sccb_sda = EP_CAM_PIN_SIOD;
    cc.pin_sccb_scl = EP_CAM_PIN_SIOC;
    cc.pin_d0 = EP_CAM_PIN_D0;
    cc.pin_d1 = EP_CAM_PIN_D1;
    cc.pin_d2 = EP_CAM_PIN_D2;
    cc.pin_d3 = EP_CAM_PIN_D3;
    cc.pin_d4 = EP_CAM_PIN_D4;
    cc.pin_d5 = EP_CAM_PIN_D5;
    cc.pin_d6 = EP_CAM_PIN_D6;
    cc.pin_d7 = EP_CAM_PIN_D7;
    cc.pin_vsync = EP_CAM_PIN_VSYNC;
    cc.pin_href = EP_CAM_PIN_HREF;
    cc.pin_pclk = EP_CAM_PIN_PCLK;

    cc.xclk_freq_hz = EP_CAM_XCLK_FREQ_HZ;
    cc.ledc_timer = EP_CAM_LEDC_TIMER;
    cc.ledc_channel = EP_CAM_LEDC_CHANNEL;
    cc.jpeg_quality = 10;
    cc.fb_count = 2; /* Enable continuous mode */
    cc.fb_location = CAMERA_FB_IN_PSRAM;
    cc.grab_mode = CAMERA_GRAB_LATEST;

    /* Pixel format and frame size values are fixed to match trained model */
    cc.pixel_format = PIXFORMAT_GRAYSCALE;
    cc.frame_size = FRAMESIZE_96X96;

    if (esp_err_t err = esp_camera_init(&cc); err == ESP_OK) {
        ESP_LOGI(TAG, "Camera module <%s> was configured", EP_CAM_MODULE_NAME);
    } else {
        ESP_LOGE(TAG, "Unable to init camera: 0x%x", err);
        return false;
    }

    sensor_t* sensor = esp_camera_sensor_get();
    sensor->set_vflip(sensor, 1); /* Flip back */
    if (sensor->id.PID == OV3660_PID) {
        sensor->set_brightness(sensor, 1);  /* Increase the brightness */
        sensor->set_saturation(sensor, -2); /* Decrease the saturation */
    }

    return true;
}

void
Camera::fini()
{
    esp_camera_deinit();
}