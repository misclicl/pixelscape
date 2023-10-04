#pragma once

struct FragmentData {
    float x, y;
    float depth;
    float u, v;  // Texture coordinates
};

typedef float (*FragmentShader)(void* data);
