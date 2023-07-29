#ifndef __RENDERING_CUBE__
#define __RENDERING_CUBE__

#include <stdint.h>

#include "../color_buffer.h"
#include "../mesh.h"

namespace tinyrenderer::MeshRendering {
enum Mode {
    Wireframe = 0,
    Triangles = 1,
};

struct Program {
    void init();
    void run(ColorBuffer *color_buffer, int delta);
    void cleanup();

    Mesh mesh;
    Mode mode;
};
} // namespace tinyrenderer::program
#endif
