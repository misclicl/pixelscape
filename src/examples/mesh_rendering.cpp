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

namespace pixelscape::MeshRendering {

static Vec3f camera_position = {0, 0, 0};
static float camera_fov = 45;
static TinyColor default_color = 0xafafafff;

// FIXME: I need to project with matrix now
static void draw_triangle_normal(
    ColorBuffer *color_buffer, 
    Vec3f *vertices, 
    Vec3f *triangle_normal) {
    //
    // Vec3f normal_scaled = *triangle_normal * .1;
    // Vec3f triangle_normal_start = vertices[0];
    // Vec3f triangle_normal_end = vertices[0] + normal_scaled;
    //
    // int half_width = color_buffer->width / 2;
    // int half_height = color_buffer->height / 2;
    //
    // Vec3f normal_end_projected = {
    //     (fov_factor * triangle_normal_end.x / triangle_normal_end.z) + half_height,
    //     (fov_factor * triangle_normal_end.y / triangle_normal_end.z) + half_height,
    //     triangle_normal_end.z};
    //
    // Vec3f normal_start_projected = {
    //     (fov_factor * triangle_normal_start.x / triangle_normal_start.z) + half_height,
    //     (fov_factor * triangle_normal_start.y / triangle_normal_start.z) + half_height,
    //     triangle_normal_start.z,
    // };
}

static Vec3f get_triangle_normal(Vec3f *vertices) {
    Vec3f a = vertices[0];
    Vec3f b = vertices[1];
    Vec3f c = vertices[2];

    Vec3f vec_ab = b - a;
    Vec3f vec_ac = c - a;
    return Vec3f::cross(vec_ab, vec_ac).normalize();
};

static void render_triangle(
    ColorBuffer *color_buffer,
    FaceBufferItem *face,
    std::bitset<24> render_flags,
    Light *light,
    Image *diffuse_texture
) {
    float alignment = std::max(-Vec3f::dot(light->direction.normalize(), face->triangle_normal), 0.f);
    float intensity = render_flags[ENABLE_SHADING] ?
        alignment:
        1.f;

    TinyColor normal_color = tiny_color_from_rgb(
        (255 * (face->triangle_normal.x + 1) / 2),
        (255 * (face->triangle_normal.y + 1) / 2),
        (255 * (face->triangle_normal.z + 1) / 2));

    TinyColor base_color;
    
    base_color = render_flags[ENABLE_FACE_NORMALS] ?
                 normal_color : default_color;

    base_color = apply_intensity(base_color, intensity);

    // TODO: might be a good idea to move all vertex array conversion 
    // over here. I already done it in DISPLAY_VERTICES section
    if (render_flags[DISPLAY_TRIANGLES]) {
        draw_triangle(
            color_buffer, 
            face->vertices,
            face->texcoords,
            base_color,
            diffuse_texture
        );
    }

    if (render_flags[DISPLAY_WIREFRAME]) {
        // draw_triangle_wireframe(color_buffer, face->vertices, 0xFFAFAFAF);
    }

    if (render_flags[DISPLAY_VERTICES]) {
        color_buffer->set_pixel(face->vertices[0].x, face->vertices[0].y, 0xF57716FF);
        color_buffer->set_pixel(face->vertices[1].x, face->vertices[1].y, 0xF57716FF);
        color_buffer->set_pixel(face->vertices[2].x, face->vertices[2].y, 0xF57716FF);
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
    // Vec3f **

    Vec3f v_view[3];
    Vec4f v_camera[3];

    Matrix4 mat_world = mat4_get_world(
        mesh.scale,
        mesh.rotation,
        mesh.translation
    );
    
    for (int i = 0; i < mesh.face_count; i++) {
        TinyFace *face = &(mesh.faces[i]);

        face_vertices[0] = &(mesh.vertices[face->indices[0]].position);
        face_vertices[1] = &(mesh.vertices[face->indices[1]].position);
        face_vertices[2] = &(mesh.vertices[face->indices[2]].position);

        for (int j = 0; j < 3; j++) {
            Vec3f vertex = *face_vertices[j];

            Vec4f v_world = mat4_multiply_vec4(&mat_world, vec4_from_vec3(vertex));

            // FIXME: No need to transform for now as the camera is located in the origin
            v_view[j] = {v_world.x, v_world.y, v_world.z};
        }

        Vec3f triangle_normal = get_triangle_normal(v_view);

        if (render_flags[BACKFACE_CULLING]) {
            Vec3f camera_ray = camera_position - v_view[0];
            float dot_normal_cam = Vec3f::dot(camera_ray, triangle_normal);

            if (dot_normal_cam < 0.f && render_flags[BACKFACE_CULLING]) {
                continue;
            }
        }

        // Calculate face depth
        float avg_depth = (v_view[0].z + v_view[1].z + v_view[2].z) / 3.f;

        // view -> projection step
        float aspect_ratio = (float)(color_buffer->height) / (float)(color_buffer->width);
        Matrix4 projection = mat4_get_projection(aspect_ratio, DEG2RAD * camera_fov, -.1f, -100.f);

        for (int j = 0; j < 3; j++) {
            // This gives me image space or NDC
            Vec4f v_projected = mat4_multiply_projection_vec4(projection, vec4_from_vec3(v_view[j]));
        
            v_camera[j] = {
                (v_projected.x * half_width) + half_width,
                (v_projected.y * half_height)+ half_height,
                // TODO: multiply?
                v_projected.z,
                v_projected.w,
            };
        }

        // Write transformed to buffer
        FaceBufferItem f = {
            .vertices = {
                { v_camera[0].x, v_camera[0].y, v_camera[0].z, v_camera[0].w},
                { v_camera[1].x, v_camera[1].y, v_camera[1].z, v_camera[1].w},
                { v_camera[2].x, v_camera[2].y, v_camera[2].z, v_camera[2].w},
            },
            .texcoords = {
                { 
                    mesh.vertices[face->indices[0]].texcoords.x,
                    mesh.vertices[face->indices[0]].texcoords.y,
                },
                {
                    mesh.vertices[face->indices[1]].texcoords.x,
                    mesh.vertices[face->indices[1]].texcoords.y,
                },
                { 
                    mesh.vertices[face->indices[2]].texcoords.x,
                    mesh.vertices[face->indices[2]].texcoords.y,
                }
            },
            .triangle_normal = triangle_normal, 
            .avg_depth = avg_depth,
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
        // TODO: add pointer to a texture
        render_triangle(color_buffer, &face, render_flags, &light, &mesh.diffuse_texture);

        // FIXME: Rendering for normals doesn't work
        // The reason for that is that I use projected vertices
        // if (render_flags[DISPLAY_NORMALS]) {
        //     draw_triangle_normal(color_buffer, face.vertices, &face.triangle_normal);
        // }
    }
}

void MeshRendering::Program::init() {
    char *bunny_obj_path = (char *)"assets/bunny-lr.obj";
    char *cube_obj_path = (char *)"assets/cube.obj";
    // char *head_obj_path = (char *)"assets/head.obj";
    char *head_obj_path = (char *)"assets/headscan.obj";
    // char *head_obj_path = (char *)"assets/head-mod.obj";

    render_flags.set(DISPLAY_TRIANGLES, 1);
    render_flags.set(BACKFACE_CULLING, 1);
    render_flags.set(VERTEX_ORDERING, 1);
    render_flags.set(ENABLE_SHADING, 1);

    std::vector<Vertex> vertices;
    std::vector<TinyFace> faces;

    parse_mesh(cube_obj_path, &vertices, &faces);

    mesh.vertices = (Vertex *)malloc(vertices.size() * sizeof(Vertex));
    mesh.faces = (TinyFace *)malloc(faces.size() * sizeof(TinyFace));

    for (int i = 0; i < vertices.size(); i++) {
        mesh.vertices[i].position = vertices[i].position;
        mesh.vertices[i].texcoords = vertices[i].texcoords;
    }

    for (int i = 0; i < faces.size(); i++) {
        mesh.faces[i] = faces[i];
    }

    mesh.face_count = faces.size();
    mesh.vertex_count = vertices.size();
    mesh.scale = { 1.0f, 1.0f, 1.0f };
    mesh.translation = { 0.f, 0.f, -5.f };
    mesh.rotation.x = -DEG2RAD * 10;
    // mesh.diffuse_texture = LoadImage("assets/headscan-128.png");
    mesh.diffuse_texture = LoadImage("assets/grid.png");

    light.direction = { 0.f, 1.f, -.5f };
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
        render_flags.flip(ENABLE_FACE_NORMALS);
        std::cout << "Enable face normals: " << render_flags[ENABLE_FACE_NORMALS] << "\n";
    }
    if (IsKeyPressed(KEY_S)) {
        render_flags.flip(ENABLE_SHADING);
        std::cout << "Enable shading: " << render_flags[ENABLE_SHADING] << "\n";
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
    // Clean-up for vertices and faces
    free(mesh.vertices);
    free(mesh.faces);
    mesh.vertices = nullptr; // Set the pointer to nullptr after freeing
    mesh.faces = nullptr; // Set the pointer to nullptr after freeing

    UnloadImage(mesh.diffuse_texture);
}

} // namespace pixelscape::MeshRendering

