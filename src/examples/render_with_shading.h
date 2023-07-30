#ifndef __RENDERING_MESH__
#define __RENDERING_MESH__

#include <stdint.h>

#include "../color_buffer.h"
#include "../mesh.h"

namespace tinyrenderer::ShadedRendering {
struct Program {
    void init();
    void run(ColorBuffer *color_buffer, Mesh *mesh);
    void cleanup();
};
} // namespace tinyrenderer::program
#endif
