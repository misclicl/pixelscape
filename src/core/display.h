#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include <cstdlib>
#include <vector>

#include "raylib.h"

#include "color_buffer.h"
#include "tiny_color.h"
#include "tiny_math.h"

namespace tinyrenderer {
void draw_line(ColorBuffer *color_buffer, Vec3f *p0, Vec3f *p1, uint32_t color);

void draw_rectangle(
    ColorBuffer *color_buffer,
    Vector2 position,
    int size_x,
    int size_y,
    uint32_t color);

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
    Vec3f *vertices,
    TinyColor face_color
    // float *zbuffer
);

void draw_axis(ColorBuffer *color_buffer);
} // namespace tinyrenderer
#endif
