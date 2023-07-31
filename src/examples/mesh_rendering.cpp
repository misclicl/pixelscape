#include <bitset>
#include <iostream>
#include <stdlib.h>
#include <vector>

#include "raylib.h"

#include "../core/display.h"
#include "../core/tiny_color.h"
#include "../core/tiny_math.h"
#include "../loader_obj.h"
#include "../mesh.h"

#include "mesh_rendering.h"

#define ROTATION_SPEED 1.2;

namespace tinyrenderer::MeshRendering {

static Vec3f camera_position = {0, 0, 0};

static void render_mesh_wireframe(
    ColorBuffer *color_buffer,
    Mesh *mesh, 
    TinyColor color,
    std::bitset<24> render_flags
) {
    float fov_factor = 140;

    int half_width = color_buffer->width / 2;
    int half_height = color_buffer->height / 2;

    for (int i = 0; i < mesh->face_count; i++) {
        TriangleFace *face = &(mesh->faces[i]);

        Vec3f **face_vertices = (Vec3f **)malloc(sizeof(Vec3f *) * 3);

        face_vertices[0] = mesh->vertices[face->a].position;
        face_vertices[1] = mesh->vertices[face->b].position;
        face_vertices[2] = mesh->vertices[face->c].position;

        Vec3f *v_view = (Vec3f *)malloc(sizeof(Vec3f) * 3);
        Vec3f *v_camera = (Vec3f *)malloc(sizeof(Vec3f) * 3);

        for (int j = 0; j < 3; j++) {
            Vec3f vertex = *face_vertices[j];

            Vec3 v_model = Vec3f::rotate_x(&vertex, DEG2RAD * 180);
            v_model = Vec3f::rotate_y(&v_model, mesh->rotation.y);
            v_model = v_model * 1.2f;

            // v_view[j] = {v_model.x, v_model.y, v_model.z - camera_position.z};
            v_view[j] = {v_model.x, v_model.y, v_model.z - 3};
        }

        // Backface culling check:
        //  a
        //  |\
        //  | \
        //  c - b
        //
        Vec3f a = v_view[0];
        Vec3f b = v_view[1];
        Vec3f c = v_view[2];

        Vec3f vec_ab = b - a;
        Vec3f vec_ac = c - a;
        Vec3f triangle_normal = Vec3f::cross(vec_ab, vec_ac).normalize();

        Vec3f camera_ray = camera_position - a;
        float dot_normal_cam = Vec3f::dot(camera_ray, triangle_normal);

        if (dot_normal_cam < 0.f && render_flags[BACKFACE_CULLING]) {
            continue;
        }

        Vec3f normal_scaled = triangle_normal * .1;
        Vec3f normal_vec_start = a;
        Vec3f normal_vec_end = a + normal_scaled;

        Vec3f normal_end_projected = {
            (fov_factor * normal_vec_end.x / normal_vec_end.z) + half_height,
            (fov_factor * normal_vec_end.y / normal_vec_end.z) + half_height,
            normal_vec_end.z};
        Vec3f normal_start_projected = {
            (fov_factor * normal_vec_start.x / normal_vec_start.z) + half_height,
            (fov_factor * normal_vec_start.y / normal_vec_start.z) + half_height,
            normal_vec_start.z,
        };

        for (int j = 0; j < 3; j++) {
            Vec2f v_projected = {
                fov_factor * v_view[j].x / v_view[j].z,
                fov_factor * v_view[j].y / v_view[j].z};

            v_camera[j] = {
                v_projected.x + half_width,
                v_projected.y + half_height,
                face_vertices[j]->z,
            };
        }

        Color normal_color = {
            (unsigned char)(255 * (triangle_normal.x + 1) / 2),
            (unsigned char)(255 * (triangle_normal.y + 1) / 2),
            (unsigned char)(255 * (triangle_normal.z + 1) / 2),
            255};


        draw_triangle_wireframe(color_buffer, v_camera, color);

        if (render_flags[DISPLAY_NORMALS]) {
          draw_line(color_buffer, 
                    &normal_start_projected,
                    &normal_end_projected, 
                    ColorToInt(normal_color));
        }

        if (render_flags[DISPLAY_VERTICES]) {
            color_buffer->set_pixel(v_camera[0].x, v_camera[0].y, 0xFF0000FF);
            color_buffer->set_pixel(v_camera[1].x, v_camera[1].y, 0xFF0000FF);
            color_buffer->set_pixel(v_camera[2].x, v_camera[2].y, 0xFF0000FF);
        }


        free(face_vertices);
        free(v_view);
        free(v_camera);
    }
}

void MeshRendering::Program::init() {
    char *bunny_obj_path = (char *)"assets/bunny-lr.obj";
    char *cube_obj_path = (char *)"assets/cube.obj";
    char *head_obj_path = (char *)"assets/head.obj";

    std::vector<Vec3f> vertices;
    std::vector<TriangleFace> faces;

    parse_mesh(head_obj_path, &vertices, &faces);

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
        mesh.vertices[i].position = nullptr; // Set the pointer to nullptr after freeing
    }

    // Clean-up for vertices and faces
    free(mesh.vertices);
    mesh.vertices = nullptr; // Set the pointer to nullptr after freeing
    free(mesh.faces);
    mesh.faces = nullptr; // Set the pointer to nullptr after freeing
}

void MeshRendering::Program::run(ColorBuffer *color_buffer) {
    float delta = GetFrameTime();

    mesh.rotation.y += (float)delta * ROTATION_SPEED;
    mesh.rotation.y = fmod(mesh.rotation.y, DEG2RAD * 361);

    if (IsKeyPressed(KEY_ONE)) {
        render_flags.flip(BACKFACE_CULLING);
        // printf("Backface culling: %d\n", render_flags[BACKFACE_CULLING]);
    }

    if (IsKeyPressed(KEY_TWO)) {
        render_flags.flip(DISPLAY_NORMALS);
        std::cout << "Backface culling: " << render_flags[BACKFACE_CULLING] << "\n";
    }

    if (IsKeyPressed(KEY_THREE)) {
        render_flags.flip(DISPLAY_VERTICES);
        std::cout << "Display vertices: " << render_flags[DISPLAY_VERTICES] << "\n";
    }

    render_mesh_wireframe(color_buffer, &mesh, 0xFFFFFFFF, render_flags);
}

} // namespace tinyrenderer::MeshRendering
