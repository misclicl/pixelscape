#include <cstdlib>

#include "raylib.h"

#include "color_buffer.h"
#include "tiny_color.h"

Color *ColorBuffer::get_buffer_pixel(int x, int y) {
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return nullptr;
    }
    return &pixels[x + width * y];
};

Color *ColorBuffer::get_buffer_pixel(int idx) {
    if (idx > width * height * sizeof(Color)) {
        return nullptr;
    }
    return &pixels[idx];
}

void ColorBuffer::set_pixel(int x, int y, Color color) {
    if (x < 0 || x >= this->width || y < 0 || y >= this->height) {
        return;
    }

    Color *pixel = this->get_buffer_pixel(x, y);
    *pixel = color;
}

void ColorBuffer::set_pixel(int i, Color color) {
    Color *pixel = this->get_buffer_pixel(i);

    *pixel = color;
}

void ColorBuffer::clear(Color color) {
    for (size_t i = 0; i < width * height; ++i) {
        set_pixel(i, color);
    }
}

void ColorBuffer::draw_to_texture() {
    for (size_t i = 0; i < width * height; ++i) {
        // Retrieve buffer color
        auto pixel = get_buffer_pixel(i);

        // Draw pixel
        int x = i % width;
        int y = i / width;

        DrawPixel(x, y, *pixel);
    }
};
