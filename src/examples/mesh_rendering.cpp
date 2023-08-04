#include <bitset>
#include <cmath>
#include <iostream>
#include <stdlib.h>
#include <vector>

#include "raylib.h"

#include "../core/display.h"
#include "../core/tiny_color.h"
#include "../core/tiny_math.h"
#include "../core/matrix.h"
#include "../loader_obj.h"
#include "../mesh.h"

#include "mesh_rendering.h"

#define ROTATION_SPEED 1.2f

namespace tinyrenderer::MeshRendering {

static Vec3f camera_position = {0, 0, 0};
static float fov_factor = 140;

static void draw_triangle_normal(
    ColorBuffer *color_buffer, 
    Vec3f *vertices, 
    Vec3f *normal_vec) {

    Vec3f normal_scaled = *normal_vec * .1;
    Vec3f normal_vec_start = vertices[0];
    Vec3f normal_vec_end = vertices[0] + normal_scaled;

    int half_width = color_buffer->width / 2;
    int half_height = color_buffer->height / 2;

    Vec3f normal_end_projected = {
        (fov_factor * normal_vec_end.x / normal_vec_end.z) + half_height,
        (fov_factor * normal_vec_end.y / normal_vec_end.z) + half_height,
        normal_vec_end.z};

    Vec3f normal_start_projected = {
        (fov_factor * normal_vec_start.x / normal_vec_start.z) + half_height,
        (fov_factor * normal_vec_start.y / normal_vec_start.z) + half_height,
        normal_vec_start.z,
    };
}

static Vec3f get_triangle_normal(Vec3f *vertices) {
    Vec3f a = vertices[0];
    Vec3f b = vertices[1];
    Vec3f c = vertices[2];

    Vec3f vec_ab = b - a;
    Vec3f vec_ac = c - a;
    Vec3f triangle_normal = Vec3f::cross(vec_ab, vec_ac).normalize();

    return triangle_normal;
};

static void render_triangle(
    ColorBuffer *color_buffer,
    Vec3f *vertices,
    std::bitset<24> render_flags,
    TinyColor face_color
) {
        // Color normal_color = {
        //     (unsigned char)(255 * (triangle_normal.x + 1) / 2),
        //     (unsigned char)(255 * (triangle_normal.y + 1) / 2),
        //     (unsigned char)(255 * (triangle_normal.z + 1) / 2),
        //     255};

        if (render_flags[DISPLAY_TRIANGLES]) {
            draw_triangle(color_buffer, vertices, face_color);
        }

        if (render_flags[DISPLAY_WIREFRAME]) {
            draw_triangle_wireframe(color_buffer, vertices, 0xAFAFAFAF);
        }

        if (render_flags[DISPLAY_VERTICES]) {
            color_buffer->set_pixel(vertices[0].x, vertices[0].y, 0xF57716FF);
            color_buffer->set_pixel(vertices[1].x, vertices[1].y, 0xF57716FF);
            color_buffer->set_pixel(vertices[2].x, vertices[2].y, 0xF57716FF);
        }

}

void MeshRendering::Program::project_mesh( ColorBuffer *color_buffer ) {
    faces_to_render.clear();

    int depth_buffer_size = color_buffer->height * color_buffer->width;
    float depth_buffer[depth_buffer_size];
    std::fill_n(depth_buffer, depth_buffer_size, -10000);

    int half_width = color_buffer->width / 2;
    int half_height = color_buffer->height / 2;


    Vec3f **face_vertices = (Vec3f **)malloc(sizeof(Vec3f *) * 3);

    Vec3f v_view[3];
    Vec3f v_camera[3];

    Matrix4 mat_scale = mat4_get_scale(
        mesh.scale.x,
        mesh.scale.y,
        mesh.scale.z
    );

    Matrix4 mat_translate = mat4_get_translation(
        mesh.translation.x,
        mesh.translation.y,
        mesh.translation.z
    );

    Matrix4 mat_rotation_x = mat4_get_rotation_x(DEG2RAD * -10);
    Matrix4 mat_rotation_y = mat4_get_rotation_y(mesh.rotation.y);
    Matrix4 mat_rotation_z = mat4_get_rotation_z(mesh.rotation.z);
    
    for (int i = 0; i < mesh.face_count; i++) {
        TinyFace *face = &(mesh.faces[i]);

        face_vertices[0] = mesh.vertices[face->indices[0]].position;
        face_vertices[1] = mesh.vertices[face->indices[1]].position;
        face_vertices[2] = mesh.vertices[face->indices[2]].position;

        for (int j = 0; j < 3; j++) {
            Vec3f vertex = *face_vertices[j];

            Vec3f v_model;

            Vec4f v_scaled = mat4_multiply_vec4(&mat_scale, vec4_from_vec3(vertex));

            Vec4f v_rotated = mat4_multiply_vec4(&mat_rotation_x, v_scaled);
            v_rotated = mat4_multiply_vec4(&mat_rotation_y, v_rotated);
            v_rotated = mat4_multiply_vec4(&mat_rotation_z, v_rotated);

            // Moving away from the camera
            Vec4f v_translated = mat4_multiply_vec4(&mat_translate, v_rotated);

            // FIXME: No need to transform for now as the camera is located in the origin
            v_view[j] = {v_translated.x, v_translated.y, v_translated.z};

        }

        Vec3f triangle_normal = get_triangle_normal(v_view);
        Color normal_color = {
            (unsigned char)(255 * (triangle_normal.x + 1) / 2),
            (unsigned char)(255 * (triangle_normal.y + 1) / 2),
            (unsigned char)(255 * (triangle_normal.z + 1) / 2),
            255};

        if (render_flags[BACKFACE_CULLING]) {
            Vec3f camera_ray = camera_position - v_view[0];
            float dot_normal_cam = Vec3f::dot(camera_ray, triangle_normal);

            if (dot_normal_cam < 0.f && render_flags[BACKFACE_CULLING]) {
                continue;
            }
        }

        // view -> projection step
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


        // Calculate face depth
        float avg_depth = (v_view[0].z + v_view[1].z + v_view[2].z) / 3.f;

        // Write transformed to buffer
        FaceBufferItem f = {
            .vertices = {
                { v_camera[0].x, v_camera[0].y, v_camera[0].z },
                { v_camera[1].x, v_camera[1].y, v_camera[1].z },
                { v_camera[2].x, v_camera[2].y, v_camera[2].z },
            },
            .normal_vec = triangle_normal, 
            .avg_depth = avg_depth,
            // .color = face->color,
            .color = (TinyColor)ColorToInt(normal_color)
        };

        faces_to_render.push_back(f);

    }

    int num_triangles = faces_to_render.size();

    if (render_flags[VERTEX_ORDERING]) {
        std::sort(faces_to_render.begin(), faces_to_render.end(), [](const FaceBufferItem &a, const FaceBufferItem &b) {
            return a.avg_depth < b.avg_depth;
        });
    }

    free(face_vertices);
}

void MeshRendering::Program::render_mesh(ColorBuffer *color_buffer) {
    for (FaceBufferItem face : faces_to_render) {
        render_triangle(color_buffer, face.vertices, render_flags, face.color);

        // FIXME: Rendering for normals doesn't work
        // The reason for that is that I use projected vertices
        // if (render_flags[DISPLAY_NORMALS]) {
        //     draw_triangle_normal(color_buffer, face.vertices, &face.normal_vec);
        // }
    }
}

void MeshRendering::Program::init() {
    char *bunny_obj_path = (char *)"assets/bunny-lr.obj";
    char *cube_obj_path = (char *)"assets/cube.obj";
    // char *head_obj_path = (char *)"assets/head.obj";
    // char *head_obj_path = (char *)"assets/headscan.obj";
    char *head_obj_path = (char *)"assets/head-mod.obj";

    render_flags.set(DISPLAY_TRIANGLES, 1);
    render_flags.set(BACKFACE_CULLING, 1);
    render_flags.set(VERTEX_ORDERING, 1);

    std::vector<Vec3f> vertices;
    std::vector<TinyFace> faces;

    parse_mesh(head_obj_path, &vertices, &faces);

    mesh.vertices = (Vertex *)malloc(vertices.size() * sizeof(Vertex));
    mesh.faces = (TinyFace *)malloc(faces.size() * sizeof(TinyFace));

    for (int i = 0; i < vertices.size(); i++) {
        mesh.vertices[i].position = (Vec3f *)malloc(sizeof(Vec3f));
        *(mesh.vertices[i].position) = vertices[i];
    }

    for (int i = 0; i < faces.size(); i++) {
        mesh.faces[i] = faces[i];
    }

    mesh.face_count = faces.size();
    mesh.vertex_count = vertices.size();
    mesh.scale = { 1.5f, 1.5f, 1.5f };
    mesh.translation = { 0.f, 0.f, -3.f };
}

void MeshRendering::Program::run(ColorBuffer *color_buffer) {
    handle_input();

    float delta = GetFrameTime();
    float elapsed = GetTime();

    mesh.rotation.y = fmod((elapsed * ROTATION_SPEED), DEG2RAD * 360);
    // mesh.rotation.z = fmod((elapsed * ROTATION_SPEED), DEG2RAD * 360);

    project_mesh(color_buffer);
    render_mesh(color_buffer);
}

void MeshRendering::Program::handle_input() {
    if (IsKeyPressed(KEY_C)) {
        render_flags.flip(BACKFACE_CULLING);
        std::cout << "Backface culling: " << render_flags[BACKFACE_CULLING] << "\n";
    }
    if (IsKeyPressed(KEY_N)) {
        render_flags.flip(DISPLAY_NORMALS);
        std::cout << "Display normals: " << render_flags[BACKFACE_CULLING] << "\n";
    }
    if (IsKeyPressed(KEY_Z)) {
        render_flags.flip(VERTEX_ORDERING);
        std::cout << "Vertex ordering : " << render_flags[VERTEX_ORDERING] << "\n";
    }

    if (IsKeyPressed(KEY_ONE)) {
        render_flags.flip(DISPLAY_VERTICES);
        std::cout << "Display vertices: " << render_flags[DISPLAY_VERTICES] << "\n";
    }
    if (IsKeyPressed(KEY_TWO)) {
        render_flags.flip(DISPLAY_WIREFRAME);
        std::cout << "Display wireframe: " << render_flags[DISPLAY_WIREFRAME] << "\n";
    }
    if (IsKeyPressed(KEY_THREE)) {
        render_flags.flip(DISPLAY_TRIANGLES);
        std::cout << "Display triangles: " << render_flags[DISPLAY_TRIANGLES] << "\n";
    }
}
    
void MeshRendering::Program::cleanup() {
    // Clean-up for the positions
    for (int i = 0; i < mesh.vertex_count; i++) {
        free(mesh.vertices[i].position);
        mesh.vertices[i].position = nullptr; // Set the pointer to nullptr after freeing
    }

    // Clean-up for vertices and faces
    free(mesh.vertices);
    free(mesh.faces);
    mesh.vertices = nullptr; // Set the pointer to nullptr after freeing
    mesh.faces = nullptr; // Set the pointer to nullptr after freeing
}

} // namespace tinyrenderer::MeshRendering

