#ifndef __RENDERING_MESH__
#define __RENDERING_MESH__

#include <bitset>
#include <stdint.h>

#include "../core/color_buffer.h"
#include "../core/matrix.h"
#include "../mesh.h"
#include "../light.h"

namespace pixelscape::MeshRendering {
enum RenderingFlags {
    BACKFACE_CULLING,
    VERTEX_ORDERING,
    DISPLAY_VERTICES,
    DISPLAY_TRIANGLES,
    DISPLAY_WIREFRAME,
    
    ENABLE_FACE_NORMALS,
    ENABLE_SHADING,
    ENABLE_Z_BUFFER_CHECK,
};

struct FaceBufferItem {
    Vec4f vertices[3];
    Vec2f texcoords[3];
    Vec3f triangle_normal;

    TinyColor color;
};
    
struct Program {
    void init(int width, int height);
    void run(ColorBuffer *color_buffer);
    void cleanup();
    void project_mesh(
        ColorBuffer *color_buffer,
        Matrix4 *mat_world,
        Matrix4 *mat_view
    );
    void render_mesh(ColorBuffer *color_buffer, Light *light);

    void handle_input();

    Light light;
    Mesh mesh;
    std::bitset<24> render_flags;

    FaceBufferItem *face_buffer;
    size_t face_buffer_size = 0;

    float *depth_buffer;
};
} // namespace pixelscape::MeshRendering
#endif
