#ifndef __BACKGROUND_GRID__
#define __BACKGROUND_GRID__

#include "../color_buffer.h"
#include <stdint.h>

namespace tinyrenderer::program {
struct BackgroundGrid {
    void run(ColorBuffer *color_buffer);
};
} // namespace tinyrenderer::program
#endif
