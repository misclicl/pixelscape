#ifndef __RENDERING_MESH__
#define __RENDERING_MESH__

#include <stdint.h>

#include "../core/color_buffer.h"
#include "../mesh.h"

namespace tinyrenderer::MeshRendering {
enum Mode {
    Wireframe = 0,
    Triangles = 1,
};

struct Program {
    void init();
    void run(ColorBuffer *color_buffer);
    void cleanup();

    Mesh mesh;
    Mode mode;
    bool backface_culling = true;
};
} // namespace tinyrenderer::program
#endif
