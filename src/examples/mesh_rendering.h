#ifndef __RENDERING_MESH__
#define __RENDERING_MESH__

#include <bitset>
#include <stdint.h>

#include "../core/color_buffer.h"
#include "../mesh.h"

namespace tinyrenderer::MeshRendering {
enum RenderingFlags {
    BACKFACE_CULLING = 0,
    DISPLAY_NORMALS = 1,
    DISPLAY_VERTICES = 2,
};
    
struct Program {
    void init();
    void run(ColorBuffer *color_buffer);
    void cleanup();

    Mesh mesh;
    std::bitset<24> render_flags;
};
} // namespace tinyrenderer::MeshRendering
#endif
