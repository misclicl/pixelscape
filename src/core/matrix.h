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

Matrix4 mat4_add(Matrix4 *a, Matrix4 *b);
Matrix4 mat4_substract(Matrix4 *a, Matrix4 *b);
Matrix4 mat4_multiply(Matrix4 *a, Matrix4 *b);

Vec4f mat4_multiply_vec4(Matrix4 *mat, Vec4f vec);

#endif
