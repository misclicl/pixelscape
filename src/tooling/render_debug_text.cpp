#include "render_debug_text.h"

// FIXME:
void draw_debug_quad(
    ColorBuffer *color_buffer,
    Image *diffuse_texture,
    float *depth_buffer
) {
    Vec4f verts[3] = {
        {0, 0, -5, 1},
        {45, 45, -5, 1},
        {0, 45, -5, 1}
    };

    Vec2f uvs[3] = {
        {0, 1},
        {1, 0},
        {0, 0}
    };

    TinyTriangle triangle = {
        .vertices = {
            {
                .position={0, 0, -5, 1},
                .texcoords={0, 1},
            },
            {
                .position={0, 0, -5, 1},
                .texcoords={0, 1},
            },
            {
                .position={0, 0, -5, 1},
                .texcoords={0, 1},
            }
        }
    };

    // draw_triangle(
    //     color_buffer,
    //     depth_buffer,
    //     triangle,
    //     diffuse_texture
    //     // 1.0f,
    //     // nullptr
    // );

    // verts[0] = {0, 0, -5, 1};
    // verts[1] = {45, 0, -5, 1};
    // verts[2] = {45, 45, -5, 1};
    // uvs[0] = {0, 1};
    // uvs[1] = {1, 1};
    // uvs[2] = {1, 0};

    // draw_triangle(
    //     color_buffer,
    //     depth_buffer,
    //     verts,
    //     uvs,
    //     0x000000FF,
    //     diffuse_texture,
    //     1.0f,
    //     nullptr
    // );
}