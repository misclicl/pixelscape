#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include <cstdlib>
#include <vector>

#include "raylib.h"
#include "color_buffer.h"

namespace tinyrenderer {
void draw_line(ColorBuffer *color_buffer, Vector3 p0, Vector3 p1, uint32_t color);

void draw_triangle_wireframe(Vector3 p0, Vector3 p1, Vector3 p2, Color color);

void draw_triangles(
    std::vector<Vector3> &vertices,
    std::vector<std::array<int, 3>> &t_indices, 
    int screen_width,
    int screen_heigth,
    Color color
);

void draw_triangle(
    const Vector3 *vertices,
    const Vector2 (&uv_coords)[3],
    Image &diffuse_texture,
    const float intencity, 
    float *zbuffer,

    int screen_width,
    int screen_height
);

void draw_rectangle(
    ColorBuffer *color_buffer,
    Vector2 position,
    int size_x,
    int size_y,
    uint32_t color
);

void draw_axis(ColorBuffer *color_buffer);
}
#endif 
