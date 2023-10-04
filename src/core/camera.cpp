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
    camera.yaw = 0.0f;
    camera.pitch = 0.0f;

    camera.z_near = z_near;
    camera.z_far = z_far;

    return camera;
}


PSCameraOthographic orthographic_cam_create(
    float left,
    float right,
    float top,
    float bottom,
    float z_near,
    float z_far,
    Vec3f position
) {
    log_message(LOG_LEVEL_INFO, "Create orthographic camera");

    PSCameraOthographic camera = {};

    camera.position = position;
    camera.initial_direction = {0.0f, 0.0f, 0.0f};

    camera.left = left;
    camera.right = right;
    camera.top = top;
    camera.bottom = bottom;
    camera.z_near = z_near;
    camera.z_far = z_far;

    return camera;
}

void perspective_cam_update(PSCameraPerspective *camera) {
    auto target = camera->initial_direction;
    Vec3f up = {0.0f, 0.1f, 0.0f};

    Matrix4 rotation_matrix = mat4_get_rotation(camera->pitch, camera->yaw, 0);
    camera->direction = vec3_from_vec4(mat4_multiply_vec4(rotation_matrix, vec4_from_vec3(target)));
    target = camera->position + camera->direction;

    camera->view_matrix = mat4_look_at(
        camera->position,
        target,
        up
    );

    camera->projection_matrix = mat4_get_perspective_projection(
        camera->aspect,
        camera->fov,
        camera->z_near,
        camera->z_far);
}

void orthographic_cam_update(PSCameraOthographic *camera) {
    Vec3f target = {0.0f, 0.0f, -1.0f}; // TODO: allow to define the target from the outside
    Vec3f up = {0.0f, 0.1f, 0.0f};

    camera->projection_matrix = mat4_get_orthographic_projection(
        camera->left, camera->right,
        camera->top, camera->bottom,
        camera->z_near, camera->z_far
    );

    camera->view_matrix = mat4_look_at(
        camera->position,
        target,
        up
    );
}
