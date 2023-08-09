#include <array>
#include <cstdio>
#include <iostream>
#include <vector>

#include "examples/background_grid.h"
#include "examples/draw_rectangles.h"
#include "examples/projection.h"
#include "examples/mesh_rendering.h"
#include "examples/linear_transformations.h"

#include "examples/render_with_shading.h"

#include "mesh.h"
#include "raylib.h"
#include "raymath.h"

#include "core/color_buffer.h"
#include "core/display.h"
#include "model.h"

#define local_persist static;
#define global_variable static;
#define internal static;

const int window_width = 512;
const int window_height = 512;

const int target_render_size_x = window_width / 2;
const int target_render_size_y = window_height / 2;

int main(int argc, char **argv) {
    InitWindow(window_width, window_height, "pixelscape");
    SetTargetFPS(60);

    pixelscape::Model *model = new pixelscape::Model("assets/head.obj");

    // Image diffuse_img = LoadImage("assets/diffuse.png");
    Image diffuse_img = LoadImage("assets/grid.png");

    Vector3 light_dir = {0, 0, -1};

    RenderTexture2D render_texture = LoadRenderTexture(target_render_size_x, target_render_size_y);


    pixelscape::program::BackgroundGrid bg_grid;
    pixelscape::program::DrawRectangles draw_rectangles;
    pixelscape::program::Projection projection;


    pixelscape::RenderWithShading::Program render_with_shading;


    pixelscape::ColorBuffer color_buffer;
    color_buffer.width = target_render_size_x;
    color_buffer.height = target_render_size_y;
    color_buffer.pixels =
        (uint32_t *)malloc(target_render_size_x * target_render_size_y * sizeof(unsigned int));

    pixelscape::MeshRendering::Program mesh_rendering;
    mesh_rendering.init(color_buffer.width, color_buffer.height);

    while (!WindowShouldClose()) {
        BeginTextureMode(render_texture);
        ClearBackground(BLACK);

        // color_buffer.clear(0xa3a3a3ff);
        color_buffer.clear(0x878787ff);

        // render_vertices(diffuse_img);
        // examples::shape_perspective();
        // bg_grid.run(&color_buffer);
        // draw_rectangles.run(&color_buffer);
        // projection.run(&color_buffer, frame_counter);
        // pixelscape::linear_transformations(&color_buffer);

        // render_with_shading.run(&color_buffer, model, light_dir, diffuse_img);
        mesh_rendering.run(&color_buffer);
        pixelscape::draw_axis(&color_buffer);

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

        EndDrawing();
    }

    UnloadRenderTexture(render_texture);
    UnloadImage(diffuse_img);
    CloseWindow();

    mesh_rendering.cleanup();
    render_with_shading.cleanup();

    delete model;
    free(color_buffer.pixels);

    return 0;
}
