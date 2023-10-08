#pragma once

#include <cstdlib>
#include <vector>

#include "raylib.h"

#include "color_buffer.h"
#include "depth_buffer.h"
#include "tiny_color.h"
#include "tiny_math.h"
#include "mesh.h"
#include "camera.h"
#include "shader.h"

#include "../examples/renderer.h"
#include "../light.h"

void draw_line(
    ColorBuffer *color_buffer,
    float x0, float y0,
    float x1, float y1,
    Color color);

void draw_rectangle(
    ColorBuffer *color_buffer,
    Vector2 position,
    int size_x,
    int size_y,
    uint32_t color);

void draw_triangle_wireframe(
    ColorBuffer *color_buffer,
    Vec4f *vertices,
    TinyColor color
);

void draw_triangle(
    CameraType camera_type,
    ColorBuffer *color_buffer,
    DepthBuffer *depth_buffer,
    TinyTriangle *triangle,
    Image *diffuse_texture,
    Light *light,
    RendererState *renderer_state
);

void depth_test(
    CameraType camera_type,
    ColorBuffer *color_buffer,
    DepthBuffer *depth_buffer,
    TinyTriangle *triangle,
    FragmentShader fragment_shader
);

void draw_axis(ColorBuffer *color_buffer);

