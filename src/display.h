#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include <cstdlib>
#include <vector>

#include "raylib.h"

namespace tinyrenderer {
uint32_t *get_buffer_pixel(uint32_t *color_buffer, int x, int y, int width, int height);
uint32_t *get_buffer_pixel(uint32_t *color_buffer, int idx);

void draw_line(Vector3 p0, Vector3 p1, Color color);
void draw_line(uint32_t *color_buffer, Vector3 p0, Vector3 p1, uint32_t color, int width, int height);

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

void draw_axis(int width, int height);

// Apply given colour buffer to raylib's render texture;
void draw_color_buffer(uint32_t *color_buffer, int width, int height);

// void render_color_buffer(uint32_t *color_buffer, int width, int height);
void clear_color_buffer(uint32_t *color_buffer, int width, int height, uint32_t color);
}

#endif 
