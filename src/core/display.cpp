#include <array>
#include <cstdlib>
#include <vector>

#include "color_buffer.h"
#include "display.h"
#include "raylib.h"
#include "raymath.h"
#include "tiny_color.h"
#include "tiny_math.h"

namespace tinyrenderer {

void draw_line(
    ColorBuffer *color_buffer,
    Vec3f p0,
    Vec3f p1,
    TinyColor color) {
    int x0 = p0.x;
    int y0 = p0.y;
    int x1 = p1.x;
    int y1 = p1.y;

    bool isSteep = false;

    if (std::abs(x0 - x1) < std::abs(y0 - y1)) {
        isSteep = true;
        std::swap(x0, y0);
        std::swap(x1, y1);
    }

    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    int dx = x1 - x0;
    int dy = y1 - y0;

    float slope = std::abs(dy / float(dx));
    float y_overflow = 0;
    int y = y0;

    for (int x = x0; x <= x1; x++) {
        if (isSteep) {
            color_buffer->set_pixel(y, x, color);
        } else {
            color_buffer->set_pixel(x, y, color);
        }

        y_overflow += slope;

        // When yOverflow exceeds 0.5, we've accumulated enough
        // fractional yStep to adjust the y coordinate.
        if (y_overflow > .5) {
            y += (y1 > y0 ? 1 : -1);
            y_overflow -= 1.;
        }
    }
}

void draw_triangle_wireframe(
    ColorBuffer *color_buffer,
    Vec3f *vertices,
    TinyColor color) {
    draw_line(color_buffer, vertices[0], vertices[1], color);
    draw_line(color_buffer, vertices[1], vertices[2], color);
    draw_line(color_buffer, vertices[2], vertices[0], color);
}

// TODO: hold dimensions in an instance
inline void triangle_bb(const Vector3 *vertices, int *boundaries, int screen_width, int screen_height) {
    int x_start = floor(std::min(vertices[0].x, std::min(vertices[1].x, vertices[2].x)));
    int x_end = ceil(std::max(vertices[0].x, std::max(vertices[1].x, vertices[2].x)));

    int y_start = floor(std::min(vertices[0].y, std::min(vertices[1].y, vertices[2].y)));
    int y_end = ceil(std::max(vertices[0].y, std::max(vertices[1].y, vertices[2].y)));

    boundaries[0] = std::max(0, x_start);
    boundaries[1] = std::min(screen_width, x_end);
    boundaries[2] = std::max(0, y_start);
    boundaries[3] = std::min(screen_height, y_end);
}

static void triangle_bb(Vec3f *vertices, int *boundaries, int screen_width, int screen_height) {
    int x_start = floor(std::min(vertices[0].x, std::min(vertices[1].x, vertices[2].x)));
    int x_end = ceil(std::max(vertices[0].x, std::max(vertices[1].x, vertices[2].x)));

    int y_start = floor(std::min(vertices[0].y, std::min(vertices[1].y, vertices[2].y)));
    int y_end = ceil(std::max(vertices[0].y, std::max(vertices[1].y, vertices[2].y)));

    boundaries[0] = std::max(0, x_start);
    boundaries[1] = std::min(screen_width, x_end);
    boundaries[2] = std::max(0, y_start);
    boundaries[3] = std::min(screen_height, y_end);
}

static void barycentric_coords(Vector3 const *vertices, Vector3 const p, float &u, float &v, float &w) {
    // Formula: P = A + w1 * (B - A) + w2 * (C - A)
    //          P = A + u * vAB + vAC
    Vector3 vAB = Vector3Subtract(vertices[1], vertices[0]);
    Vector3 vAC = Vector3Subtract(vertices[2], vertices[0]);
    Vector3 vAP = Vector3Subtract(p, vertices[0]);

    float den = vAB.x * vAC.y - vAC.x * vAB.y;

    v = (vAP.x * vAC.y - vAC.x * vAP.y) / den;
    w = (vAB.x * vAP.y - vAP.x * vAB.y) / den;
    u = 1.0f - v - w;
}

static void barycentric_coords(Vec3f *vertices, Vec3f p, float &u, float &v, float &w) {
    // Formula: P = A + w1 * (B - A) + w2 * (C - A)
    //          P = A + u * vAB + vAC
    Vec3f vAB = vertices[1] - vertices[0];
    Vec3f vAC = vertices[2] - vertices[0];
    Vec3f vAP = p - vertices[0];

    float den = vAB.x * vAC.y - vAC.x * vAB.y;

    v = (vAP.x * vAC.y - vAC.x * vAP.y) / den;
    w = (vAB.x * vAP.y - vAP.x * vAB.y) / den;
    u = 1.0f - v - w;
}

void draw_triangle(
    ColorBuffer *color_buffer,
    const Vector3 *vertices,
    const Vector2 (&uv_coords)[3],
    Image &diffuse_texture,
    const float intencity,
    float *zbuffer) {
    int screen_width = color_buffer->width;
    int screen_height = color_buffer->height;

    int boundaries[4];
    triangle_bb(vertices, boundaries, screen_width, screen_height);

    Vector3 p;

    for (p.x = boundaries[0]; p.x <= boundaries[1]; p.x++) {
        for (p.y = boundaries[2]; p.y <= boundaries[3]; p.y++) {
            float u, v, w;
            barycentric_coords(vertices, Vector3{(float)p.x, (float)p.y, 0}, u, v, w);

            if (w >= 0 && v >= 0 && u >= 0) {
                p.z = u * vertices[0].z + v * vertices[1].z + w * vertices[2].z;
                int index = (int)p.x + (int)p.y * screen_width;

                if (zbuffer[index] < p.z) {
                    // ######## UV ##########

                    float alpha = u;
                    float beta = v;
                    float gamma = w;

                    float u = alpha * uv_coords[0].x + beta * uv_coords[1].x + gamma * uv_coords[2].x;
                    float v = alpha * uv_coords[0].y + beta * uv_coords[1].y + gamma * uv_coords[2].y;

                    u = floor(u * diffuse_texture.width);
                    v = diffuse_texture.height - floor(v * diffuse_texture.height);

                    int index_uv = (v * diffuse_texture.height) + u;

                    Color *data = (Color *)diffuse_texture.data;
                    Color color = GetPixelColor(data + index_uv, diffuse_texture.format);
                    color = {
                        (unsigned char)(color.r * intencity),
                        (unsigned char)(color.g * intencity),
                        (unsigned char)(color.b * intencity),
                        color.a};

                    // ######## UV ENDS ##########

                    zbuffer[index] = p.z;
                    color_buffer->set_pixel(p.x, p.y, ColorToInt(color));
                }
            }
        }
    }
}

void draw_triangle(
    ColorBuffer *color_buffer,
    Vec3f *vertices,
    TinyColor face_color
    ) {
    int screen_width = color_buffer->width;
    int screen_height = color_buffer->height;

    int boundaries[4];
    triangle_bb(vertices, boundaries, screen_width, screen_height);

    Vector3 p;

    for (p.x = boundaries[0]; p.x <= boundaries[1]; p.x++) {
        for (p.y = boundaries[2]; p.y <= boundaries[3]; p.y++) {
            float u, v, w;
            barycentric_coords(vertices, Vec3f{(float)p.x, (float)p.y, 0}, u, v, w);

            if (w >= 0 && v >= 0 && u >= 0) {
                p.z = u * vertices[0].z + v * vertices[1].z + w * vertices[2].z;
                int index = (int)p.x + (int)p.y * screen_width;

                color_buffer->set_pixel(p.x, p.y, face_color);
            }
        }
    }
}

void draw_rectangle(
    ColorBuffer *color_buffer,
    Vector2 position,
    int size_x,
    int size_y,
    TinyColor color) {
    int x_start = std::max(0, (int)position.x);
    int x_end = std::min(color_buffer->height, (int)position.x + size_x);

    int y_start = std::max(0, (int)position.y);
    int y_end = std::min(color_buffer->height, (int)position.y + size_y);

    for (int x = x_start; x < x_end; x++) {
        for (int y = y_start; y < y_end; y++) {
            color_buffer->set_pixel(x, y, color);
        }
    }
}

void draw_axis(ColorBuffer *color_buffer) {
    const int unit = 32;
    float half_height = (float)color_buffer->width / 2;
    float half_width = (float)color_buffer->height / 2;
    float axis_length = unit;
    Vec3f origin = {half_width, half_height, 1};

    draw_line(color_buffer, origin, {half_width, half_height + axis_length, 1}, 0x00FF00FF);
    draw_line(color_buffer, origin, {half_width + axis_length, half_height, 1}, 0xFF0000FF);
}
} // namespace tinyrenderer
