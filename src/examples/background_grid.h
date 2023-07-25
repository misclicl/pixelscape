#ifndef __BACKGROUND_GRID__
#define __BACKGROUND_GRID__

#include <stdint.h>
#include "../color_buffer.h"

namespace tinyrenderer::program {
struct BackgroundGrid{
    void run(ColorBuffer *color_buffer);
};
}
#endif
