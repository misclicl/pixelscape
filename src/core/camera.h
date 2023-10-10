#pragma once

#include "tiny_math.h"
#include "matrix.h"

enum CameraType {
    PERSPECTIVE,
    ORTHOGRAPHIC
};

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
};

struct PSCameraOthographic {
    Vec3f position;
    Vec3f initial_direction;
    Vec3f target;

    float left;
    float right;
    float top;
    float bottom;
    float z_near;
    float z_far;

    Matrix4 view_matrix;
    Matrix4 projection_matrix;
};

PSCameraPerspective perspective_cam_create(
    float fov_y,
    float aspect,
    float z_near,
    float z_far,
    Vec3f position
);
void perspective_cam_update(PSCameraPerspective *camera);

PSCameraOthographic orthographic_cam_create(
    float left,
    float right,
    float top,
    float bottom,
    float z_near,
    float z_far,
    Vec3f position
);
void orthographic_cam_update(PSCameraOthographic *camera);
