#pragma once

#include "raylib.h"
#include "tiny_math.h"

struct FragmentData {
    float depth;
    float u, v;  // Texture coordinates
    Vec3f normal;
};

typedef Color (*FragmentShader)(void* data, void* uniforms);
