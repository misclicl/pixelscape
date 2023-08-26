#pragma once

#include "./tiny_math.h"
#include "./matrix.h"
#include "./mesh.h"

typedef void (*VertexShader)(const TinyVertex *input_vertex, Vec3f *output_vertex, const Matrix4 *modelViewProj);
typedef void (*FragmentShader)(const TinyVertex *interpolated_vertex, Vec4f *output_color);

struct ShaderProgram {
    VertexShader vertex_shader;
    FragmentShader fragment_shader;
    // Add other shader stages as needed
    // Add any uniform data shared between shaders
};
