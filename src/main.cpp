#include <iostream>
#include <vector>

#include "raylib.h"
#include "raymath.h"

#include "model.h"

const int window_width = 512;
const int window_height = 512;
const int texture_size = 128;

std::pair<Vector2, Vector2> triangle_bb(const Vector3 *vertices) {
    float x_start = std::max(0.f, std::min(vertices[0].x, std::min(vertices[1].x, vertices[2].x)));
    float x_end = std::min(128.0f, std::max(vertices[0].x, std::max(vertices[1].x, vertices[2].x)));

    float y_start = std::max(0.f, std::min(vertices[0].y, std::min(vertices[1].y, vertices[2].y)));
    float y_end = std::min(128.f, std::max(vertices[0].y, std::max(vertices[1].y, vertices[2].y)));

    return {Vector2{x_start, y_start}, Vector2{x_end, y_end}};
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

void draw_triangle(const Vector3 *vertices, const Color color) {
    std::pair<Vector2, Vector2> bb = triangle_bb(vertices);
    Vector2 start = bb.first;
    Vector2 end = bb.second;

    for (int x = start.x; x <= end.x; ++x) {
        for (int y = start.y; y <= end.y; ++y) {
            float u, v, w;

            barycentric_coords(vertices, Vector3 {(float)x, (float)y, 0}, u, v, w);

            if (w >= 0 && v >= 0 && u >= 0) {
                DrawPixel(x, y, color);
            }
        }
    }
}


void render_with_shading(const tinyrenderer::Model *model, Vector3 &light_dir) {
    for (int i = 0; i < model->nfaces(); i++) {
        Vector2 screen_coords[3];
        Vector3 world_coords[3];
        Vector3 vertices[3];

        std::vector<int> face = model->face(i);

        int half_width = floor(128 / 2);
        int half_height = floor(128 / 2);

        for (int j = 0; j < 3; j++) {
            world_coords[j] = model->vert(face[j]);
        }

        for (int j = 0; j < 3; j++) {
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

        float intensity = std::min(1.f, Vector3DotProduct(triangle_normal, light_dir));

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
            draw_triangle(vertices, color);
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
