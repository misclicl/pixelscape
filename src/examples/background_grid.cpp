#include <stdint.h>

#include "../core/color_buffer.h"
#include "../core/display.h"
#include "background_grid.h"

namespace tinyrenderer {
void draw_grid(ColorBuffer *color_buffer, uint32_t color) {
    int n = 32;

    for (int x = n; x < color_buffer->width; x += n) {
        tinyrenderer::draw_line(
            color_buffer,
            {(float)x, 0, 0},
            {(float)x, (float)(color_buffer->height - 1), 0},
            color);
    }

    for (int y = n; y < color_buffer->height; y += n) {
        tinyrenderer::draw_line(
            color_buffer,
            {0, (float)y, 0},
            {(float)(color_buffer->width - 1), (float)y, 0},
            color);
    }
}

void program::BackgroundGrid::run(
    tinyrenderer::ColorBuffer *color_buffer) {
    draw_grid(color_buffer, 0x101010FF);
};
} // namespace tinyrenderer
