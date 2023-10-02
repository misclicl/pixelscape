#pragma once

#include "tiny_math.h"
#include "matrix.h"

struct PSCameraPerspective {
    Vec3f position;
    Vec3f initial_direction;
    Vec3f direction;

    float fov;
    float aspect;
    float z_near;
    float z_far;
    float yaw;
    float pitch;

    Matrix4 view_matrix;
    Matrix4 projection_matrix;
    // Matrix4 inverse_view_matrix;
    // Matrix4 inverse_projection_matrix;
};

struct TinyFPSCamera {
    Vec3f position;
    Vec3f direction;
    Vec3f forward_velocity;
    Vec3f lateral_velocity;
    float yaw_angle;
    float pitch_angle;
};

PSCameraPerspective perspective_cam_create(
    float fov_y,
    float aspect,
    float z_near,
    float z_far,
    Vec3f position
);

void perspective_cam_update(PSCameraPerspective *camera);
// void perspective_cam_update(PSCameraPerspective *camera);
