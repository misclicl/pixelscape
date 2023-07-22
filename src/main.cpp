#include <cstdio>
#include <iostream>
#include <vector>

#include "raylib.h"
#include "raymath.h"

#include "model.h"

const int window_width = 512;
const int window_height = 512;

const int texture_size = 128;

Vector3 vertices[5] = {
    Vector3 { 40, 40, 1 },
    Vector3 { 80, 40, 1 },
    Vector3 { 40, 80, 1 },
    Vector3 { 90, 90, 1 },
    Vector3 { 75, 20, 1 },
};

const float far_clipping_plane = -std::numeric_limits<float>::max();
const float close_clipping_plane = 0;

inline void triangle_bb(const Vector3 *vertices, int *boundaries) {
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

void draw_triangle(
    const Vector3 *vertices,
    const Vector2 (&uv_coords)[3],
    Image &diffuse_texture,

    const float intencity, 
    float *zbuffer
) {
    int *boundaries = new int[4];
    triangle_bb(vertices, boundaries);

    float area = -edgeFunction(vertices[0], vertices[1], vertices[2]);
    Vector3 p;

    for (p.x = boundaries[0]; p.x <= boundaries[1]; p.x++) {
        for (p.y = boundaries[2]; p.y <= boundaries[3]; p.y++) {
            float u, v, w;
            barycentric_coords(vertices, Vector3 {(float)p.x, (float)p.y, 0}, u, v, w);

            if (w >= 0 && v >= 0 && u >= 0) {
                printf("uvw: %f, %f, %f \n", u, v, w);
                p.z = u * vertices[0].z + v * vertices[1].z + w * vertices[2].z;
                int index = (int)p.x + (int)p.y * texture_size;

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

                    // Color color = {
                    //     (unsigned char)((p.z + .5f) * 128),
                    //     (unsigned char)((p.z + .5f) * 128),
                    //     (unsigned char)((p.z + .5f) * 128),
                    //     255,
                    // };

                    DrawPixel(p.x, p.y, color);
                }
            }
        }
    }

    delete[] boundaries;
}

void render_with_shading(const tinyrenderer::Model *model, Vector3 &light_dir, Image diffuse_texture) {
    float *depth_buffer = new float[texture_size*texture_size];

    for (uint32_t i = 0; i < texture_size * texture_size; ++i) {
        depth_buffer[i] = far_clipping_plane;
    }

    for (uint32_t i = 0; i < model->n_faces(); ++i) {
        Vector3 world_coords[3];
        Vector2 uv_coords[3];
        Vector3 vertices[3];

        std::vector<int> face_vertex_indices = model->face_vertices(i);
        std::vector<int> uvs = model->face_uvs(i);

        int half_width = floor(texture_size/ 2);
        int half_height = floor(texture_size/ 2);

        for (int j = 0; j < 3; j++) {
            world_coords[j] = model->vert(face_vertex_indices[j]);
            uv_coords[j] = model->uv_coords(uvs[j]);

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

        if (intensity > 0) {
            draw_triangle(vertices, uv_coords, diffuse_texture, intensity, depth_buffer);
        }
    }

    delete[] depth_buffer;
}

void render_vertices(Vector3 *vertices, Image &diffuse_texture) {
    float *depth_buffer = new float[texture_size*texture_size];

    for (uint32_t i = 0; i < texture_size * texture_size; ++i) {
        depth_buffer[i] = far_clipping_plane;
    }

    Vector3 v0 = vertices[0];
    Vector3 v1 = vertices[1];
    Vector3 v2 = vertices[2];
    Vector3 v3 = vertices[3];
    Vector3 v4 = vertices[4];

    Vector3 t0[3] = {v0, v1, v2};
    Vector2 uv[3] = { {0, 0}, {0, 1}, {1, 1} };
    draw_triangle(t0, uv, diffuse_texture, 1, depth_buffer);

    delete[] depth_buffer;
}

int main(int argc, char **argv) {
    InitWindow(window_width, window_height, "tinyrenderer");
    SetTargetFPS(60);

    tinyrenderer::Model *model = new tinyrenderer::Model("assets/head.obj");
    // tinyrenderer::Model *model = new tinyrenderer::Model("assets/corner.obj");
    // tinyrenderer::Model *model = new tinyrenderer::Model("assets/teapot.obj");
    Image diffuse_img = LoadImage("assets/diffuse.png");
    // Image diffuse_img = LoadImage("assets/grid.png");

    Vector3 light_dir = {0, 0, -1};

    RenderTexture2D render_texture = LoadRenderTexture(texture_size, texture_size);

    BeginTextureMode(render_texture);
    ClearBackground(BLACK);
    EndTextureMode();

    while (!WindowShouldClose()) {
        BeginTextureMode(render_texture);

        render_with_shading(model, light_dir, diffuse_img);
        // render_vertices(vertices, diffuse_img);

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
    UnloadImage(diffuse_img);
    CloseWindow();

    delete model;

    return 0;
}
