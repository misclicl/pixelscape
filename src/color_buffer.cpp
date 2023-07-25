#include <cstdlib>

#include "raylib.h"

#include "color_buffer.h"

namespace tinyrenderer {
uint32_t *ColorBuffer::get_buffer_pixel(int x, int y) {
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return nullptr;
    }
    return &pixels[x + width * y];
};

uint32_t *ColorBuffer::get_buffer_pixel(int idx) {
    if (idx > width * height * sizeof(uint32_t)) {
        return nullptr;
    }
    return &pixels[idx];
}

void ColorBuffer::set_pixel(int x, int y, uint32_t color) {
    uint32_t *pixel = this->get_buffer_pixel(x, y);

    *pixel = color;
}

void ColorBuffer::set_pixel(int i, uint32_t color) {
    uint32_t *pixel = this->get_buffer_pixel(i);

    *pixel = color;
}

void ColorBuffer::clear(uint32_t color) {
    for (size_t i = 0; i < width * height; ++i) {
        set_pixel(i, color);
    }
}

void ColorBuffer::draw_to_texture() {
    // TODO: find out if it's possible to write content
    // of color_buffer as a whole
    for (size_t i = 0; i < width * height; ++i) {
        // Retrieve buffer color
        uint32_t *pixel = get_buffer_pixel(i);

        // Transform color
        Color color;
        color.r = (*pixel >> 24) & 0xFF; // Red channel
        color.g = (*pixel >> 16) & 0xFF; // Green channel
        color.b = (*pixel >> 8) & 0xFF;  // Blue channel
        color.a = *pixel & 0xFF;         // Alpha channel

        // Draw pixel
        int x = i % width;
        int y = i / width;

        DrawPixel(x, y, color);
    }
};
}
