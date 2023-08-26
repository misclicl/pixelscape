#pragma once

#include <stdint.h>

#include "../core/color_buffer.h"
#include "../core/matrix.h"
#include "../core/mesh.h"
#include "../light.h"

#include "renderer.h"

struct Program {
    void init(int width, int height);
    void run(ColorBuffer *color_buffer);
    void cleanup();
    void project_mesh(
        TinyMesh *mesh,
        size_t index,
        ColorBuffer *color_buffer,
        Matrix4 *mat_world,
        Matrix4 *mat_view
    );

    void render_mesh(
        ColorBuffer *color_buffer,
        size_t index,
        Light *light,
        TinyMesh *mesh
    );

    void handle_input(float delta_time);

    Light light;

    RendererState renderer_state;

    TinyTriangle *face_buffer;
    size_t face_buffer_size = 0;

    float *depth_buffer;
};
