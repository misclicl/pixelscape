#include <bitset>
#include <cmath>
#include <cstdio>
#include <iostream>
#include <stdlib.h>
#include <vector>

#include "raylib.h"

#include "../core/camera.h"
#include "../core/clipping.h"
#include "../core/display.h"
#include "../core/matrix.h"
#include "../core/tiny_color.h"
#include "../core/tiny_math.h"

#include "../tooling/logger.h"
#include "../tooling/render_debug_text.h"

#include "mesh_rendering.h"
#include "raymath.h"
#include "renderer.h"

// TODO: Prevent crashing if overflows
#define FACE_BUFFER_SIZE_LIMIT 250000
#define ROTATION_SPEED 0.2f
#define Z_NEAR -0.1f
#define Z_FAR -30.0f

static float camera_fov_y = 59 * DEG2RAD;
static float aspect_ratio = 1;
static TinyColor default_color = 0xafafafff;
static Color light_color = {200, 20, 180, 255};

static TinyFPSCamera camera_fps;
static Plane clipping_planes[6];

static void render_triangle(
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

inline Vec4f transform_model_view(Vec4f in, Matrix4 *mat_world, Matrix4 *mat_view) {
    Vec4f out = mat4_multiply_vec4(
        *mat_world,
        in
    );

    out = mat4_multiply_vec4(
        *mat_view,
        out
    );

    return out;
};


void Program::project_mesh(TinyMesh *mesh, size_t index, ColorBuffer *color_buffer, Matrix4 *mat_world, Matrix4 *mat_view) {
    size_t face_buffer_idx = 0;

    int target_half_width = color_buffer->width / 2;
    int target_half_height = color_buffer->height / 2;

    Vec4f v_view[3];
    Vec3f normals[3];

    auto mat_projection = mat4_get_projection(aspect_ratio, camera_fov_y, Z_NEAR, Z_FAR);
    auto mat_inversed = inverse_matrix(mat_view);
    Matrix4 mat_view_tr_inv = transpose_matrix(&mat_inversed);

    for (int i = 0; i < mesh->shapes[index].face_count; i++) {
        TinyFace *face = &(mesh->shapes[index].faces[i]);

        // model -> view
        for (int j = 0; j < 3; j++) {
            v_view[j] = transform_model_view(mesh->vertices[face->indices[j]].position, mat_world, mat_view);
            normals[j] = vec3_from_vec4(mat4_multiply_vec4(
                mat_view_tr_inv,
                vec4_from_vec3(mesh->vertices[face->indices[j]].normal, false)
            ));
        }

        // SECTION: backface culling
        auto triangle_normal = get_triangle_normal(v_view);

        if (renderer_state.flags[CULL_BACKFACE]) {
            Vec3f camera_ray = -vec3_from_vec4(v_view[0]);
            float dot_normal_cam = Vec3f::dot(camera_ray, triangle_normal);

            if (dot_normal_cam < 0.f) {
                continue;
            }
        }
        // SECTION_END

        // TODO: define in the begining and write data here right away
        TinyPolygon polygon = polygon_from_triangle(
            v_view[0],
            v_view[1],
            v_view[2],
            mesh->vertices[face->indices[0]].texcoords,
            mesh->vertices[face->indices[1]].texcoords,
            mesh->vertices[face->indices[2]].texcoords,
            normals[0],
            normals[1],
            normals[2]
        );

        // Clip the polygon
        clip_polygon(&polygon, clipping_planes);
        // Triangulate the polygon
        TinyTriangle triangles[POLYGON_MAX_TRIANGLES] = {};
        size_t triangle_count = 0;

        triangulate_polygon(&polygon, triangles, &triangle_count);

        for (int t_idx = 0; t_idx < triangle_count; t_idx++) {
            for (int v_idx = 0; v_idx < 3; v_idx++) {
                // This gives me the image space or NDC
                Vec4f v_projected = mat4_multiply_projection_vec4(mat_projection, triangles[t_idx].vertices[v_idx].position);

                // In-camera view
                triangles[t_idx].vertices[v_idx].position = {
                    (v_projected.x * target_half_width) + target_half_width,
                    (v_projected.y * target_half_height) + target_half_height,
                    v_projected.z, // z must be in NDC
                    v_projected.w,
                };
            }

            face_buffer[face_buffer_idx++] = triangles[t_idx];
        }
    }

    face_buffer_size = face_buffer_idx;
}

void Program::render_mesh(ColorBuffer *color_buffer, size_t idx, Light *light, TinyMesh *mesh) {
    for (int i = 0; i < face_buffer_size; i++) {
        render_triangle(
            color_buffer,
            depth_buffer,
            &face_buffer[i],
            &renderer_state,
            light,
            &mesh->textures[idx]
        );
    }
}

void Program::init(int width, int height) {
    char *cube_obj_path = (char *)"assets/cube.obj";
    char *headscan_obj_path = (char *)"assets/headscan.obj";

    char *medic_obj_path = (char *)"assets/medic.obj";
    char *p_body_obj_path = (char *)"assets/p-body.obj";

    char *susan_obj_path = (char *)"assets/susan.obj";
    char *jacket_obj_path = (char *)"assets/jacket.obj";

    renderer_state.flags.set(DRAW_TRIANGLES, 1);
    renderer_state.flags.set(DRAW_WIREFRAME, 0);
    renderer_state.flags.set(CULL_BACKFACE, 1);
    renderer_state.flags.set(USE_Z_BUFFER, 1);
    renderer_state.flags.set(USE_SHADING, 1);

    std::vector<std::string> medic_textures = {
        "assets/medic-eyeball.png",
        "assets/medic-eyeball.png",
        "assets/medic-face-diffuse.png",
        "assets/medic-body-diffuse.png",
    };
    std::vector<std::string> p_body_textures = {
        "assets/p-body/shell-2.png",
    };

    auto medic_mesh = ps_load_mesh(medic_obj_path, medic_textures);
    auto p_body_mesh = ps_load_mesh(p_body_obj_path, p_body_textures);

    medic_mesh->translation.y = -1.0f;
    medic_mesh->translation.x = -1.0f;

    p_body_mesh->translation.y = -1.0f;
    p_body_mesh->translation.x = 1.0f;

    camera_fps = {
        .position = {0.0, 0.0f, 3.0f},
        .direction = {0.0, 0.0f, -1.0f},
        .forward_velocity = {0.0, 0.0f, 0.0f},
        .yaw_angle = 0.0f,
        .pitch_angle = 0.0f
    };

    // light.direction = { 1.0f, -1.0f, -1.0f };
    light.direction = { 0.0f, -1.0f, -1.0f };

    depth_buffer = depth_buffer_create(width, height, -10000);
    if (!depth_buffer) {
        log_message(LogLevel::LOG_LEVEL_ERROR, "Failed to create depth buffer");
    }

    face_buffer = (TinyTriangle *)malloc(FACE_BUFFER_SIZE_LIMIT * sizeof(TinyTriangle));

    aspect_ratio = static_cast<float>(height) / width;
    float aspect_ratio_x = 1 / aspect_ratio;
    float camera_fov_x = 2 * atan(tan(camera_fov_y / 2) * aspect_ratio_x);

    init_clipping_planes(
        clipping_planes,
        camera_fov_x,
        camera_fov_y,
        Z_NEAR,
        Z_FAR
    );
}

void Program::run(ColorBuffer *color_buffer) {
    float delta = GetFrameTime();
    float elapsed = GetTime();

    handle_input(delta);

    depth_buffer_clear(depth_buffer, -10000);

    auto mesh_data = ps_get_mesh_data();
    auto mesh_count = ps_get_mesh_count();

    Vec3f up= {0.0f, 1.0f, 0.0f};
    Vec3f target = {0.0f, 0.0f, -1.0f};

    Matrix4 camera_rotation = mat4_get_rotation(camera_fps.pitch_angle, camera_fps.yaw_angle, 0);
    camera_fps.direction = vec3_from_vec4(mat4_multiply_vec4(camera_rotation, vec4_from_vec3(target)));
    target = camera_fps.position + camera_fps.direction;

    Matrix4 mat_view = mat4_look_at(
        camera_fps.position,
        target,
        {0.0f, 1.0f, 0.0f}
    );
    Vec4f light_direction_projected = mat4_multiply_vec4(
        mat_view,
        vec4_from_vec3(light.direction, false)
    );
    Light light_pr = { .direction = vec3_from_vec4(light_direction_projected) };

    Matrix4 mat_world;
    for (size_t i = 0; i < mesh_count; i++) {
        auto mesh = &mesh_data[i];

        mat_world = mat4_get_world(
            mesh->scale,
            mesh->rotation,
            mesh->translation
        );

        for (int i = 0; i < mesh->shape_count; i++) {
            Program::project_mesh(mesh, i, color_buffer, &mat_world, &mat_view);
            render_mesh(color_buffer, i, &light_pr, mesh);
        }
    }
}

void Program::handle_input(float delta_time) {
    // SECTION: Render modes
    if (IsKeyPressed(KEY_C)) {
        renderer_state.flags.flip(CULL_BACKFACE);
        log_message(
            LogLevel::LOG_LEVEL_DEBUG,
            "Backface culling: %d",
            static_cast<int>(renderer_state.flags[CULL_BACKFACE])
        );
    }
    if (IsKeyPressed(KEY_N)) {
        renderer_state.flags.flip(USE_FACE_NORMALS);
        log_message(
            LogLevel::LOG_LEVEL_DEBUG,
            "Face normals: %d",
            static_cast<int>(renderer_state.flags[USE_FACE_NORMALS])
        );
    }
    if (IsKeyPressed(KEY_ZERO)) {
        renderer_state.flags.flip(USE_SHADING);
        log_message(
            LogLevel::LOG_LEVEL_DEBUG,
            "Enable shading: %d",
            static_cast<int>(renderer_state.flags[USE_SHADING])
        );
    }
    if (IsKeyPressed(KEY_NINE)) {
        renderer_state.flags.flip(USE_Z_BUFFER);
        log_message(
            LogLevel::LOG_LEVEL_DEBUG,
            "z-buffer check: %d",
            static_cast<int>(renderer_state.flags[USE_Z_BUFFER])
        );
    }
    if (IsKeyPressed(KEY_EIGHT)) {
        renderer_state.flags.flip(DRAW_DEPTH_BUFFER);
        log_message(
            LogLevel::LOG_LEVEL_DEBUG,
            "draw z-buffer: %d",
            static_cast<int>(renderer_state.flags[DRAW_DEPTH_BUFFER])
        );
    }

    if (IsKeyPressed(KEY_ONE)) {
        renderer_state.flags.flip(DRAW_VERTICES);
        log_message(LogLevel::LOG_LEVEL_DEBUG, "Display vertices: %d", static_cast<int>(renderer_state.flags[DRAW_VERTICES]));
    }
    if (IsKeyPressed(KEY_TWO)) {
        renderer_state.flags.flip(DRAW_WIREFRAME);
        log_message(LogLevel::LOG_LEVEL_DEBUG, "Display wireframe: %d", static_cast<int>(renderer_state.flags[DRAW_WIREFRAME]));
    }
    if (IsKeyPressed(KEY_THREE)) {
        renderer_state.flags.flip(DRAW_TRIANGLES);
        std::cout << "Display triangles: " << renderer_state.flags[DRAW_TRIANGLES] << "\n";
    }

    // SECTION: Camera rotation
    if (IsKeyDown(KEY_LEFT)) {
        camera_fps.yaw_angle += 1.0f * delta_time;
    }
    if (IsKeyDown(KEY_RIGHT)) {
        camera_fps.yaw_angle -= 1.0f * delta_time;
    }
    if (IsKeyDown(KEY_DOWN)) {
        camera_fps.pitch_angle -= 1.0f * delta_time;
    }
    if (IsKeyDown(KEY_UP)) {
        camera_fps.pitch_angle += 1.0f * delta_time;
    }

    // SECTION: Camera position
    float camera_movement_speed = 1.0f;

    if (IsKeyDown(KEY_W)) {
        camera_fps.forward_velocity = camera_fps.direction * camera_movement_speed * delta_time;
        camera_fps.position = camera_fps.position + camera_fps.forward_velocity;
    }
    if (IsKeyDown(KEY_D)) {
        Vec3f up = {0, 1, 0};
        auto camera_right = Vec3f::cross(camera_fps.direction, up);
        camera_fps.lateral_velocity = camera_right * camera_movement_speed * delta_time;
        camera_fps.position = camera_fps.position + camera_fps.lateral_velocity;
    }
    if (IsKeyDown(KEY_A)) {
        Vec3f up = {0, 1, 0};
        auto camera_right = Vec3f::cross(camera_fps.direction, up);
        camera_fps.lateral_velocity = camera_right * -1.0 * camera_movement_speed * delta_time;
        camera_fps.position = camera_fps.position + camera_fps.lateral_velocity;
    }
    if (IsKeyDown(KEY_S)) {
        camera_fps.forward_velocity = camera_fps.direction * -1.0f * camera_movement_speed * delta_time;
        camera_fps.position = camera_fps.position + camera_fps.forward_velocity;
    }
    if (IsKeyDown(KEY_E)) {
        camera_fps.position.y += camera_movement_speed * delta_time;
    }
    if (IsKeyDown(KEY_Q)) {
        camera_fps.position.y -= camera_movement_speed * delta_time;
    }

    if (IsKeyPressed(KEY_T)) {
        if (renderer_state.texture_filter_mode == TextureFilterMode::NEAREST_NEIGHBOR) {
            renderer_state.texture_filter_mode = TextureFilterMode::BILINEAR;
        } else {
            renderer_state.texture_filter_mode = TextureFilterMode::NEAREST_NEIGHBOR;
        }

        log_message(
            LogLevel::LOG_LEVEL_DEBUG, "Texture filtering %d",
            static_cast<int>(renderer_state.texture_filter_mode)
        );
    }
}

void Program::cleanup() {
    depth_buffer_destroy(depth_buffer);
    free(face_buffer);
}
