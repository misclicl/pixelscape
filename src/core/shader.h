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

struct VertexShaderAttributes {
    Vec4f position;
    Vec3f normal;
    Vec2f tex_coords;
};

typedef Color (*VertextShader)(void* fs_in);
