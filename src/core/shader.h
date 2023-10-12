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

struct VsOut {
    Vec3f normal;
    Vec2f tex_coords;
    Vec4f frag_pos;
    Vec4f frag_pos_light_space;
};

typedef VsOut (*VertextShader)(void* fs_in);
