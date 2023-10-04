#include "pipeline.h"



void render_triangle(
    CameraType camera_type,
    ColorBuffer *color_buffer,
    DepthBuffer *depth_buffer,
    TinyTriangle *triangle,
    RendererState *renderer_state,
    Light *light,
    Image *diffuse_texture
) {
    // TODO: might be a good idea to move all vertex array conversion
    // over here. I already done it in SHOW_VERTICES section
    if (renderer_state->flags[DRAW_TRIANGLES]) {
        draw_triangle(
            camera_type,
            color_buffer,
            depth_buffer,
            triangle,
            diffuse_texture,
            light,
            renderer_state
        );
    }

    if (renderer_state->flags[DRAW_WIREFRAME]) {
        Vec4f vertices[3] =  {
            triangle->vertices[0].position,
            triangle->vertices[1].position,
            triangle->vertices[2].position
        };
        draw_triangle_wireframe(color_buffer, vertices, 0xAAAAAB00);
    }

    if (renderer_state->flags[DRAW_VERTICES]) {
        color_buffer->set_pixel(triangle->vertices[0].position.x, triangle->vertices[0].position.y, 0xF57716FF);
        color_buffer->set_pixel(triangle->vertices[1].position.x, triangle->vertices[1].position.y, 0xF57716FF);
        color_buffer->set_pixel(triangle->vertices[2].position.x, triangle->vertices[2].position.y, 0xF57716FF);
    }
}