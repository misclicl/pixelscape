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

        Vec3f **face_vertices = (Vec3f **)malloc(sizeof(Vec3f *) * 3);
        face_vertices[0] = mesh->vertices[face->a].position;
        face_vertices[1] = mesh->vertices[face->b].position;
        face_vertices[2] = mesh->vertices[face->c].position;

        Vec3f *transformed_vertices = (Vec3f *)malloc(sizeof(Vec3f) * 3);

        for (int j = 0; j < 3; j++) {
            Vec3f vertex = *face_vertices[j];

            Vec3 v_model = Vec3f::rotate_y(&vertex, mesh->rotation.y);

            Vec3f v_view = {
                v_model.x,
                v_model.y,
                v_model.z - camera_position.z
            };

            Vec2f v_projected = {
                fov_factor * v_view.x / v_view.z,
                fov_factor * v_view.y / v_view.z};

            transformed_vertices[j] = {
                v_projected.x + half_width,
                v_projected.y + half_height,
                vertex.z
            };
        }

        switch (mode) {
        case Mode::Wireframe:
            draw_triangle_wireframe(
                color_buffer,
                transformed_vertices,
                // face_vertices,
                color);
            break;
        case Mode::Triangles:
            break;
        }

        free(face_vertices);
        free(transformed_vertices);
    }
}

void MeshRendering::Program::init() {
    mode = Mode::Wireframe;

    std::vector<Vec3f> vertices;
    std::vector<TriangleFace> faces;
    load_mesh(&vertices, &faces);

    const char* filename = "assets/cube.obj";
    parse_mesh(filename);

    mesh.vertices = (Vertex *)malloc(vertices.size() * sizeof(Vertex));
    mesh.faces = (TriangleFace *)malloc(faces.size() * sizeof(TriangleFace));

    for (int i = 0; i < vertices.size(); i++) {

        // TODO: cleanup
        mesh.vertices[i].position = (Vec3f *)malloc(sizeof(Vec3f));
        *(mesh.vertices[i].position) = vertices[i];
    }

    for (int i = 0; i < faces.size(); i++) {
        mesh.faces[i] = faces[i];
    }

    mesh.face_count = faces.size();
    mesh.vertex_count = vertices.size();
}

void MeshRendering::Program::cleanup() {
    // Clean-up for the positions
    for (int i = 0; i < mesh.vertex_count; i++) {
        free(mesh.vertices[i].position);
        mesh.vertices[i].position = nullptr;  // Set the pointer to nullptr after freeing
    }

    // Clean-up for vertices and faces
    free(mesh.vertices);
    mesh.vertices = nullptr;  // Set the pointer to nullptr after freeing
    free(mesh.faces);
    mesh.faces = nullptr;  // Set the pointer to nullptr after freeing
}

void MeshRendering::Program::run(ColorBuffer *color_buffer, int delta) {
    mesh.rotation.y = (float)delta / 120;
    render_mesh_wireframe(color_buffer, &mesh, mode);
}

} // namespace tinyrenderer::MeshRendering
