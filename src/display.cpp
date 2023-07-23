#include <cstdlib>
#include <vector>
#include <array>

#include "raylib.h"
#include "display.h"
#include "raymath.h"

void tinyrenderer::draw_line(Vector3 p0, Vector3 p1, Color color) {
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

    float y_step = std::abs(dy/float(dx));
    float y_overflow = 0; 
    int y = y0; 

    for (int x = x0; x <= x1; x++) {
        if (isSteep) {
            DrawPixel(y, x, color);
        } else {
            DrawPixel(x, y, color);
        }

        y_overflow += y_step;

        // When yOverflow exceeds 0.5, we've accumulated enough
        // fractional yStep to adjust the y coordinate.
        if (y_overflow > .5) {
            y += (y1 > y0 ? 1 : -1);
            y_overflow -= 1.;
        }
    }
}

void tinyrenderer::draw_triangle_wireframe(Vector3 p0, Vector3 p1, Vector3 p2, Color color) {
    draw_line(p0, p1, color);
    draw_line(p1, p2, color);
    draw_line(p2, p0, color);
}

Vector3 to_screen_coord(Vector3 &v, int screen_width, int screen_height) {
    return Vector3 {
        v.x + (float)screen_width/ 2,
        v.y + (float)screen_height / 2,
        v.z
    };
}

void tinyrenderer::draw_triangles(
    std::vector<Vector3> &vertices, 
    std::vector<std::array<int, 3>> &t_indices, 
    int screen_width,
    int screen_height,
    Color color
) {
    for (std::array<int, 3> t : t_indices) {
        Vector3 v1 = vertices[t[0]];
        Vector3 v2 = vertices[t[1]];
        Vector3 v3 = vertices[t[2]];

        tinyrenderer::draw_triangle_wireframe(
            to_screen_coord(v1, screen_width, screen_height), 
            to_screen_coord(v2, screen_width, screen_height),
            to_screen_coord(v3, screen_width, screen_height),
            color
        );
    }
}

// TODO: hold dimensions in an instance
inline void triangle_bb(const Vector3 *vertices, int *boundaries, int screen_width, int screen_height) {
    int x_start = floor(std::min(vertices[0].x, std::min(vertices[1].x, vertices[2].x)));
    int x_end = ceil(std::max(vertices[0].x, std::max(vertices[1].x, vertices[2].x)));

    int y_start = floor(std::min(vertices[0].y, std::min(vertices[1].y, vertices[2].y)));
    int y_end = ceil (std::max(vertices[0].y, std::max(vertices[1].y, vertices[2].y)));

    boundaries[0] = std::max(0, x_start);
    boundaries[1] = std::min(screen_width, x_end);
    boundaries[2] = std::max(0, y_start);
    boundaries[3] = std::min(screen_height, y_end);
}

void barycentric_coords(Vector3 const *vertices, Vector3 const p, float &u, float &v, float&w) {
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

float edgeFunction(const Vector3 &a, const Vector3 &b, const Vector3 &c) {
    return (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x);
}

void tinyrenderer::draw_triangle(
    const Vector3 *vertices,
    const Vector2 (&uv_coords)[3],
    Image &diffuse_texture,
    const float intencity, 
    float *zbuffer,

    int screen_width,
    int screen_height
) {
    int *boundaries = new int[4];
    triangle_bb(vertices, boundaries, screen_width, screen_height);

    float area = -edgeFunction(vertices[0], vertices[1], vertices[2]);
    Vector3 p;

    for (p.x = boundaries[0]; p.x <= boundaries[1]; p.x++) {
        for (p.y = boundaries[2]; p.y <= boundaries[3]; p.y++) {
            float u, v, w;
            barycentric_coords(vertices, Vector3 {(float)p.x, (float)p.y, 0}, u, v, w);

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

                    Color* data = (Color*)diffuse_texture.data;
                    Color color = GetPixelColor(data + index_uv, diffuse_texture.format);
                    color = {
                        (unsigned char)(color.r * intencity),
                        (unsigned char)(color.g * intencity),
                        (unsigned char)(color.b * intencity),
                        color.a
                    };

                    // ######## UV ENDS ##########

                    zbuffer[index] = p.z;

                    color = {
                        (unsigned char)((p.z + .5f) * 128),
                        (unsigned char)((p.z + .5f) * 128),
                        (unsigned char)((p.z + .5f) * 128),
                        255,
                    };

                    DrawPixel(p.x, p.y, color);
                }
            }
        }
    }

    delete[] boundaries;
}

