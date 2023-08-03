#include "render_with_shading.h"
#include "raylib.h"
#include "raymath.h"
#include "../core/display.h"

namespace tinyrenderer::RenderWithShading {

void Program::init() {
    // Initialization code goes here
    // For instance, you may initialize shaders, load texture data, etc.
}

const float far_clipping_plane = 10000;
const float near_clipping_plane = .1;

Vec3f rotation = {};

void Program::run(
    ColorBuffer *color_buffer, 
    Model *model,
    Vector3 &light_dir,
    Image diffuse_texture
) {
    int target_render_size_x = 256;
    int target_render_size_y = 256;
    float *depth_buffer = new float[256 * 256];

    for (uint32_t i = 0; i < target_render_size_x * target_render_size_y; ++i) {
        depth_buffer[i] = -far_clipping_plane;
    }

    float delta = GetFrameTime();
    rotation.y += delta;
    float angle = rotation.y;
    
    Matrix rotation = MatrixRotateY(angle);
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

void Program::cleanup() {
    // Code for cleanup after the program has done its work.
    // Typically you would deallocate any resources you allocated in init()
}

} // namespace tinyrenderer::RenderWithShading

