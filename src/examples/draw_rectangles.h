#ifndef __DRAW_RECTANGLES__
#define __DRAW_RECTANGLES__

#include <stdint.h>

#include "../core/color_buffer.h"

namespace pixelscape::program {
struct DrawRectangles {
    void run(ColorBuffer *colorBuffer);
};
} // namespace pixelscape::program
#endif
