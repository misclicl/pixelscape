#pragma once

#include "tiny_math.h"
#include "matrix.h"
#include "mesh.h"
#include "shader.h"
#include "color_buffer.h"
#include "depth_buffer.h"
#include "camera.h"
#include "renderer.h"

void render_mesh(
    TinyMesh *mesh,
    ColorBuffer *color_buffer,
    DepthBuffer * depth_buffer,
    Matrix4 *mat_world,
    Vec3f camera_direction,
    VertextShader vertex_shader,
    FragmentShader fragment_shader,
    RenderFlagSet render_flags
);

void psgl_set_projection_type(CameraType type);
CameraType *psgl_get_projection_type();

