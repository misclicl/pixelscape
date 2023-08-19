#ifndef __ORTHOGRAHIC_PROJECTION__
#define __ORTHOGRAHIC_PROJECTION__

#include "../core/color_buffer.h"
#include <stdint.h>

struct Projection {
    void run(ColorBuffer *color_buffer, int delta);
};
#endif
