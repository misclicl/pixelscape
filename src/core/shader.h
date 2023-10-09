#pragma once

#include "raylib.h"
#include "tiny_math.h"

struct FragmentData {
    float x, y, z;
    float depth;
    float u, v;
    Vec3f normal;
};

typedef Color (*FragmentShader)(void* data, void* uniforms);
