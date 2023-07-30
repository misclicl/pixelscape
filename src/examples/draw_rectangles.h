#ifndef __DRAW_RECTANGLES__
#define __DRAW_RECTANGLES__

#include <stdint.h>

#include "../core/color_buffer.h"

namespace tinyrenderer::program {
struct DrawRectangles {
    void run(ColorBuffer *colorBuffer);
};
} // namespace tinyrenderer::program
#endif
