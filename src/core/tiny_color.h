#ifndef __TINY_COLOR__
#define __TINY_COLOR__

#include "math.h"
#include <stdlib.h>
#include "raylib.h"

#include <cstdint>

typedef uint32_t TinyColor;

static int min(int a, int b) {
    return a < b ? a : b;
}

inline Color make_raylib(TinyColor color) {
    Color out;

    out.r = (color >> 24) & 0xFF; // Red channel
    out.g = (color >> 16) & 0xFF; // Green channel
    out.b = (color >> 8) & 0xFF;  // Blue channel
    out.a = color & 0xFF;         // Alpha channel

    return out;
}

static void extract_rgb_from_color(TinyColor color, uint8_t *r, uint8_t *g, uint8_t *b){
    *r = (color >> 24) & 0xFF;
    *g = (color >> 16) & 0xFF;
    *b = (color >> 8)  & 0xFF;
}

static TinyColor apply_intensity(TinyColor color, Color light_color, float intensity) {
    // Extract individual color components
    uint8_t r = (color >> 24) & 0xFF;
    uint8_t g = (color >> 16) & 0xFF;
    uint8_t b = (color >> 8)  & 0xFF;
    uint8_t a =  color        & 0xFF;

    r = (uint8_t) fminf(r * intensity, 255.0f);
    g = (uint8_t) fminf(g * intensity, 255.0f);
    b = (uint8_t) fminf(b * intensity, 255.0f);

    // Combine the color channels back together
    return (r << 24) | (g << 16) | (b << 8) | a;
}

static TinyColor tiny_color_from_rgb(uint8_t r, uint8_t g, uint8_t b) {
    return (r << 24) | (g << 16) | (b << 8) | 255;
}

static TinyColor add_colors(TinyColor color_a, TinyColor color_b) {
    uint8_t ra, ga, ba;
    uint8_t rb, gb, bb;

    extract_rgb_from_color(color_a, &ra, &ga, &ba);
    extract_rgb_from_color(color_b, &rb, &gb, &bb);

    uint8_t r = min(ra + rb, 255);
    uint8_t g = min(ga + gb, 255);
    uint8_t b = min(ba + bb, 255);
    uint8_t a = 255;

    return (r << 24) | (g << 16) | (b << 8) | a;
}

#endif
