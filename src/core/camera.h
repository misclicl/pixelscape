#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "tiny_math.h"

struct TinyCamera {
    Vec3f position;
    Vec3f direction;
};

struct TinyFPSCamera {
    Vec3f position;
    Vec3f direction;
    Vec3f forward_velocity;
    Vec3f lateral_velocity;
    float yaw_angle;
    float pitch_angle;
};

#endif
