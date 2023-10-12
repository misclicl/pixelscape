#pragma once

#include "tiny_math.h"
#include "matrix.h"

enum CameraType {
    PERSPECTIVE,
    ORTHOGRAPHIC
};

struct PSCameraPerspective {
    Vec3f initial_direction;
    Vec3f direction;

    float fov;
    float aspect;
    float yaw;
    float pitch;
};

struct PSCameraOthographic {
    Vec3f target;

    float left;
    float right;
    float top;
    float bottom;
};

struct PSCamera {
    CameraType type;

    Vec3f position;

    float z_near;
    float z_far;

    Matrix4 view_matrix;
    Matrix4 projection_matrix;

    union {
        PSCameraPerspective persp;
        PSCameraOthographic ortho;
    };
};


PSCamera create_camera_orthographic(
    Vec3f position,
    float left, float right,
    float top, float bottom,
    float z_near, float z_far
);

PSCamera create_camera_perspective(
    Vec3f position,
    float fov_y, float aspect,
    float z_near, float z_far
);

void update_camera(PSCamera *camera);

