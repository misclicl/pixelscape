#ifndef __MATRIX_H__
#define __MATRIX_H__

#include "tiny_math.h"

struct Matrix4 {
    float m0, m4, m8,  m12;
    float m1, m5, m9,  m13;
    float m2, m6, m10, m14;
    float m3, m7, m11, m15;
};

Matrix4 mat4_get_identity();

Matrix4 mat4_get_scale(float x, float y, float z);
Matrix4 mat4_get_translation(float x, float y, float z);

Matrix4 mat4_get_rotation_x(float angle);
Matrix4 mat4_get_rotation_y(float angle);
Matrix4 mat4_get_rotation_z(float angle);
Matrix4 mat4_get_rotation(float x, float y, float z);
Matrix4 mat4_get_rotation_z(Vec3f from, Vec3f to, Vec3f up);

Matrix4 mat4_get_world(Vec3f scale, Vec3f rotation, Vec3f translation);
Matrix4 mat4_get_perspective_projection(float aspect_ratio_x, float fov_x, float z_near, float z_far);
Matrix4 mat4_get_orthographic_projection(float left, float right, float bottom, float top, float z_near, float z_far);
Matrix4 mat4_look_at(Vec3f from, Vec3f to, Vec3f up);

Matrix4 mat4_add(Matrix4 *a, Matrix4 *b);
Matrix4 mat4_substract(Matrix4 *a, Matrix4 *b);
Matrix4 mat4_multiply(Matrix4 a, Matrix4 b);

Vec4f mat4_multiply_vec4(Matrix4 mat, Vec4f vec);
Vec4f mat4_multiply_projection_vec4(Matrix4 mat, Vec4f vec);

Matrix4 transpose_matrix(Matrix4 *other);
Matrix4 inverse_matrix(Matrix4 *other);

#endif
