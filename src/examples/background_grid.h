#ifndef __BACKGROUND_GRID__
#define __BACKGROUND_GRID__

#include "../core/color_buffer.h"
#include <stdint.h>

struct BackgroundGrid {
    void run(ColorBuffer *color_buffer);
};

#endif
