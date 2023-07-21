#include <iostream>
#include <vector>

#include "raylib.h"
#include "raymath.h"

#include "model.h"

const int window_width = 512;
const int window_height = 512;

const int texture_size = 128;

const float far_clipping_plane = -std::numeric_limits<float>::max();
const float close_clipping_plane = 0;

void triangle_bb(const Vector3 *vertices, int *boundaries) {
    int x_start = floor(std::min(vertices[0].x, std::min(vertices[1].x, vertices[2].x)));
    int x_end = ceil(std::max(vertices[0].x, std::max(vertices[1].x, vertices[2].x)));

    int y_start = floor(std::min(vertices[0].y, std::min(vertices[1].y, vertices[2].y)));
    int y_end = ceil (std::max(vertices[0].y, std::max(vertices[1].y, vertices[2].y)));

    boundaries[0] = std::max(0, x_start);
    boundaries[1] = std::min(texture_size, x_end);
    boundaries[2] = std::max(0, y_start);
    boundaries[3] = std::min(texture_size, y_end);
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

void draw_triangle(const Vector3 *vertices, const Color color, float *zbuffer) {
    int *boundaries = new int[4];
    triangle_bb(vertices, boundaries);

    Vector3 p;

    for (p.x = boundaries[0]; p.x <= boundaries[1]; p.x++) {
        for (p.y = boundaries[2]; p.y <= boundaries[3]; p.y++) {
            float u, v, w;
            barycentric_coords(vertices, Vector3 {(float)p.x, (float)p.y, 0}, u, v, w);

            float epsilon = 0;
            if (w >= epsilon && v >= epsilon && u >= epsilon) {
                p.z = u * vertices[0].z + v * vertices[1].z + w * vertices[2].z;
                int index = (int)p.x + (int)p.y * texture_size;

                if (zbuffer[index] < p.z) {
                    zbuffer[index] = p.z;
                    Color color = {
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
}

void render_with_shading(const tinyrenderer::Model *model, Vector3 &light_dir) {
    float *depth_buffer = new float[texture_size*texture_size];
    for (uint32_t i = 0; i < texture_size * texture_size; ++i) {
        depth_buffer[i] = far_clipping_plane;
    }

    for (uint32_t i = 0; i < model->nfaces(); ++i) {
        Vector2 screen_coords[3];
        Vector3 world_coords[3];
        Vector3 vertices[3];

        std::vector<int> face = model->face(i);

        int half_width = floor(texture_size/ 2);
        int half_height = floor(texture_size/ 2);

        for (int j = 0; j < 3; j++) {
            world_coords[j] = model->vert(face[j]);

            vertices[j].x = (world_coords[j].x + 1.) * (half_width);
            vertices[j].y = (world_coords[j].y + 1.) * (half_height);
            vertices[j].z = world_coords[j].z;
        }

        Vector3 triangle_normal = Vector3Normalize(
            Vector3CrossProduct(
                Vector3Subtract(world_coords[2], world_coords[0]),
                Vector3Subtract(world_coords[1], world_coords[0])
            )
        );

        float intensity = std::min(100.f, Vector3DotProduct(triangle_normal, light_dir));

        Color color = Color{
            (unsigned char)(intensity * (255)),
            (unsigned char)(intensity * (255)),
            (unsigned char)(intensity * (255)),
            // (unsigned char)(intensity * (rand()%255)),
            // (unsigned char)(intensity * (rand()%255)),
            // (unsigned char)(intensity * (rand()%255)),
            255
        };

        if (intensity > 0) {
            draw_triangle(vertices, color, depth_buffer);
        }
    }
}

int main(int argc, char **argv) {
    InitWindow(window_width, window_height, "tinyrenderer");
    SetTargetFPS(60);

    tinyrenderer::Model *model = new tinyrenderer::Model("assets/head.obj");

    Vector3 light_dir = {0, 0, -1};

    RenderTexture2D render_texture = LoadRenderTexture(texture_size, texture_size);

    BeginTextureMode(render_texture);
    ClearBackground(BLACK);
    EndTextureMode();

    while (!WindowShouldClose()) {
        BeginTextureMode(render_texture);

        render_with_shading(model, light_dir);

        EndTextureMode();

        BeginDrawing();

        ClearBackground(WHITE);
        DrawTexturePro(
            render_texture.texture,
            Rectangle{ 0, 0, static_cast<float>(render_texture.texture.width), static_cast<float>(render_texture.texture.height) },
            Rectangle{ 0, 0, static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight()) },
            Vector2{ 0, 0 },
            0,
            WHITE
        );

        EndDrawing();
    }

    UnloadRenderTexture(render_texture);
    CloseWindow();

    delete model;

    return 0;
}
