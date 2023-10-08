#include <stdint.h>

#include <raylib.h>
#include "../core/display.h"

#include "draw_rectangles.h"

void DrawRectangles::run(
    ColorBuffer *color_buffer) {
    draw_rectangle( color_buffer, {10, 10}, 50, 50, RED);
    draw_rectangle( color_buffer, {45, 45}, 100, 50, GREEN);
    draw_rectangle( color_buffer, {-45, 45}, 100, 50, BLUE);
};
