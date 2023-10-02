#include "camera.h"
#include "matrix.h"
#include "../tooling/logger.h"

PSCameraPerspective perspective_cam_create(
    float fov_y,
    float aspect,
    float z_near,
    float z_far,
    Vec3f position
) {
    log_message(LOG_LEVEL_INFO, "Create perspective camera");

    PSCameraPerspective camera = {};

    camera.position = position;
    camera.initial_direction = {0.0f, 0.0f, -1.0f};

    camera.fov = fov_y;
    camera.aspect = aspect;
    camera.z_near = z_near;
    camera.z_far = z_far;

    camera.yaw = 0.0f;
    camera.pitch = 0.0f;

    return camera;
}

void perspective_cam_update(PSCameraPerspective *camera) {
    // CLEANUP: is this needed? Use initial_direction instead
    Vec3f target = {0.0f, 0.0f, -1.0f};
    Vec3f up = {0.0f, 0.1f, 0.0f};

    Matrix4 rotation_matrix = mat4_get_rotation(camera->pitch, camera->yaw, 0);
    camera->direction = vec3_from_vec4(mat4_multiply_vec4(rotation_matrix, vec4_from_vec3(target)));
    target = camera->position + camera->direction;

    camera->view_matrix = mat4_look_at(
        camera->position,
        target,
        up
    );

    camera->projection_matrix = mat4_get_projection(
        camera->aspect,
        camera->fov,
        camera->z_near,
        camera->z_far);
}
