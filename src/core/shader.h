#pragma once

#include "raylib.h"
#include "tiny_math.h"
#include "tiny_color.h"

struct FragmentData {
    float depth;
    float u, v;  // Texture coordinates
    Vec3f normal;
};

typedef TinyColor (*FragmentShader)(void* data, void* uniforms);
