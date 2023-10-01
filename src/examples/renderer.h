#ifndef __RENDERER_H__
#define __RENDERER_H__

#include <bitset>

enum TextureFilterMode {
    NEAREST_NEIGHBOR,
    BILINEAR,
};

enum RenderingFlags {
    DRAW_VERTICES,
    DRAW_TRIANGLES,
    DRAW_WIREFRAME,
    DRAW_DEPTH_BUFFER,

    USE_FACE_NORMALS,
    USE_SHADING,
    USE_Z_BUFFER,

    CULL_BACKFACE,
};

struct RendererState {
    // Indexed with Rendering Flags
    std::bitset<24> flags;
    TextureFilterMode texture_filter_mode;
};

#endif
