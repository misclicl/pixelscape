#ifndef __BACKGROUND_GRID__
#define __BACKGROUND_GRID__

#include <stdint.h>

namespace tinyrenderer::program {
struct BackgroundGrid{
    void run(uint32_t *colorBuffer, int width, int height);
};
}
#endif
