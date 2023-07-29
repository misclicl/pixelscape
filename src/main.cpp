#include <array>
#include <cstdio>
#include <iostream>
#include <vector>

#include "examples/background_grid.h"
#include "examples/draw_rectangles.h"
#include "examples/projection.h"
#include "examples/mesh_rendering.h"

#include "examples/linear_transformations.h"

#include "mesh.h"
#include "raylib.h"
#include "raymath.h"

#include "color_buffer.h"
#include "display.h"
#include "model.h"
// #include "loader_obj.h"

const int window_width = 512;
const int window_height = 512;

const int target_render_size_x = 256;
const int target_render_size_y = 256;

const int unit = 100;

const float cube_halfsize = .5f;

const float front = -cube_halfsize;
const float back = cube_halfsize;

const float far_clipping_plane = 10000;
const float near_clipping_plane = .1;

Vector4 Vector4Transform(const Vector4 v, const Matrix mat) {
    Vector4 result;
    result.x = mat.m0 * v.x + mat.m4 * v.y + mat.m8 * v.z + mat.m12 * v.w;
    result.y = mat.m1 * v.x + mat.m5 * v.y + mat.m9 * v.z + mat.m13 * v.w;
    result.z = mat.m2 * v.x + mat.m6 * v.y + mat.m10 * v.z + mat.m14 * v.w;
    result.w = mat.m3 * v.x + mat.m7 * v.y + mat.m11 * v.z + mat.m15 * v.w;
    return result;
}

void render_with_shading(
    tinyrenderer::ColorBuffer *color_buffer,
    const int &delta,
    const tinyrenderer::Model *model,
    Vector3 &light_dir,
    Image diffuse_texture) {
    float *depth_buffer = new float[target_render_size_x * target_render_size_y];

    for (uint32_t i = 0; i < target_render_size_x * target_render_size_y; ++i) {
        depth_buffer[i] = -far_clipping_plane;
    }

    Matrix rotation = MatrixRotateY((float)delta / 5 * DEG2RAD);
    Matrix scale = MatrixScale(1.f, 1.f, 1.f);
    Matrix model_to_world = MatrixMultiply(scale, rotation);

    for (uint32_t i = 0; i < model->n_faces(); ++i) {
        Vector3 world_coords[3];
        Vector2 uv_coords[3];
        Vector3 vertices[3];

        std::vector<int> face_vertex_indices = model->face_vertices(i);
        std::vector<int> uvs = model->face_uvs(i);

        int half_width = floor(target_render_size_x / 2);
        int half_height = floor(target_render_size_y / 2);

        for (int j = 0; j < 3; j++) {
            world_coords[j] = Vector3Transform(
                model->vert(face_vertex_indices[j]),
                model_to_world);
        }

        for (int j = 0; j < 3; j++) {
            uv_coords[j] = model->uv_coords(uvs[j]);

            vertices[j].x = (world_coords[j].x + 1.) * (half_width);
            vertices[j].y = (world_coords[j].y + 1.) * (half_height);
            vertices[j].z = world_coords[j].z;
        }

        Vector3 triangle_normal = Vector3Normalize(
            Vector3CrossProduct(
                Vector3Subtract(world_coords[2], world_coords[0]),
                Vector3Subtract(world_coords[1], world_coords[0])));

        float intensity = std::min(100.f, Vector3DotProduct(triangle_normal, light_dir));

        if (intensity > 0) {
            tinyrenderer::draw_triangle(
                color_buffer,
                vertices,
                uv_coords,
                diffuse_texture,
                intensity,
                depth_buffer);
        }
    }

    delete[] depth_buffer;
}

int main(int argc, char **argv) {
    InitWindow(window_width, window_height, "tinyrenderer");
    SetTargetFPS(60);
    int frame_counter = 0;

    tinyrenderer::Model *model = new tinyrenderer::Model("assets/head.obj");
    // tinyrenderer::Model *model = new tinyrenderer::Model("assets/cube.obj");

    // Image diffuse_img = LoadImage("assets/diffuse.png");
    Image diffuse_img = LoadImage("assets/grid.png");

    Vector3 light_dir = {0, 0, -1};

    RenderTexture2D render_texture = LoadRenderTexture(target_render_size_x, target_render_size_y);

    tinyrenderer::program::BackgroundGrid bg_grid;
    tinyrenderer::program::DrawRectangles draw_rectangles;
    tinyrenderer::program::Projection projection;
    tinyrenderer::MeshRendering::Program cube_rendering;
    cube_rendering.init();

    // std::vector<tinyrenderer::Vec3f> vs;
    // std::vector<tinyrenderer::TriangleFace> fs;
    // load_mesh(&vs, &fs);

    tinyrenderer::ColorBuffer color_buffer;
    color_buffer.width = target_render_size_x;
    color_buffer.height = target_render_size_y;
    color_buffer.pixels =
        (uint32_t *)malloc(target_render_size_x * target_render_size_y * sizeof(unsigned int));

    while (!WindowShouldClose()) {
        BeginTextureMode(render_texture);
        ClearBackground(BLACK);
    // printf("HELLLLLLLLLLLOOOOOOOOOO");

        color_buffer.clear(0x000000FF);

        // render_vertices(diffuse_img);
        // examples::shape_perspective();
        // bg_grid.run(&color_buffer);
        // draw_rectangles.run(&color_buffer);
        // projection.run(&color_buffer, frame_counter);
        // render_with_shading(&color_buffer, frame_counter, model, light_dir, diffuse_img);
        // tinyrenderer::draw_axis(&color_buffer);
        // tinyrenderer::linear_transformations(&color_buffer);
        // FIXME: rename delta, or change it to actual delta
        cube_rendering.run(&color_buffer, frame_counter);

        color_buffer.draw_to_texture();

        EndTextureMode();

        BeginDrawing();

        ClearBackground(WHITE);
        DrawTexturePro(
            render_texture.texture,
            Rectangle{0, 0, static_cast<float>(render_texture.texture.width), static_cast<float>(render_texture.texture.height)},
            Rectangle{0, 0, static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight())},
            Vector2{0, 0},
            0,
            WHITE);

        frame_counter++;
        EndDrawing();
    }

    UnloadRenderTexture(render_texture);
    UnloadImage(diffuse_img);
    CloseWindow();

    delete model;
    free(color_buffer.pixels);

    return 0;
}
