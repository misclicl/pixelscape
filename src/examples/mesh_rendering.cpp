#include <stdlib.h>
#include <vector>

#include "raylib.h"

#include "../display.h"
#include "../mesh.h"
#include "../tiny_color.h"
#include "../tiny_math.h"
#include "../loader_obj.h"

#include "mesh_rendering.h"

namespace tinyrenderer::MeshRendering {
void render_mesh_wireframe(ColorBuffer *color_buffer, Mesh *mesh, Mode mode) {
    Vec3f camera_position = {0, 0, 2};
    float fov_factor = 90;

    TinyColor color = 0x9F9F9FFF;

    int half_width = color_buffer->width / 2;
    int half_height = color_buffer->height / 2;

    for (int i = 0; i < mesh->face_count; i++) {
        TriangleFace *face = &(mesh->faces[i]);

        Vec3f *face_vertices = (Vec3f *)malloc(sizeof(Vec3f) * 3);
        face_vertices[0] = mesh->vertices[face->a];
        face_vertices[1] = mesh->vertices[face->b];
        face_vertices[2] = mesh->vertices[face->c];

        for (int j = 0; j < 3; j++) {
            Vec3f *vertex = &face_vertices[j];

            Vec3 v_model = Vec3f::rotate_y(vertex, mesh->rotation.y);

            Vec3f v_view = {
                v_model.x,
                v_model.y,
                v_model.z - camera_position.z
            };

            Vec2f v_projected = {
                fov_factor * v_view.x / v_view.z,
                fov_factor * v_view.y / v_view.z};

            *vertex = {
                v_projected.x + half_width,
                v_projected.y + half_width,
                vertex->z,
            };
        }

        switch (mode) {
        case Mode::Wireframe:
            draw_triangle_wireframe(
                color_buffer,
                face_vertices,
                color);
            break;
        case Mode::Triangles:
            break;
        }

        free(face_vertices);
    }
}

void MeshRendering::Program::init() {
    mode = Mode::Wireframe;

    std::vector<Vec3f> vertices;
    std::vector<TriangleFace> faces;
    load_mesh(&vertices, &faces);

    const char* filename = "assets/cube.obj";
    parse_mesh(filename);

    mesh.vertices = (Vec3f *)malloc(vertices.size() * sizeof(Vec3f));
    mesh.faces = (TriangleFace *)malloc(faces.size() * sizeof(TriangleFace));

    for (int i = 0; i < vertices.size(); i++) {
        mesh.vertices[i] = vertices[i];
    }

    for (int i = 0; i < faces.size(); i++) {
        mesh.faces[i] = faces[i];
    }

    // mesh.rotation = {0.f, 0.f, 0.f};

    mesh.face_count = faces.size();
    mesh.vertex_count = vertices.size();
}

void MeshRendering::Program::cleanup() {
    free(this->mesh.vertices);
    free(this->mesh.faces);
}

void MeshRendering::Program::run(ColorBuffer *color_buffer, int delta) {
    mesh.rotation.y = (float)delta / 120;
    render_mesh_wireframe(color_buffer, &mesh, mode);
}

} // namespace tinyrenderer::MeshRendering
