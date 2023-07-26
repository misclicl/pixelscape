#ifndef __PERS_PROJ__
#define __PERS_PROJ__

#include "../color_buffer.h"

namespace tinyrenderer::program {
struct PerspectiveProjection {
    void run(ColorBuffer *color_buffer, float near);
};
}
#endif
