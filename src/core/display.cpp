#include <algorithm>
#include <array>
#include <cstdio>
#include <cstdlib>
#include <vector>

#include "color_buffer.h"
#include "display.h"
#include "raylib.h"
#include "raymath.h"
#include "tiny_math.h"

const Color default_color = DARKGRAY;

void draw_line(
    ColorBuffer *color_buffer,
    float x0, float y0,
    float x1, float y1,
    Color color
) {
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
    Color color
) {
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
    boundaries[1] = std::min(screen_width - 1, x_end);
    boundaries[2] = std::max(0, y_start);
    boundaries[3] = std::min(screen_height - 1, y_end);
}

inline static void barycentric_coords(Vec2f vertices[3], Vec2f p, float *alpha, float *beta, float *gamma) {
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

inline static float apply_barycentric(float a, float b, float c, float alpha, float beta, float gamma) {
    return alpha * a + beta * b + gamma * c;
}

static Color sample_color_from_texture(
    Color* texture_data,
    Image *texture,
    int u, int v,
    RendererState *renderer_state
) {
     size_t uv_idx = (v * texture->width) + u;
     auto color = GetPixelColor(texture_data + uv_idx, texture->format);

     auto texture_filter_mode = renderer_state == nullptr ?
        NEAREST_NEIGHBOR : renderer_state->texture_filter_mode;

     switch (texture_filter_mode) {
        case TextureFilterMode::NEAREST_NEIGHBOR:
            // return ColorToInt(color);
            return color;
        case TextureFilterMode::BILINEAR: {
            // uv_idx = (v * texture->height) + u + 1;
            // auto color_right = GetPixelColor(texture_data + uv_idx, texture->format);
            // uv_idx = (v * texture->height) + u - 1;
            // auto color_left = GetPixelColor(texture_data + uv_idx, texture->format);
            // uv_idx = ((v - 1) * texture->height) + u;
            // auto color_up = GetPixelColor(texture_data + uv_idx, texture->format);
            // uv_idx = ((v + 1) * texture->height) + u;
            // auto color_down = GetPixelColor(texture_data + uv_idx, texture->format);

            // return tiny_color_from_rgb(
            //     (color.r + color_left.r + color_right.r + color_down.r + color_up.r) / 5,
            //     (color.g + color_left.g + color_right.g + color_down.g + color_up.g) / 5,
            //     (color.b + color_left.b + color_right.b + color_down.b + color_up.b) / 5
            // );
            // TODO: fix boundary checks
            // return ColorToInt(color);
            return color;
        }
        default:
            // return tiny_color_from_rgb(0, 0, 0);
            return BLACK;
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

void depth_test(
    CameraType camera_type,
    ColorBuffer *color_buffer,
    DepthBuffer *depth_buffer,
    TinyTriangle *triangle,
    TinyTriangle *triangle_original,
    FragmentShader fragment_shader
) {
    int target_width = depth_buffer->width;
    int target_height = depth_buffer->height;

    int boundaries[4];
    Vec2f v_screen[3] = {
        {triangle->vertices[0].position.x, triangle->vertices[0].position.y},
        {triangle->vertices[1].position.x, triangle->vertices[1].position.y},
        {triangle->vertices[2].position.x, triangle->vertices[2].position.y},
    };
    triangle_bb(v_screen, boundaries, target_width, target_height);

    Vec2f p; // Current pixel
    float alpha, beta, gamma;

    float w_inverse0 = 1 / triangle->vertices[0].position.w;
    float w_inverse1 = 1 / triangle->vertices[1].position.w;
    float w_inverse2 = 1 / triangle->vertices[2].position.w;

    float depth_value;
    float intensity = 1.0f;

    for (p.x = boundaries[0]; p.x <= boundaries[1]; p.x++) {
        for (p.y = boundaries[2]; p.y <= boundaries[3]; p.y++) {
            barycentric_coords(v_screen, {p.x, p.y}, &alpha, &beta, &gamma);

            if (alpha >= 0 && beta >= 0 && gamma >= 0) {
                auto w_inverse_interpl = apply_barycentric(
                    w_inverse0,
                    w_inverse1,
                    w_inverse2,
                    alpha, beta, gamma);

                switch (camera_type) {
                    case CameraType::ORTHOGRAPHIC:
                        depth_value = apply_barycentric(
                            triangle->vertices[0].position.z * w_inverse0,
                            triangle->vertices[1].position.z * w_inverse1,
                            triangle->vertices[2].position.z * w_inverse2,
                            alpha, beta, gamma
                        );

                        depth_value /= w_inverse_interpl;
                        break;
                    case CameraType::PERSPECTIVE:
                        depth_value = w_inverse_interpl;
                        break;
                }

                auto depth_buffer_value = *buffer_pixel_get(depth_buffer, p.x, p.y);

                if (depth_value < depth_buffer_value) {
                    continue;
                }

                // SECTION: Normals
                // Calculate fragment normal vertor
                Vec3f fragment_normal = calculate_fragment_normal(
                    triangle,
                    w_inverse0, w_inverse1, w_inverse2,
                    w_inverse_interpl,
                    alpha, beta, gamma
                );
                // SECTION_END

                auto x = apply_barycentric(
                    triangle_original->vertices[0].position.x * w_inverse0,
                    triangle_original->vertices[1].position.x * w_inverse1,
                    triangle_original->vertices[2].position.x * w_inverse2,
                    alpha, beta, gamma
                );

                auto y = apply_barycentric(
                    triangle_original->vertices[0].position.y * w_inverse0,
                    triangle_original->vertices[1].position.y * w_inverse1,
                    triangle_original->vertices[2].position.y * w_inverse2,
                    alpha, beta, gamma
                );
                auto z = apply_barycentric(
                    triangle_original->vertices[0].position.z * w_inverse0,
                    triangle_original->vertices[1].position.z * w_inverse1,
                    triangle_original->vertices[2].position.z * w_inverse2,
                    alpha, beta, gamma
                );
                x /= w_inverse_interpl;
                y /= w_inverse_interpl;
                z /= w_inverse_interpl;

                if (fragment_shader) {
                    FragmentData shader_data = {
                        .x = x,
                        .y = y,
                        .z = z,
                        .depth = depth_value,
                        .normal = fragment_normal
                    };

                    // This is nonsence. The only reason why I pass on struct twice
                    // is that I don't know to create generic pointers yet, if such
                    // thing exists. If not, well I need to find a way to pass
                    // uniforms as a second argument here
                    auto result = fragment_shader(&shader_data, &shader_data);

                    // color_buffer->set_pixel(p.x, p.y, Color { .r = (unsigned char)(z * 255), .a = 255 });
                    color_buffer->set_pixel(p.x, p.y, result);
                }

                depth_buffer_set(depth_buffer, p.x, p.y, depth_value);
            }
        }
    }
}


// TODO: remove
inline float clamp(float val, float min_val, float max_val) {
    if (val < min_val) return min_val;
    if (val > max_val) return max_val;
    return val;
}

void draw_rectangle(
    ColorBuffer *color_buffer,
    Vector2 position,
    int size_x,
    int size_y,
    Color color
) {
    int x_start = std::max(0, (int)position.x);
    int x_end = std::min(color_buffer->height, (int)position.x + size_x);

    int y_start = std::max(0, (int)position.y);
    int y_end = std::min(color_buffer->height, (int)position.y + size_y);

    for (int x = x_start; x < x_end; x++) {
        for (int y = y_start; y < y_end; y++) {
            // TODO: put back
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

    // draw_line(color_buffer, origin.x, origin.y, origin.x, origin.y + axis_length, 0x00FF00FF);
    // draw_line(color_buffer, origin.x, origin.y, origin.x + axis_length, origin.y, 0xFF0000FF);
}
