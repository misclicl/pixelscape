#include <array>
#include <cstdlib>
#include <vector>

#include "color_buffer.h"
#include "display.h"
#include "raylib.h"
#include "raymath.h"
#include "tiny_color.h"
#include "tiny_math.h"

void draw_line(
    ColorBuffer *color_buffer,
    float x0, float y0,
    float x1, float y1,
    TinyColor color) {
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
    Vec4f *vertices,
    TinyColor color) {
    draw_line(color_buffer, vertices[0].x, vertices[0].y, vertices[1].x, vertices[1].y, color);
    draw_line(color_buffer, vertices[1].x, vertices[1].y, vertices[2].x, vertices[2].y, color);
    draw_line(color_buffer, vertices[2].x, vertices[2].y, vertices[0].x, vertices[0].y, color);
}

static void triangle_bb(Vec2f vertices[3], int boundaries[4], int screen_width, int screen_height) {
    int x_start = floor(std::min(vertices[0].x, std::min(vertices[1].x, vertices[2].x)));
    int x_end = ceil(std::max(vertices[0].x, std::max(vertices[1].x, vertices[2].x)));

    int y_start = floor(std::min(vertices[0].y, std::min(vertices[1].y, vertices[2].y)));
    int y_end = ceil(std::max(vertices[0].y, std::max(vertices[1].y, vertices[2].y)));

    boundaries[0] = std::max(0, x_start);
    boundaries[1] = std::min(screen_width, x_end);
    boundaries[2] = std::max(0, y_start);
    boundaries[3] = std::min(screen_height, y_end);
}

// static void barycentric_coords(Vec2<int> vertices[3], Vec2<int> p, float *alpha, float *beta, float *gamma) {
static void barycentric_coords(Vec2f vertices[3], Vec2f p, float *alpha, float *beta, float *gamma) {
    // Formula: P = A + w1 * (B - A) + w2 * (C - A)
    //          P = A + u * vAB + vAC
    Vec2f vAB = vertices[1] - vertices[0];
    Vec2f vAC = vertices[2] - vertices[0];
    Vec2f vAP = p - vertices[0];

    float area_abc = vAB.x * vAC.y - vAC.x * vAB.y;

    *beta =  (float)(vAP.x * vAC.y - vAC.x * vAP.y) / area_abc;
    *gamma = (float)(vAB.x * vAP.y - vAP.x * vAB.y) / area_abc;
    *alpha = 1.0f - *beta - *gamma;
}

inline float apply_barycentric(float a, float b, float c, float alpha, float beta, float gamma) {
    return alpha * a + beta * b + gamma * c;
}

static void draw_triangle_pixel() {
}

static TinyColor sample_color_from_texture(
    Color* texture_data,
    Image *texture,
    int u, int v,
    RendererState *renderer_state
) {
     size_t uv_idx = (v * texture->height) + u;
     auto color = GetPixelColor(texture_data + uv_idx, texture->format);

     auto texture_filter_mode = renderer_state == nullptr ?
        NEAREST_NEIGHBOR : renderer_state->texture_filter_mode;

     switch (texture_filter_mode) {
        case TextureFilterMode::NEAREST_NEIGHBOR:
            return ColorToInt(color);
        case TextureFilterMode::BILINEAR: {
            uv_idx = (v * texture->height) + u + 1;
            auto color_right = GetPixelColor(texture_data + uv_idx, texture->format);
            uv_idx = (v * texture->height) + u - 1;
            auto color_left = GetPixelColor(texture_data + uv_idx, texture->format);
            uv_idx = ((v - 1) * texture->height) + u;
            auto color_up = GetPixelColor(texture_data + uv_idx, texture->format);
            uv_idx = ((v + 1) * texture->height) + u;
            auto color_down = GetPixelColor(texture_data + uv_idx, texture->format);

            return tiny_color_from_rgb(
                (color.r + color_left.r + color_right.r + color_down.r + color_up.r) / 5,
                (color.g + color_left.g + color_right.g + color_down.g + color_up.g) / 5,
                (color.b + color_left.b + color_right.b + color_down.b + color_up.b) / 5
            );
        }
        default:
            return tiny_color_from_rgb(0, 0, 0);
    }
}

inline static Vec3f calculate_fragment_normal(
    TinyTriangle * triangle,
    float w_inverse0,
    float w_inverse1,
    float w_inverse2,
    float w_inverse,
    float alpha,
    float beta,
    float gamma
) {
    Vec3f fragment_normal = {};
    fragment_normal.x = apply_barycentric(
        triangle->vertices[0].normal.x * w_inverse0,
        triangle->vertices[1].normal.x * w_inverse1,
        triangle->vertices[2].normal.x * w_inverse2,
        alpha, beta, gamma);
    fragment_normal.y = apply_barycentric(
        triangle->vertices[0].normal.y * w_inverse0,
        triangle->vertices[1].normal.y * w_inverse1,
        triangle->vertices[2].normal.y * w_inverse2,
        alpha, beta, gamma);
    fragment_normal.z = apply_barycentric(
        triangle->vertices[0].normal.z * w_inverse0,
        triangle->vertices[1].normal.z * w_inverse1,
        triangle->vertices[2].normal.z * w_inverse2,
        alpha, beta, gamma);

    fragment_normal.x /= w_inverse;
    fragment_normal.y /= w_inverse;
    fragment_normal.z /= w_inverse;

    return fragment_normal;
}

void draw_triangle(
    ColorBuffer *color_buffer,
    float *depth_buffer,
    TinyTriangle *triangle,
    Image *diffuse_texture,
    Light *light,
    RendererState *renderer_state
) {
    int screen_width = color_buffer->width;
    int screen_height = color_buffer->height;

    int boundaries[4];

    Vec2f p;

    float u, v; // Stores interpolated values

    float alpha, beta, gamma;
    TinyColor color = 0xffffffff;
    Color *data = (Color *)diffuse_texture->data;

    Vec2f v_screen[3] = {
        {triangle->vertices[0].position.x, triangle->vertices[0].position.y},
        {triangle->vertices[1].position.x, triangle->vertices[1].position.y},
        {triangle->vertices[2].position.x, triangle->vertices[2].position.y},
    };

    triangle_bb(v_screen, boundaries, screen_width, screen_height);

    float w_inverse0 = 1 / triangle->vertices[0].position.w;
    float w_inverse1 = 1 / triangle->vertices[1].position.w;
    float w_inverse2 = 1 / triangle->vertices[2].position.w;
    float w_inverse_interpl; // Holds depth of a pixel

    float intensity = 1.0f;

    for (p.x = boundaries[0]; p.x <= boundaries[1]; p.x++) {
        for (p.y = boundaries[2]; p.y <= boundaries[3]; p.y++) {
            barycentric_coords(v_screen, {p.x, p.y}, &alpha, &beta, &gamma);

            if (alpha >= 0 && beta >= 0 && gamma >= 0) {
                w_inverse_interpl = apply_barycentric(
                    w_inverse0,
                    w_inverse1,
                    w_inverse2,
                    alpha, beta, gamma);

                if (diffuse_texture != nullptr) {
                    // UV Interpolation of u/w and v/w
                    u = apply_barycentric(
                        triangle->vertices[0].texcoords.x * w_inverse0,
                        triangle->vertices[1].texcoords.x * w_inverse1,
                        triangle->vertices[2].texcoords.x * w_inverse2,
                        alpha, beta, gamma);

                    v = apply_barycentric(
                        triangle->vertices[0].texcoords.y * w_inverse0,
                        triangle->vertices[1].texcoords.y * w_inverse1,
                        triangle->vertices[2].texcoords.y * w_inverse2,
                        alpha, beta, gamma);

                    u /= w_inverse_interpl;
                    v /= w_inverse_interpl;

                    // in-texture coords
                    u = floor(u * (diffuse_texture->width - 1));
                    v = floor(v * (diffuse_texture->height - 1));

                    color = sample_color_from_texture(data, diffuse_texture, u, v, renderer_state);
                }

                Vec3f fragment_normal = calculate_fragment_normal(
                    triangle,
                    w_inverse0, w_inverse1, w_inverse2,
                    w_inverse_interpl,
                    alpha, beta, gamma
                );

                float alignment = -Vec3f::dot(light->direction.normalize(), fragment_normal);
                alignment = std::max(alignment, 0.f);
                float intensity = renderer_state->flags[USE_SHADING] ?
                    alignment:
                    1.f;
                color = apply_intensity(color, {255, 255, 255, 255}, intensity);


                int depth_buffer_idx = static_cast<int>(p.x + color_buffer->width * p.y);

                bool use_z_buffer_check = renderer_state == nullptr ? false : renderer_state->flags[USE_Z_BUFFER];

                if (w_inverse_interpl < depth_buffer[depth_buffer_idx] && use_z_buffer_check) {
                    continue;
                }

                auto depth_buffer_color = tiny_color_from_rgb(
                    -w_inverse_interpl * 255,
                    -w_inverse_interpl * 255,
                    -w_inverse_interpl * 255
                );

                // normals_color
                // color = tiny_color_from_rgb(
                //     (1 + normal.x) * 128,
                //     (1 + normal.y) * 128,
                //     (1 + normal.z) * 128
                // );

                depth_buffer[depth_buffer_idx] = w_inverse_interpl;
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
    /*
     *      ^ y
     *      |
     *      +------------------+
     *      |                  |
     *      |                  |
     *      |---+ (x, y)       |
     *      |   |              |   x
     *      +------------------+--->
     */

    int unit = 16;
    float margin = 10.f;

    float axis_length = unit;
    Vec3f origin = {
        (float)color_buffer->width - margin - unit,
        margin,
        1};

    draw_line(color_buffer, origin.x, origin.y, origin.x, origin.y + axis_length, 0x00FF00FF);
    draw_line(color_buffer, origin.x, origin.y, origin.x + axis_length, origin.y, 0xFF0000FF);
}
