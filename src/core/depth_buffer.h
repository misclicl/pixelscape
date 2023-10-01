#pragma once

// #include "color_buffer.h"
#include <cstddef>
#include <stdint.h>

struct DepthBuffer {
    uint16_t width;
    uint16_t height;
    float *data = nullptr;
    size_t size;
};

DepthBuffer* depth_buffer_create(uint16_t width, uint16_t height, float default_value);
void depth_buffer_destroy(DepthBuffer *d_buffer);

float *buffer_pixel_get(DepthBuffer *d_buffer, uint16_t x, uint16_t y);
float *buffer_pixel_get(DepthBuffer *d_buffer, size_t idx);

void depth_buffer_set(DepthBuffer *d_buffer, uint16_t x, uint16_t y, float value);
void depth_buffer_set(DepthBuffer *d_buffer, uint16_t i, float value);

void depth_buffer_clear(DepthBuffer *d_buffer, float value);
// void generate_depth_image_data(DepthBuffer *d_buffer, ColorBuffer *c_buffer);
