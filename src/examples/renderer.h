#ifndef __RENDERER_H__
#define __RENDERER_H__

#include <bitset>

enum TextureFilterMode {
    NEAREST_NEIGHBOR,
    BILINEAR,
};

enum RenderingFlags {
    CULL_BACKFACE,
    SHOW_VERTICES,
    SHOW_TRIANGLES,
    SHOW_WIREFRAME,

    USE_FACE_NORMALS,
    USE_SHADING,
    USE_Z_BUFFER,
};

struct RendererState {
    // Refer to RenderingFlags struct
    std::bitset<24> flags;
    TextureFilterMode texture_filter_mode;
};

#endif
