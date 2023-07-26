#ifndef __ORTHOGRAHIC_PROJECTION__
#define __ORTHOGRAHIC_PROJECTION__

#include "../color_buffer.h"
#include <stdint.h>

namespace tinyrenderer::program {

struct Projection {
    void run(ColorBuffer *color_buffer, int delta);
};
} // namespace tinyrenderer::program
#endif
