#ifndef __RENDERER_H__
#define __RENDERER_H__

#include <bitset>

enum TextureFilterMode {
    NEAREST_NEIGHBOR,
    BILINEAR,
};

enum RenderingFlags {
    SHOW_VERTICES    = 0,
    SHOW_TRIANGLES   = 1,
    SHOW_WIREFRAME   = 2,

    USE_FACE_NORMALS = 3,
    USE_SHADING      = 4,
    USE_Z_BUFFER     = 5,

    CULL_BACKFACE    = 23,
};

struct RendererState {
    // Indexed with Rendering Flags
    std::bitset<24> flags;
    TextureFilterMode texture_filter_mode;
};

#endif
