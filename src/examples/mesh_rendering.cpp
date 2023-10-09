#include <bitset>
#include <cmath>
#include <cstdio>
#include <iostream>
#include <stdlib.h>
#include <vector>

#include "raylib.h"

#include "../core/camera.h"
#include "../core/shader.h"
#include "../core/clipping.h"
#include "../core/display.h"
#include "../core/matrix.h"
#include "../core/tiny_math.h"
#include "../core/ps_array.h"

#include "../tooling/logger.h"
#include "../tooling/render_debug_text.h"

#include "mesh_rendering.h"
#include "raymath.h"
#include "renderer.h"

// TODO: Prevent crashing if overflows
#define FACE_BUFFER_SIZE_LIMIT 250000
#define ROTATION_SPEED 0.2f
#define Z_NEAR -0.1f
#define Z_FAR -300.0f

static float camera_fov_y = 60 * DEG2RAD;
static float aspect_ratio = 1; // Gets calculated from color buffer size later
static Color default_color = DARKGRAY;
static Color light_color = {200, 20, 180, 255};

static DepthBuffer *depth_buffer_light;

static PSCameraPerspective camera_perspective;
static Plane clipping_planes_persp[6];

static PSCameraOthographic camera_orthographic;
static Plane clipping_planes_ortho[6];

inline float clamp(float val, float min_val, float max_val) {
    if (val < min_val) return min_val;
    if (val > max_val) return max_val;
    return val;
}

// TODO: this should live in the gl program state
struct Uniforms {
    Vec3f light_dir;
    Matrix4 light_vp;
};


static Uniforms uniforms = {};

Color fragment_shader_depth(void *data, void *uniforms) {
    FragmentData* fd = static_cast<FragmentData*>(data);
    float depth_color_float = fd->depth * 255;
    uint8_t depth_color = round(clamp(depth_color_float, 0.0, 255.0));

    return { depth_color, 0, 0, 255 };
}


Color fragment_shader_main(void* data, void *_uniforms) {
    FragmentData* fd = static_cast<FragmentData*>(data);
    Vec3f normal = fd->normal;
    float depth_value = fd->depth;
    float x = fd->x;
    float y = fd->y;
    float z = fd->z;

    auto half_height = depth_buffer_light->height / 2;
    auto half_width = depth_buffer_light->width / 2;

    int shadow_x = x * half_width;
    int shadow_y = y * half_height;

    shadow_x += half_width;
    shadow_y += half_height;

    auto value = buffer_pixel_get(depth_buffer_light, shadow_x, shadow_y);

    float bias = 0.01;

    if (value != nullptr && *value - bias > z) {
       return DARKGRAY;
    }

    float brightness = -Vec3f::dot(uniforms.light_dir.normalize(), normal);
    return ColorBrightness(default_color, brightness);
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


static void project_mesh(
    TinyMesh *mesh,
    size_t shape_idx,
    ColorBuffer *color_buffer,
    DepthBuffer * depth_buffer,
    Matrix4 *mat_world,
    Matrix4 *mat_view,
    CameraType camera_type,
    FragmentShader fragment_shader
) {
    size_t face_buffer_idx = 0;

    int target_half_width = color_buffer->width / 2;
    int target_half_height = color_buffer->height / 2;

    Vec4f v_projected[3];
    Vec3f normals[3];

    auto mat_inversed = inverse_matrix(mat_view);
    Matrix4 mat_view_tr_inv = transpose_matrix(&mat_inversed);

    for (int i = 0; i < mesh->shapes[shape_idx].face_count; i++) {
        TinyFace *face = &(mesh->shapes[shape_idx].faces[i]);

        for (int j = 0; j < 3; j++) {
            auto view = transform_model_view(mesh->vertices[face->indices[j]].position, mat_world, mat_view);

            Vec4f projected;

            if (camera_type == CameraType::PERSPECTIVE) {
                projected = mat4_multiply_projection_vec4(
                    camera_perspective.projection_matrix,
                    view);
            } else {
                projected = mat4_multiply_projection_vec4(
                    camera_orthographic.projection_matrix,
                    view);
            }

            v_projected[j] = {
                (projected.x * target_half_width) + target_half_width,
                (projected.y * target_half_height) + target_half_height,
                projected.z, // z must be in NDC
                projected.w,
            };


            // not important
            normals[j] = vec3_from_vec4(mat4_multiply_vec4(
                mat_view_tr_inv,
                vec4_from_vec3(mesh->vertices[face->indices[j]].normal, false)
            ));
        }

        TinyTriangle triangle = {
            .vertices = {
                {
                    .position = v_projected[0],
                    .texcoords = mesh->vertices[face->indices[0]].texcoords,
                    .normal = normals[0]
                },
                {
                    .position = v_projected[1],
                    .texcoords = mesh->vertices[face->indices[1]].texcoords,
                    .normal = normals[1]
                },
                {
                    .position = v_projected[2],
                    .texcoords = mesh->vertices[face->indices[2]].texcoords,
                    .normal = normals[2]
                }
            }
        };

        TinyTriangle triangle_original = {
            .vertices = {
                {
                    .position = mesh->vertices[face->indices[0]].position,
                },
                {
                    .position = mesh->vertices[face->indices[1]].position,
                },
                {
                    .position = mesh->vertices[face->indices[2]].position,
                }
            }
        };

        // light thing
        for (int v_idx = 0; v_idx < 3; v_idx++) {
            auto temp = transform_model_view(mesh->vertices[face->indices[v_idx]].position, mat_world, &camera_orthographic.view_matrix);
            auto temp2 = mat4_multiply_projection_vec4(
                    camera_orthographic.projection_matrix,
                    temp);

            triangle_original.vertices[v_idx].position = temp2;
        }

        depth_test(
            camera_type,
            color_buffer,
            depth_buffer,
            &triangle,
            &triangle_original,
            fragment_shader
        );
    }
}

void Program::init(int width, int height) {
    this->render_texture = LoadRenderTexture(width, height);
    BeginTextureMode(render_texture);
    ClearBackground(PINK);
    EndTextureMode();

    char *cube_obj_path = (char *)"assets/cube.obj";
    char *plane_obj_path = (char *)"assets/plane.obj";
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
    // std::vector<std::string> p_body_textures = {
    //     "assets/p-body/shell-2.png",
    // };

    auto medic_mesh = ps_load_mesh(medic_obj_path, medic_textures);
    std::vector<std::string> plane_textures = {};
    auto plane_mesh = ps_load_mesh(plane_obj_path, plane_textures);
    // auto p_body_mesh = ps_load_mesh(p_body_obj_path, p_body_textures);


    aspect_ratio = static_cast<float>(height) / width;

    auto view_width = 5.0;
    camera_orthographic = orthographic_cam_create(
        view_width, -view_width,
        -view_width * aspect_ratio, view_width * aspect_ratio,
        Z_NEAR,
        -15.0,
        {1.0, 3.0, 3.0}
    );

    camera_perspective = perspective_cam_create(
        camera_fov_y,
        aspect_ratio,
        Z_NEAR,
        Z_FAR,
        {0.0, 1.0f, 10.0f}
    );

    light.direction = { 1.0f, -1.0f, -0.5f };

    depth_buffer_light = depth_buffer_create(width, height, -10000);
    depth_buffer = depth_buffer_create(width, height, -10000);
    if (!depth_buffer) {
        log_message(LogLevel::LOG_LEVEL_ERROR, "Failed to create depth buffer");
    }

    face_buffer = (TinyTriangle *)malloc(FACE_BUFFER_SIZE_LIMIT * sizeof(TinyTriangle));

    float aspect_ratio_x = 1 / aspect_ratio;
    float camera_fov_x = 2 * atan(tan(camera_fov_y / 2) * aspect_ratio_x);

    init_clipping_planes_perspective(
        clipping_planes_persp,
        camera_fov_x,
        camera_fov_y,
        camera_perspective.z_near,
        camera_perspective.z_far
    );

    init_clipping_planes_orthographic(
        clipping_planes_ortho,
        2.0, -2.0,
        -2.0, 2.0,
        camera_orthographic.z_near,
        camera_orthographic.z_far
    );
}

void Program::update(ColorBuffer *color_buffer) {
    color_buffer->clear(BLACK);

    float delta = GetFrameTime();
    float elapsed = GetTime();

    handle_input(delta);

    depth_buffer_clear(depth_buffer, -10000);
    depth_buffer_clear(depth_buffer_light, -10000);

    auto mesh_data = ps_get_mesh_data();
    auto mesh_count = ps_get_mesh_count();

    Vec3f up= {0.0f, 1.0f, 0.0f};
    Vec3f target = {0.0f, 0.0f, -1.0f};

    perspective_cam_update(&camera_perspective);
    orthographic_cam_update(&camera_orthographic);

    Vec4f light_direction_projected = mat4_multiply_vec4(
        camera_perspective.view_matrix,
        vec4_from_vec3(light.direction, false)
    );

    uniforms.light_dir = vec3_from_vec4(light_direction_projected);
    uniforms.light_vp = mat4_multiply(camera_orthographic.projection_matrix, camera_orthographic.view_matrix);

    // Depth pass
    Matrix4 mat_world;
    for (size_t i = 0; i < mesh_count; i++) {
        auto mesh = &mesh_data[i];

        mat_world = mat4_get_world(
            mesh->scale,
            mesh->rotation,
            mesh->translation
        );

        for (int i = 0; i < mesh->shape_count; i++) {
            project_mesh(
                mesh,
                i,
                color_buffer,
                depth_buffer_light,
                &mat_world,
                &camera_orthographic.view_matrix,
                CameraType::ORTHOGRAPHIC,
                fragment_shader_depth
            );
        }
    }

    // Main camera
    for (size_t i = 0; i < mesh_count; i++) {
        auto mesh = &mesh_data[i];

        mat_world = mat4_get_world(
            mesh->scale,
            mesh->rotation,
            mesh->translation
        );

        // TODO: Im pretty sure I could do this loop inside project mesh, right?
        // or pass down the shape instead
        for (int shape_idx = 0; shape_idx < mesh->shape_count; shape_idx++) {
            project_mesh(
                mesh,
                shape_idx,
                color_buffer,
                depth_buffer,
                &mat_world,
                &camera_perspective.view_matrix,
                CameraType::PERSPECTIVE, // TODO: camera type can be recognised from the camera itsef
                fragment_shader_main
            );
       }
    }

    BeginTextureMode(render_texture);
    UpdateTexture(render_texture.texture, color_buffer->pixels);
    EndTextureMode();
}

void Program::draw(ColorBuffer *color_buffer) {
    BeginDrawing();
    DrawTexturePro(
        render_texture.texture,
        Rectangle{0, 0, static_cast<float>(color_buffer->width), -static_cast<float>(color_buffer->height)},
        Rectangle{0, 0, static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight())},
        Vector2{0, 0},
        0,
        WHITE
    );
    EndDrawing();
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
        log_message(
            LogLevel::LOG_LEVEL_DEBUG,
            "Display vertices: %d", static_cast<int>(renderer_state.flags[DRAW_VERTICES])
        );
    }
    if (IsKeyPressed(KEY_TWO)) {
        renderer_state.flags.flip(DRAW_WIREFRAME);
        log_message(
            LogLevel::LOG_LEVEL_DEBUG,
            "Display wireframe: %d", static_cast<int>(renderer_state.flags[DRAW_WIREFRAME])
        );
    }
    if (IsKeyPressed(KEY_THREE)) {
        renderer_state.flags.flip(DRAW_TRIANGLES);
        std::cout << "Display triangles: " << renderer_state.flags[DRAW_TRIANGLES] << "\n";
    }

    // SECTION: Camera rotation
    if (IsKeyDown(KEY_LEFT)) {
        camera_perspective.yaw += 1.0f * delta_time;
    }
    if (IsKeyDown(KEY_RIGHT)) {
        camera_perspective.yaw -= 1.0f * delta_time;
    }
    if (IsKeyDown(KEY_DOWN)) {
        camera_perspective.pitch -= 1.0f * delta_time;
    }
    if (IsKeyDown(KEY_UP)) {
        camera_perspective.pitch += 1.0f * delta_time;
    }

    // // SECTION: Camera position
    float camera_movement_speed = 1.0f;
    Vec3f up = {0, 1, 0};

    if (IsKeyDown(KEY_W)) {
        // camera_orthographic.position =  Vec3f { 0.0, 0.0, -camera_movement_speed * delta_time}
        //     + camera_orthographic.position;
        auto forward_velocity = camera_perspective.direction * camera_movement_speed * delta_time;
        camera_perspective.position = camera_perspective.position + forward_velocity;
    }
    if (IsKeyDown(KEY_D)) {
        auto camera_right = Vec3f::cross(camera_perspective.direction, up);
        auto lateral_velocity = camera_right * camera_movement_speed * delta_time;
        camera_perspective.position = camera_perspective.position + lateral_velocity;
    }
    if (IsKeyDown(KEY_A)) {
        auto camera_right = Vec3f::cross(camera_perspective.direction, up);
        auto lateral_velocity = camera_right * -1.0 * camera_movement_speed * delta_time;
        camera_perspective.position = camera_perspective.position + lateral_velocity;
    }
    if (IsKeyDown(KEY_S)) {
        // camera_orthographic.position = Vec3f { 0.0, 0.0, camera_movement_speed * delta_time }
        //     + camera_orthographic.position;
        auto forward_velocity = camera_perspective.direction * -1.0f
            * camera_movement_speed * delta_time;
        camera_perspective.position = camera_perspective.position + forward_velocity;
    }
    if (IsKeyDown(KEY_E)) {
        // camera_orthographic.position.y += camera_movement_speed * delta_time;
        camera_perspective.position.y += camera_movement_speed * delta_time;
    }
    if (IsKeyDown(KEY_Q)) {
        // camera_orthographic.position.y -= camera_movement_speed * delta_time;
        camera_perspective.position.y -= camera_movement_speed * delta_time;
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
    UnloadRenderTexture(this->render_texture);
    depth_buffer_destroy(depth_buffer);
    free(face_buffer);
}
