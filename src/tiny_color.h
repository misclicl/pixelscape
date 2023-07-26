#ifndef __TINY_COLOR__
#define __TINY_COLOR__

#include "raylib.h"
#include <cstdint>

namespace tinyrenderer {
typedef uint32_t TinyColor;

inline Color make_raylib(TinyColor color) {
    Color out;

    out.r = (color >> 24) & 0xFF; // Red channel
    out.g = (color >> 16) & 0xFF; // Green channel
    out.b = (color >> 8) & 0xFF;  // Blue channel
    out.a = color & 0xFF;         // Alpha channel

    return out;
}
} // namespace tinyrenderer

#endif
