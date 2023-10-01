#include "depth_buffer.h"
#include <stdlib.h>

DepthBuffer* depth_buffer_create(uint16_t width, uint16_t height, float default_value) {
    DepthBuffer* buffer = (DepthBuffer*)malloc(sizeof(DepthBuffer));
    if (!buffer) {
        // Handle allocation failure for the buffer structure
        return nullptr;
    }

    buffer->width = width;
    buffer->height = height;
    buffer->size = (uint32_t)(width * height);
    buffer->data = (float*)malloc(buffer->size * sizeof(float));

    if (!buffer->data) {
        // Handle allocation failure for the data field
        free(buffer);
        return nullptr;
    }

    for (uint32_t i = 0; i < buffer->size; i++) {
        buffer->data[i] = default_value;
    }

    return buffer;
}

void depth_buffer_destroy(DepthBuffer *d_buffer) {
    free(d_buffer->data);
    d_buffer->data = nullptr;
}


float *buffer_pixel_get(DepthBuffer *d_buffer, size_t i) {
    if (i >= d_buffer->size) {
        return nullptr;
    }

    return &d_buffer->data[i];
};

float *buffer_pixel_get(DepthBuffer *d_buffer, uint16_t x, uint16_t y) {
    auto idx = d_buffer->width * y + x;

    if (idx >= d_buffer->size) {
        return nullptr;
    }

    return &d_buffer->data[idx];
};


void depth_buffer_set(DepthBuffer *d_buffer, uint16_t x, uint16_t y, float value) {
    auto buffer_ptr = buffer_pixel_get(d_buffer, x, y);
    *buffer_ptr = value;
};

void depth_buffer_set(DepthBuffer *d_buffer, uint16_t i, float value) {
    auto buffer_ptr = buffer_pixel_get(d_buffer, i);
    *buffer_ptr = value;
};

void depth_buffer_clear(DepthBuffer *d_buffer, float value) {
    for (size_t i = 0; i < d_buffer->size; i++) {
        d_buffer->data[i] = value;
    }
};
// void generate_depth_image_data(DepthBuffer *d_buffer, ColorBuffer *c_buffer) {};

