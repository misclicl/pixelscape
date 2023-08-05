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

static TinyColor apply_intencity(TinyColor color, float intencity) {
    uint32_t r = (color & 0xff000000) * intencity;
    uint32_t g = (color & 0x00ff0000) * intencity;
    uint32_t b = (color & 0x0000ff00) * intencity;
    uint32_t a = (color & 0x000000ff);

    return r & 0xff000000 | g & 0x00ff0000 | b & 0x0000ff00 | a;
}

static TinyColor tiny_color_from_rgb(uint8_t r, uint8_t g, uint8_t b) {
    return (r << 24) | (g << 16) | (b << 8) | 255;
}
} // namespace tinyrenderer


#endif
