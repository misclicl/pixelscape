#pragma once

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

typedef std::bitset<24> RenderFlagSet;

struct RendererState {
    // Indexed with Rendering Flags
    RenderFlagSet flags;
    TextureFilterMode texture_filter_mode;
};

