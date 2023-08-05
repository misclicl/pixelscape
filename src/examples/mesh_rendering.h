#ifndef __RENDERING_MESH__
#define __RENDERING_MESH__

#include <bitset>
#include <stdint.h>

#include "../core/color_buffer.h"
#include "../mesh.h"
#include "../light.h"

namespace tinyrenderer::MeshRendering {
enum RenderingFlags {
    BACKFACE_CULLING,
    VERTEX_ORDERING,
    DISPLAY_NORMALS,
    DISPLAY_VERTICES,
    DISPLAY_TRIANGLES,
    DISPLAY_WIREFRAME,
    
    ENABLE_FACE_NORMALS,
    ENABLE_SHADING,
};

struct FaceBufferItem {
    Vec3f vertices[3];
    Vec3f normal_vec;
    TinyColor color;
    float avg_depth;
};
    
struct Program {
    void init();
    void run(ColorBuffer *color_buffer);
    void cleanup();
    void project_mesh(
        ColorBuffer *color_buffer
    );
    void render_mesh(ColorBuffer *color_buffer);

    void handle_input();

    Light light;
    Mesh mesh;
    std::bitset<24> render_flags;

    std::vector<FaceBufferItem> faces_to_render;
};
} // namespace tinyrenderer::MeshRendering
#endif
