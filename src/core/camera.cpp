#include "camera.h"
#include "matrix.h"
#include "../tooling/logger.h"

static void init_camera(
    PSCamera *camera, 
    CameraType type,
    Vec3f position,
    float z_near,
    float z_far
) {
    camera->type = type;

    camera->position = position;
    camera->z_near = z_near;
    camera->z_far = z_far;
}

PSCamera create_camera_perspective(
    Vec3f position,
    float fov_y,
    float aspect,
    float z_near,
    float z_far
) {
    log_message(LOG_LEVEL_INFO, "Create perspective camera");

    PSCamera camera = {};

    init_camera(
        &camera, CameraType::PERSPECTIVE,
        position,
        z_near, z_far
    );

    camera.persp.initial_direction = {0.0f, 0.0f, -1.0f};

    camera.persp.fov = fov_y;
    camera.persp.aspect = aspect;
    camera.persp.yaw = 0.0f;
    camera.persp.pitch = 0.0f;

    return camera;
}

PSCamera create_camera_orthographic(
    Vec3f position,
    float left,
    float right,
    float top,
    float bottom,
    float z_near,
    float z_far
) {
    log_message(LOG_LEVEL_INFO, "Create orthographic camera");

    PSCamera camera = {};

    init_camera(
        &camera, CameraType::PERSPECTIVE,
        position,
        z_near, z_far
    );

    camera.ortho.target = {0.0f, 0.0f, 0.0f};

    camera.ortho.left = left;
    camera.ortho.right = right;
    camera.ortho.top = top;
    camera.ortho.bottom = bottom;

    return camera;
}

void update_camera(PSCamera *camera) {
    Vec3f target; 

    switch (camera->type) {
        case CameraType::PERSPECTIVE: {
            target = camera->persp.initial_direction;

            Matrix4 rotation_matrix = mat4_get_rotation(
               camera->persp.pitch, camera->persp.yaw, 0
            );

            camera->persp.direction = vec3_from_vec4(mat4_multiply_vec4(rotation_matrix, vec4_from_vec3(target)));
            target = camera->position + camera->persp.direction;

            camera->projection_matrix = mat4_get_perspective_projection(
                camera->persp.aspect,
                camera->persp.fov,
                camera->z_near,
                camera->z_far
            );

            break;
        }
        case CameraType::ORTHOGRAPHIC: {
            camera->projection_matrix = mat4_get_orthographic_projection(
                camera->ortho.left, camera->ortho.right,
                camera->ortho.top, camera->ortho.bottom,
                camera->z_near, camera->z_far
            );

            target = camera->ortho.target;
            break;
        }
    }

    Vec3f up = {0.0f, 0.1f, 0.0f};
    camera->view_matrix = mat4_look_at(
        camera->position,
        target,
        up
    );
}

