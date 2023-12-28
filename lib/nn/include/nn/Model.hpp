#pragma once

#include <esp_types.h>

/* Detection model image  */
const size_t kImageCols = 96;
const size_t kImageRows = 96;
const size_t kImageChannels = 1 /* Greyscale */;
const size_t kImageSize = kImageCols * kImageRows * kImageChannels;

/* Detection model category labels */
const size_t kCategoryCount = 2;
const size_t kPersonIndex = 1;
const size_t kNotPersonIndex = 0;
extern const char* kCategoryLabels[kCategoryCount];

/* Person detection model data */
extern const unsigned char gPersonModelData[];
/* Person detection model size in bytes */
extern const int gPersonModelDataLen;