#include <array>
#include <cstdlib>
#include <vector>

#include "color_buffer.h"
#include "display.h"
#include "raylib.h"
#include "raymath.h"
#include "tiny_color.h"
#include "tiny_math.h"

namespace pixelscape {

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

static void triangle_bb(Vec2<int> vertices[3], int boundaries[4], int screen_width, int screen_height) {
    int x_start = floor(std::min(vertices[0].x, std::min(vertices[1].x, vertices[2].x)));
    int x_end = ceil(std::max(vertices[0].x, std::max(vertices[1].x, vertices[2].x)));

    int y_start = floor(std::min(vertices[0].y, std::min(vertices[1].y, vertices[2].y)));
    int y_end = ceil(std::max(vertices[0].y, std::max(vertices[1].y, vertices[2].y)));

    boundaries[0] = std::max(0, x_start);
    boundaries[1] = std::min(screen_width, x_end);
    boundaries[2] = std::max(0, y_start);
    boundaries[3] = std::min(screen_height, y_end);
}

static void barycentric_coords(Vec2<int> vertices[3], Vec2<int> p, float *alpha, float *beta, float *gamma) {
    // Formula: P = A + w1 * (B - A) + w2 * (C - A)
    //          P = A + u * vAB + vAC
    Vec2<int> vAB = vertices[1] - vertices[0];
    Vec2<int> vAC = vertices[2] - vertices[0];
    Vec2<int> vAP = p - vertices[0];

    float area_abc = vAB.x * vAC.y - vAC.x * vAB.y;

    *beta =  (float)(vAP.x * vAC.y - vAC.x * vAP.y) / area_abc;
    *gamma = (float)(vAB.x * vAP.y - vAP.x * vAB.y) / area_abc;
    *alpha = 1.0f - *beta - *gamma;
}

void draw_triangle(
    ColorBuffer *color_buffer,
    const Vector3 *vertices,
    const Vector2 (&uv_coords)[3],
    Image &diffuse_texture,
    const float intencity,
    float *zbuffer) {
    // int screen_width = color_buffer->width;
    // int screen_height = color_buffer->height;
    //
    // int boundaries[4];
    // triangle_bb(vertices, boundaries, screen_width, screen_height);
    //
    // Vector3 p;
    //
    // for (p.x = boundaries[0]; p.x <= boundaries[1]; p.x++) {
    //     for (p.y = boundaries[2]; p.y <= boundaries[3]; p.y++) {
    //         float u, v, w;
    //         barycentric_coords(vertices, Vector3{(float)p.x, (float)p.y, 0}, u, v, w);
    //
    //         if (w >= 0 && v >= 0 && u >= 0) {
    //             p.z = u * vertices[0].z + v * vertices[1].z + w * vertices[2].z;
    //             int index = (int)p.x + (int)p.y * screen_width;
    //
    //             if (zbuffer[index] < p.z) {
    //                 // ######## UV ##########
    //
    //                 float alpha = u;
    //                 float beta = v;
    //                 float gamma = w;
    //
    //                 float u = alpha * uv_coords[0].x + beta * uv_coords[1].x + gamma * uv_coords[2].x;
    //                 float v = alpha * uv_coords[0].y + beta * uv_coords[1].y + gamma * uv_coords[2].y;
    //
    //                 u = floor(u * diffuse_texture.width);
    //                 v = diffuse_texture.height - floor(v * diffuse_texture.height);
    //
    //                 int index_uv = (v * diffuse_texture.height) + u;
    //
    //                 Color *data = (Color *)diffuse_texture.data;
    //                 Color color = GetPixelColor(data + index_uv, diffuse_texture.format);
    //                 color = {
    //                     (unsigned char)(color.r * intencity),
    //                     (unsigned char)(color.g * intencity),
    //                     (unsigned char)(color.b * intencity),
    //                     color.a};
    //
    //                 // ######## UV ENDS ##########
    //
    //                 zbuffer[index] = p.z;
    //                 color_buffer->set_pixel(p.x, p.y, ColorToInt(color));
    //             }
    //         }
    //     }
    // }
}

inline float apply_barycentric(float a, float b, float c, float alpha, float beta, float gamma) {
    return alpha * a + beta * b + gamma * c;
}

void draw_triangle(
    ColorBuffer *color_buffer,
    Vec4f vertices[3],
    Vec2f uv_coords[3],
    TinyColor face_color,
    Image *diffuse_texture
) {
    int screen_width = color_buffer->width;
    int screen_height = color_buffer->height;

    int boundaries[4];

    Vec2<int> p;
    float u,v;
    int uv_idx;

    float alpha, beta, gamma;
    TinyColor color = 0xffffffff;
    Color *data = (Color *)diffuse_texture->data;

    Vec2<int> v_screen[3] = { 
        {static_cast<int>(vertices[0].x), static_cast<int>(vertices[0].y)},
        {static_cast<int>(vertices[1].x), static_cast<int>(vertices[1].y)},
        {static_cast<int>(vertices[2].x), static_cast<int>(vertices[2].y)} 
    };
    triangle_bb(v_screen, boundaries, screen_width, screen_height);

    for (p.x = boundaries[0]; p.x <= boundaries[1]; p.x++) {
        for (p.y = boundaries[2]; p.y <= boundaries[3]; p.y++) {
            // barycentric_coords(vertices, Vec2<int>{p.x, p.y, 0}, alpha, beta, gamma);
            barycentric_coords(v_screen, Vec2<int>{p.x, p.y}, &alpha, &beta, &gamma);

            if (alpha >= 0 && beta >= 0 && gamma >= 0) {
                if (diffuse_texture != nullptr) {
                    u = apply_barycentric(
                        uv_coords[0].x,
                        uv_coords[1].x,
                        uv_coords[2].x,
                        alpha, beta, gamma);

                    v = apply_barycentric(
                        uv_coords[0].y,
                        uv_coords[1].y,
                        uv_coords[2].y,
                        alpha, beta, gamma);

                    // TODO: perspective adjustment
                    float z = 1;

                    u = floor(u * diffuse_texture->width) / z;
                    v = floor(v * diffuse_texture->height) / z;

                    uv_idx = (v * diffuse_texture->height) + u;
                    color = ColorToInt(GetPixelColor(data + uv_idx, diffuse_texture->format));
                }

                color_buffer->set_pixel(p.x, p.y, color);
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
    int unit = 16;
    float margin = 10.f;

    float axis_length = unit;
    Vec3f origin = {(float)color_buffer->width - margin, margin, 1};

    draw_line(color_buffer, origin, {origin.x, origin.y + axis_length, 1}, 0x00FF00FF);
    draw_line(color_buffer, origin, {origin.x - axis_length, origin.y, 1}, 0xFF0000FF);
}
} // namespace tinyrenderer
