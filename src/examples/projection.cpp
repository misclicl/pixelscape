#include <cstdio>
#include <stdlib.h>

#include "../core/tiny_color.h"
#include "../core/tiny_math.h"
#include "projection.h"
#include "raylib.h"

Vec3f camera_position = {0, 0, 2};
float fov_factor = 70;

void orthographic(ColorBuffer *color_buffer, Vec3f *vertices, size_t size) {
    TinyColor color = 0x9F9F9FFF;

    int half_width = color_buffer->width / 2;
    int half_height = color_buffer->height / 2;

    for (size_t i = 0; i < size; i++) {
        Vec3f *vertex = &vertices[i];
        Vec2f v_projected = {
            fov_factor * vertex->x + half_width,
            fov_factor * vertex->y + half_height};
        color_buffer->set_pixel(v_projected.x, v_projected.y, color);
    };
}

int map(float val) {
    return ((val + 1) * .5) * 255;
}

void perspective(ColorBuffer *color_buffer, Vec3f *vertices, size_t size, int delta) {
    TinyColor color = 0x9F9F9FFF;

    int half_width = color_buffer->width / 2;
    int half_height = color_buffer->height / 2;

    for (size_t i = 0; i < size; i++) {
        Vec3f *vertex = &vertices[i];

        Vec3 v_model = Vec3f::rotate_x(vertex, (float)delta / 120);

        Vec3f v_view = {
            v_model.x,
            v_model.y,
            v_model.z - camera_position.z};

        Vec2f v_projected = {
            fov_factor * v_view.x / v_view.z,
            fov_factor * v_view.y / v_view.z};

        Color color = {
            (unsigned char)map(vertex->x),
            (unsigned char)map(vertex->y),
            (unsigned char)map(vertex->z),
            255};

        Vec2f v_screen = {
            v_projected.x + half_width,
            v_projected.y + half_width,
        };

        color_buffer->set_pixel(v_screen.x, v_screen.y, ColorToInt(color));
    };
}

void Projection::run(ColorBuffer *color_buffer, int delta) {
    float inc = .2;
    // five element > 0, 5 < 0, 1 element == 0
    int size = 11;

    Vec3f *vertices = (Vec3f *)malloc(size * size * size * sizeof(Vec3f));
    int count = 0;

    for (float x = -1; x <= 1; x += inc) {
        for (float y = -1; y <= 1; y += inc) {
            for (float z = -1; z <= 1; z += inc) {
                Vec3f vertex = {
                    (float)x,
                    (float)y,
                    (float)z,
                };

                vertices[count] = vertex;
                count++;
            }
        }
    }

    // orthographic(color_buffer, vertices, size * size * size);
    perspective(color_buffer, vertices, size * size * size, delta);
    free(vertices);
}
