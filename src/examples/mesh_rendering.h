#pragma once

#include <stdint.h>

#include "../core/color_buffer.h"
#include "../core/depth_buffer.h"
#include "../core/matrix.h"
#include "../core/mesh.h"
#include "../core/camera.h"
#include "../light.h"
#include "../core/renderer.h"


struct Program {
    void init(int width, int height);
    void update(ColorBuffer *color_buffer);
    void draw(ColorBuffer *color_buffer);
    void after_draw();
    void cleanup();

    void handle_input(float delta_time);

    Light light;

    RendererState renderer_state;

    TinyTriangle *face_buffer = nullptr;
    size_t face_buffer_size = 0;

    DepthBuffer *depth_buffer = nullptr;

    RenderTexture2D render_texture;
};
