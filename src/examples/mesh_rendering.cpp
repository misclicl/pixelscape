#include <bitset>
#include <cmath>
#include <iostream>
#include <stdlib.h>
#include <vector>

#include "raylib.h"
#include "raymath.h"

#include "../core/display.h"
#include "../core/tiny_color.h"
#include "../core/tiny_math.h"
#include "../core/matrix.h"
#include "../core/camera.h"
#include "../loader_obj.h"
#include "../mesh.h"

#include "mesh_rendering.h"

#define ROTATION_SPEED 0.2f
#define FACE_BUFFER_SIZE_LIMIT 250000

namespace pixelscape::MeshRendering {

static float camera_fov = 45;
static TinyColor default_color = 0xafafafff;
static Color light_color = {200, 20, 180, 255};

static TinyCamera camera;

static Vec3f get_triangle_normal(Vec4f a, Vec4f b, Vec4f c) {
    Vec3f vec_ab = vec3_from_vec4(b - a);
    Vec3f vec_ac = vec3_from_vec4(c - a);

    return Vec3f::cross(vec_ab, vec_ac).normalize();
};

static Matrix4 mat_to_mat4(Matrix in) {
    Matrix4 out = {};

    out.m0 = in.m0;
    out.m1 = in.m1;
    out.m2 = in.m2;
    out.m3 = in.m3;
    out.m4 = in.m4;
    out.m5 = in.m5;
    out.m6 = in.m6;
    out.m7 = in.m7;
    out.m8 = in.m8;
    out.m9 = in.m9;
    out.m10 = in.m10;
    out.m11 = in.m11;
    out.m12 = in.m12;
    out.m13 = in.m13;
    out.m14 = in.m14;
    out.m15 = in.m15;
    return out;
}

static void draw_debug_quad(
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

    draw_triangle(
        color_buffer,
        depth_buffer,
        verts,
        uvs,
        0x000000FF,
        diffuse_texture,
        1.0f,
        true
    );

    verts[0] = {0, 0, -5, 1};
    verts[1] = {45, 0, -5, 1};
    verts[2] = {45, 45, -5, 1};
    uvs[0] = {0, 1};
    uvs[1] = {1, 1};
    uvs[2] = {1, 0};

    draw_triangle(
        color_buffer,
        depth_buffer,
        verts,
        uvs,
        0x000000FF,
        diffuse_texture,
        1.0f,
        true
    );
}

static void draw_light_dir(ColorBuffer *color_buffer, Light *light) {
    draw_line(
        color_buffer, 
        20, 220, 
        20 + light->direction.x * 10, 220 + light->direction.y * 10, 
        0xFFFFFFFF
    );

    color_buffer->set_pixel(20, 20, 0xF57716FF);
}

static void render_triangle(
    ColorBuffer *color_buffer,
    float *depth_buffer,
    FaceBufferItem *face,
    std::bitset<24> render_flags,
    Light *light,
    Image *diffuse_texture
) {
    float alignment = -Vec3f::dot(light->direction.normalize(), face->triangle_normal);

    alignment = std::max(alignment, 0.f);

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

    base_color = apply_intensity(base_color, light_color, intensity * 2);

    // TODO: might be a good idea to move all vertex array conversion 
    // over here. I already done it in DISPLAY_VERTICES section
    if (render_flags[DISPLAY_TRIANGLES]) {
        draw_triangle(
            color_buffer, 
            depth_buffer,
            face->vertices,
            face->texcoords,
            base_color,
            diffuse_texture,
            intensity,
            render_flags[ENABLE_Z_BUFFER_CHECK]
        );
    }

    if (render_flags[DISPLAY_WIREFRAME]) {
        draw_triangle_wireframe(color_buffer, face->vertices, 0xAAAAAB00);
    }

    if (render_flags[DISPLAY_VERTICES]) {
        color_buffer->set_pixel(face->vertices[0].x, face->vertices[0].y, 0xF57716FF);
        color_buffer->set_pixel(face->vertices[1].x, face->vertices[1].y, 0xF57716FF);
        color_buffer->set_pixel(face->vertices[2].x, face->vertices[2].y, 0xF57716FF);
    }
}

inline Vec4f transform_model_view(Vec3f in, Matrix4 *mat_world, Matrix4 *mat_view) {
    Vec4f out = mat4_multiply_vec4(
        mat_world, 
        vec4_from_vec3(in)
    );

    out = mat4_multiply_vec4(
        mat_view,
        out
    );

    return out;
};


void MeshRendering::Program::project_mesh( ColorBuffer *color_buffer, Matrix4 *mat_world, Matrix4 *mat_view) {
    face_buffer_size = 0;

    int depth_buffer_size = color_buffer->height * color_buffer->width;
    std::fill_n(depth_buffer, depth_buffer_size, -10000);

    int half_width = color_buffer->width / 2;
    int half_height = color_buffer->height / 2;
    float aspect_ratio = (float)(color_buffer->height) / (float)(color_buffer->width);

    Vec4f v_view[3];
    Vec4f v_camera[3];
    // TODO: For debug purposes
    Vec3f initial[3];

    for (int i = 0; i < mesh.face_count; i++) {
        TinyFace *face = &(mesh.faces[i]);

        // model -> view
        for (int j = 0; j < 3; j++) {
            initial[j] = mesh.vertices[face->indices[j]].position;
            v_view[j] = transform_model_view(mesh.vertices[face->indices[j]].position, mat_world, mat_view);
        }

        // SECTION: backface culling
        Vec3f triangle_normal = get_triangle_normal(
            v_view[0],
            v_view[1],
            v_view[2]
        );

        if (render_flags[BACKFACE_CULLING]) {
            auto a = vec3_from_vec4(v_view[0]);
            auto origin = Vec3f {0.0f, 0.0f, 0.0f};

            Vec3f camera_ray = origin - a;

            float dot_normal_cam = Vec3f::dot(camera_ray, triangle_normal);

            if (dot_normal_cam < 0.f) {
                continue;
            }
        }
        // SECTION_END
        // TODO: Check if my matrix works
        auto pr = MatrixPerspective(DEG2RAD * camera_fov, aspect_ratio, -.1f, -1000.f);
        Matrix4 projection = mat_to_mat4(pr);

        for (int j = 0; j < 3; j++) {
            // This gives me image space or NDC
            Vec4f v_projected = mat4_multiply_projection_vec4(projection, v_view[j]);
        
            v_camera[j] = {
                (v_projected.x * half_width) + half_width,
                (v_projected.y * half_height)+ half_height,
                v_projected.z,
                v_projected.w,
            };
        }

        // Write transformed vertices to the buffer
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
        };

        face_buffer[face_buffer_size++] = f;
    }
}

void MeshRendering::Program::render_mesh(ColorBuffer *color_buffer, Light *light) {
    for (int i = 0; i < face_buffer_size; i++) {
        render_triangle(
            color_buffer, 
            depth_buffer,
            &face_buffer[i],
            render_flags,
            light,
            &mesh.diffuse_texture
        );
    }
}

void static render_normals(
    ColorBuffer *color_buffer, 
    FaceBufferItem *face_buffer, 
    float *depth_buffer,
    size_t face_buffer_size
) {


    for (int i = 0; i < face_buffer_size; i++) {
        auto face = &face_buffer[i];
        int depth_buffer_idx = static_cast<int>(face->vertices[0].x + color_buffer->width * face->vertices[0].y);

        if (face->vertices[0].w < depth_buffer[depth_buffer_idx]) {
            continue;
        }
        auto color = tiny_color_from_rgb(
            (1 + face->triangle_normal.x) * 128,
            (1 + face->triangle_normal.y) * 128,
            (1 + face->triangle_normal.z) * 128
        );

        draw_line(
            color_buffer,
            face->vertices[0].x, face->vertices[0].y,
            face->vertices[0].x + face->triangle_normal.x * 5, 
            face->vertices[0].y + face->triangle_normal.y * 5,
            color
        );
    }
}


void MeshRendering::Program::init(int width, int height) {
    char *cube_obj_path = (char *)"assets/cube.obj";
    char *mesh_obj_path = (char *)"assets/headscan.obj";
    char *susan_obj_path = (char *)"assets/susan.obj";
    char *test_obj_path = (char *)"assets/test.obj";

    render_flags.set(DISPLAY_TRIANGLES, 1);
    render_flags.set(DISPLAY_WIREFRAME, 0);
    render_flags.set(BACKFACE_CULLING, 1);
    render_flags.set(ENABLE_Z_BUFFER_CHECK, 1);
    render_flags.set(ENABLE_SHADING, 1);

    std::vector<Vertex> vertices;
    std::vector<TinyFace> faces;

    parse_mesh(mesh_obj_path, &vertices, &faces);
    // parse_mesh(test_obj_path, &vertices, &faces);

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

    mesh.diffuse_texture = LoadImage("assets/headscan-256.png");
    // mesh.diffuse_texture = LoadImage("assets/grid.png");

    camera = {
        .position  = {0.0f, 1.0f, -1.0f},
        .direction = {0.0f, 0.0f, -5.0f}
    };
    light.direction = { 1.0f, -1.0f, -1.0f };


    depth_buffer = (float *)malloc(width * height * sizeof(float));
    face_buffer = (FaceBufferItem *)malloc(FACE_BUFFER_SIZE_LIMIT * sizeof(FaceBufferItem));
}

void MeshRendering::Program::run(ColorBuffer *color_buffer) {
    handle_input();

    float delta = GetFrameTime();
    float elapsed = GetTime();

    // mesh.rotation.y = fmod((elapsed * ROTATION_SPEED), DEG2RAD * 360);

    Matrix4 mat_world = mat4_get_world(
        mesh.scale,
        mesh.rotation,
        mesh.translation
    );

    Matrix4 mat_view = mat4_look_at(
        camera.position,
        camera.direction,
        {0.0f, 1.0f, 0.0f}
    );
  
    project_mesh(color_buffer, &mat_world, &mat_view);

    Vec4f light_direction_projected = mat4_multiply_vec4(
        &mat_view,
        vec4_from_vec3(light.direction, false)
    );

    Light light_pr = { .direction = vec3_from_vec4(light_direction_projected) };

    draw_light_dir(color_buffer, &light_pr);
    draw_debug_quad(color_buffer, &mesh.diffuse_texture, depth_buffer);

    render_mesh(color_buffer, &light_pr);

    if (render_flags[ENABLE_FACE_NORMALS]) {
        render_normals(color_buffer, face_buffer, depth_buffer, face_buffer_size);
    }
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
    if (IsKeyPressed(KEY_D)) {
        render_flags.flip(ENABLE_Z_BUFFER_CHECK);
        std::cout << "Enable z-buffer check: " << render_flags[ENABLE_Z_BUFFER_CHECK] << "\n";
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

    if (IsKeyDown(KEY_RIGHT)) {
        mesh.rotation.y += 0.05;
    }
    if (IsKeyDown(KEY_LEFT)) {
        mesh.rotation.y -= 0.05;
    }
}
    
void MeshRendering::Program::cleanup() {
    // Clean-up for vertices and faces
    free(mesh.vertices);
    free(mesh.faces);
    mesh.vertices = nullptr; // Set the pointer to nullptr after freeing
    mesh.faces = nullptr; // Set the pointer to nullptr after freeing

    free(depth_buffer);
    free(face_buffer);

    UnloadImage(mesh.diffuse_texture);
}

} // namespace pixelscape::MeshRendering
