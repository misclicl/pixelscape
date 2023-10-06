#pragma once

#include "raylib.h"

struct FragmentData {
    float depth;
    float u, v;  // Texture coordinates
};

typedef Color (*FragmentShader)(void* data);
