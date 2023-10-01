#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>

#include <array>
#include <cstdio>
#include <iostream>
#include <vector>

#include "raylib.h"

#include "core/color_buffer.h"
#include "core/display.h"

#include "examples/draw_rectangles.h"
#include "examples/projection.h"
#include "examples/mesh_rendering.h"
#include "examples/linear_transformations.h"

#include "tooling/logger.h"

#define local_persist static;
#define global_variable static;
#define internal static;

const int window_width = 640;
const int window_height = 480;

const int target_render_size_x = window_width / 2;
const int target_render_size_y = window_height / 2;

int main(int argc, char **argv) {
    logger_init(LOG_LEVEL_DEBUG);

    doctest::Context context;
    context.applyCommandLine(argc, argv);
    int res = context.run();

    if(context.shouldExit()) // important - query flags (and --exit) rely on the user doing this
        return res;          // propagate the result of the tests

    int client_stuff_return_code = 0;

    char title[256];
    InitWindow(window_width, window_height, title);
    SetTargetFPS(60);

    RenderTexture2D render_texture = LoadRenderTexture(target_render_size_x, target_render_size_y);

    Projection projection;

    ColorBuffer color_buffer;
    color_buffer.width = target_render_size_x;
    color_buffer.height = target_render_size_y;
    color_buffer.pixels =
        (uint32_t *)malloc(target_render_size_x * target_render_size_y * sizeof(unsigned int));

    Program mesh_rendering;
    mesh_rendering.init(color_buffer.width, color_buffer.height);

    while (!WindowShouldClose()) {
        snprintf(title, sizeof(title), "pixelscape: FPS: %d - Frame Time: %.4f ms", GetFPS(), 1000.0f/GetFPS());
        SetWindowTitle(title);
        BeginTextureMode(render_texture);
        ClearBackground(BLACK);

        // color_buffer.clear(0x878787ff);
        color_buffer.clear(0x000000ff);

        mesh_rendering.run(&color_buffer);
        draw_axis(&color_buffer);

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
    CloseWindow();

    mesh_rendering.cleanup();

    free(color_buffer.pixels);

    return res + client_stuff_return_code; // the result from doctest is propagated here as well
}
