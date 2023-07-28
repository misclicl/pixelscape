#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include <cstdlib>
#include <vector>

#include "raylib.h"

#include "tiny_math.h"
#include "tiny_color.h"
#include "color_buffer.h"

namespace tinyrenderer {
void draw_line(ColorBuffer *color_buffer, Vector3 p0, Vector3 p1, uint32_t color);

void draw_rectangle(
    ColorBuffer *color_buffer,
    Vector2 position,
    int size_x,
    int size_y,
    uint32_t color
);

void draw_triangle_wireframe(
    ColorBuffer *color_buffer,
    Vector3 p0,
    Vector3 p1,
    Vector3 p2,
    uint32_t color
);

void draw_triangle_wireframe(
    ColorBuffer *color_buffer,
    Vec3f *vertices,
    TinyColor color
);

void draw_triangle(
    ColorBuffer *color_buffer,
    const Vector3 *vertices,
    const Vector2 (&uv_coords)[3],
    Image &diffuse_texture,
    const float intencity, 
    float *zbuffer
);

void draw_triangle(
    ColorBuffer *color_buffer,
    const Vec3f *vertices,
    // const Vector2 (&uv_coords)[3],
    // Image &diffuse_texture,
    // const float intencity, 
    float *zbuffer
);

void draw_axis(ColorBuffer *color_buffer);

void draw_triangles(
    ColorBuffer *color_buffer,
    std::vector<Vector3> &vertices, 
    std::vector<std::array<int, 3>> &t_indices, 
    uint32_t color
);
}
#endif 
