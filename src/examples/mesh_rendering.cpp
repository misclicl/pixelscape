#include <stdlib.h>
#include <vector>

#include "raylib.h"

#include "../core/display.h"
#include "../core/tiny_color.h"
#include "../core/tiny_math.h"

#include "../mesh.h"
#include "../loader_obj.h"

#include "mesh_rendering.h"

#define ROTATION_SPEED 1.2;

namespace tinyrenderer::MeshRendering {
void render_mesh_wireframe(ColorBuffer *color_buffer, Mesh *mesh, Mode mode, TinyColor color) {
    Vec3f camera_position = {0, 0, 2};
    float fov_factor = 90;


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

    char* bunny_obj_path = (char *)"assets/bunny-lr.obj";
    char* cube_obj_path = (char *)"assets/cube.obj";
    char* head_obj_path = (char *)"assets/head.obj";

    std::vector<Vec3f> vertices;
    std::vector<TriangleFace> faces;

    parse_mesh(cube_obj_path, &vertices, &faces);

    mesh.vertices = (Vertex *)malloc(vertices.size() * sizeof(Vertex));
    mesh.faces = (TriangleFace *)malloc(faces.size() * sizeof(TriangleFace));

    for (int i = 0; i < vertices.size(); i++) {
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

void MeshRendering::Program::run(ColorBuffer *color_buffer) {
    float delta = GetFrameTime();

    mesh.rotation.y += (float)delta * ROTATION_SPEED;
    mesh.rotation.y = fmod(mesh.rotation.y, RAD2DEG * 361);

    printf("%f\n", mesh.rotation.y);
    render_mesh_wireframe(color_buffer, &mesh, mode, 0xFFFFFFFF);
}

} // namespace tinyrenderer::MeshRendering
