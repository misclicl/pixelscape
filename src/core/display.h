#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include <cstdlib>
#include <vector>

#include "raylib.h"

#include "color_buffer.h"
#include "tiny_color.h"
#include "tiny_math.h"

enum TextureFiltering {
    NEAREST_NEIGHBOR,
    BILINEAR,
};

void draw_line(
    ColorBuffer *color_buffer,
    float x0, float y0,
    float x1, float y1,
    uint32_t color);

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
    ColorBuffer *color_buffer,
    float *depth_buffer,
    Vec4f vertices[3],
    Vec2f texcoords[3],
    TinyColor face_color,
    Image *diffuse_texture,
    float intensity,
    bool z_buffer_check
);

void draw_axis(ColorBuffer *color_buffer);

extern TextureFiltering ps_texture_filtering_mode;
void set_texture_filtering(TextureFiltering tf);
#endif
