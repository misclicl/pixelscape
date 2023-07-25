#include <stdint.h>

#include "background_grid.h"
#include "display.h"

void draw_grid(uint32_t *color_buffer, uint32_t color, int width, int height) {
    int n = 8;

    // vertical lines
    for (int x = n; x < width; x += n) {
        tinyrenderer::draw_line(
            color_buffer, 
            {(float)x, 0 ,0}, 
            {(float)x, (float)(height - 1), 0}, 
            color,
            width,
            height
        );
    }

    for (int y = n; y < height; y += n) {
        tinyrenderer::draw_line(
            color_buffer, 
            {0, (float)y,0}, 
            {(float)(width - 1), (float)y, 0}, 
            color,
            width,
            height
        );
    }
}

void tinyrenderer::program::BackgroundGrid::run(
    uint32_t *color_buffer,
    int width,
    int height
) {
    clear_color_buffer(color_buffer, width, height, 0xFF000000);
    draw_grid(color_buffer, 0x202020FF, width, height);
    draw_color_buffer(color_buffer, width, height);
};
