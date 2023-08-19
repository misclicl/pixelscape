#include <stdint.h>

#include "../core/display.h"
#include "draw_rectangles.h"

void DrawRectangles::run(
    ColorBuffer *color_buffer) {
    draw_rectangle(
        color_buffer,
        {10, 10},
        50,
        50,
        0xFF0000FF
    );
    draw_rectangle(
        color_buffer,
        {45, 45},
        100,
        50,
        0xFF00FFFF
    );
    draw_rectangle(
        color_buffer,
        {-45, 45},
        100,
        50,
        0xFFFFFFFF
    );
};
