#include <stdlib.h>

#include "raylib.h"

#include "../tiny_math.h"
#include "../tiny_color.h"
#include "../mesh.h"
#include "../display.h"

#include "rendering_cube.h"

namespace tinyrenderer::CubeRendering {
float rotation_angle = 0;

void render_mesh_wireframe(ColorBuffer *color_buffer, Mesh *mesh, Mode mode) {
    Vec3f camera_position = {0, 0, 2};
    float fov_factor = 70;

    TinyColor color = 0x9F9F9FFF;

    int half_width = color_buffer->width / 2;
    int half_height = color_buffer->height / 2;

    for (int i = 0; i < mesh->faces.size(); i++) {
        TriangleFace *face = &(mesh->faces[i]);

        Vec3f *face_vertices = (Vec3f *)malloc(sizeof(Vec3f) * 3);
        face_vertices[0] = mesh->vertices[face->a];
        face_vertices[1] = mesh->vertices[face->b];
        face_vertices[2] = mesh->vertices[face->c];

        for (int j = 0; j < 3; j++) {
            Vec3f *vertex = &face_vertices[j];

            Vec3 v_model = Vec3f::rotate_y(vertex, rotation_angle);

            Vec3f v_view = {
                v_model.x,
                v_model.y,
                v_model.z - camera_position.z};

            Vec2f v_projected = {
                fov_factor * v_view.x / v_view.z,
                fov_factor * v_view.y / v_view.z};

            *vertex = {
                v_projected.x + half_width,
                v_projected.y + half_width,
                vertex->z,
            };
        }

        switch(mode) {
            case Mode::Wireframe:
                draw_triangle_wireframe(
                    color_buffer,
                    face_vertices,
                    color
                );
                break;
            case Mode::Triangles:
                // draw_triangles(, , , )
                break;
        }

        free(face_vertices);
    }
}

void CubeRendering::program::init() {
    this->mode = Mode::Wireframe;
    this->mesh.vertices = {};
    this->mesh.faces = {};
    for (int i = 0; i < MESH_VERTICES_COUNT; i++) {
        mesh.vertices.push_back(vertices[i]);
    }

    for (int i = 0; i < MESH_FACES_COUNT; i++) {
        mesh.faces.push_back(faces[i]);
    }
}

void CubeRendering::program::run(ColorBuffer *color_buffer, int delta) {
    rotation_angle = (float)delta / 120;
    render_mesh_wireframe(color_buffer, &mesh, mode);
}
} // namespace tinyrenderer
