#include <cstdlib>

#include "raylib.h"

#include "color_buffer.h"
#include "tiny_color.h"

namespace pixelscape {
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
    if (x < 0 || x >= this->width || y < 0 || y >= this->height) {
        return;
    }

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
    for (size_t i = 0; i < width * height; ++i) {
        // Retrieve buffer color
        uint32_t *pixel = get_buffer_pixel(i);
        Color color = make_raylib(*pixel);

        // Draw pixel
        int x = i % width;
        int y = i / width;

        DrawPixel(x, y, color);
    }
};
}

