#include "matrix.h"

Matrix4 mat4_get_identity() {
    return {
        .m0 = 1, .m4 = 0, .m8  = 0, .m12 = 0,
        .m1 = 0, .m5 = 1, .m9  = 0, .m13 = 0,
        .m2 = 0, .m6 = 0, .m10 = 1, .m14 = 0,
        .m3 = 0, .m7 = 0, .m11 = 0, .m15 = 1,
    };
}

Matrix4 mat4_get_scale(float x, float y, float z) {
    Matrix4 identity = mat4_get_identity();

    identity.m0 = x;
    identity.m5 = y;
    identity.m10 = z;

    return identity;
}

Matrix4 mat4_get_translation(float x, float y, float z) {
    // | 1  0  0  x |
    // | 0  1  0  y |
    // | 0  0  1  z |
    // | 0  0  0  1 |
    Matrix4 identity = mat4_get_identity();
    identity.m12 = x;
    identity.m13 = y;
    identity.m14 = z;

    return identity;
}

Matrix4 mat4_get_rotation_x(float angle) {
    // | 1       0        0  0 |
    // | 0  cos(a)  -sin(a)  0 |
    // | 0  sin(a)   cos(a)  0 |
    // | 0       0        0  1 |
    Matrix4 identity = mat4_get_identity();
    float c = cos(angle);
    float s = sin(angle);

    identity.m5  =  c;
    identity.m6  =  s;
    identity.m9  = -s;
    identity.m10 =  c;

    return identity;
}

Matrix4 mat4_get_rotation_y(float angle) {
    // |  cos(a)  0   sin(a)  0 |
    // |       0  1        0  0 |
    // | -sin(a)  0   cos(a)  0 |
    // |       0  0        0  1 |
    Matrix4 identity = mat4_get_identity();
    float c = cos(angle);
    float s = sin(angle);

    identity.m0  =  c;
    identity.m2  = -s;
    identity.m8  =  s;
    identity.m10 =  c;

    return identity;
}

Matrix4 mat4_get_rotation_z(float angle) {
    // | cos(a)  -sin(a)  0  0 |
    // | sin(a)   cos(a)  0  0 |
    // |      0        0  1  0 |
    // |      0        0  0  1 |
    Matrix4 identity = mat4_get_identity();
    float c = cos(angle);
    float s = sin(angle);

    identity.m0 =   c;
    identity.m1 =   s;
    identity.m4 =  -s;
    identity.m5 =   c;

    return identity;
}

Matrix4 mat4_get_rotation(float x, float y, float z) {
    // | cos(b)cos(g)  sin(a)sin(b)cos(g) - cos(a)sin(g)  cos(a)sin(b)cos(g) + sin(a)sin(y)  0 |
    // | cos(b)sin(g)  sin(a)sin(b)sin(g) + cos(a)cos(y)  cos(a)sin(b)sin(g) - sin(a)cos(g)  0 |
    // |      -sin(b)                       sin(a)cos(b)                       cos(a)cos(b)  0 |
    // |            0                                  0                                  0  1 |
    float c_alpha = cos(x);
    float s_alpha = sin(x);

    float c_beta = cos(y);
    float s_beta = sin(y);

    float c_gamma = cos(z);
    float s_gamma = sin(z);

    Matrix4 identity = mat4_get_identity();
    identity.m0 = c_beta * c_gamma;
    identity.m1 = c_beta * s_gamma;
    identity.m2 = -s_beta;

    identity.m4 = s_alpha * s_beta * c_gamma - c_alpha * s_gamma;
    identity.m5 = s_alpha * s_beta * s_gamma + c_alpha * c_gamma;
    identity.m6 = s_alpha * c_beta;

    identity.m8 = c_alpha * s_beta * c_gamma + s_alpha * s_gamma;
    identity.m9 = c_alpha * s_beta * s_gamma - s_alpha * c_gamma;
    identity.m10 = c_alpha * c_beta;

    return identity;
}

Matrix4 mat4_add(Matrix4 *a, Matrix4 *b) {
    return {
        .m0 = a-> m0 + b -> m0,
        .m1 = a-> m1 + b -> m1,
        .m2 = a-> m2 + b -> m2,
        .m3 = a-> m3 + b -> m3,
        .m4 = a-> m4 + b -> m4,
        .m5 = a-> m5 + b -> m5,
        .m6 = a-> m6 + b -> m6,
        .m7 = a-> m7 + b -> m7,
        .m8 = a-> m8 + b -> m8,
        .m9 = a-> m9 + b -> m9,
        .m10 = a-> m10 + b -> m10,
        .m11 = a-> m11 + b -> m11,
        .m12 = a-> m12 + b -> m12,
        .m13 = a-> m13 + b -> m13,
        .m14 = a-> m14 + b -> m14,
        .m15 = a-> m15 + b -> m15,
    };
};

Matrix4 mat4_subtract(Matrix4 *a, Matrix4 *b) {
    return {
        .m0 = a-> m0 - b -> m0,
        .m1 = a-> m1 - b -> m1,
        .m2 = a-> m2 - b -> m2,
        .m3 = a-> m3 - b -> m3,
        .m4 = a-> m4 - b -> m4,
        .m5 = a-> m5 - b -> m5,
        .m6 = a-> m6 - b -> m6,
        .m7 = a-> m7 - b -> m7,
        .m8 = a-> m8 - b -> m8,
        .m9 = a-> m9 - b -> m9,
        .m10 = a-> m10 - b -> m10,
        .m11 = a-> m11 - b -> m11,
        .m12 = a-> m12 - b -> m12,
        .m13 = a-> m13 - b -> m13,
        .m14 = a-> m14 - b -> m14,
        .m15 = a-> m15 - b -> m15,
    };
};

Matrix4 mat4_multiply(Matrix4 a, Matrix4 b) {
    return {
        .m0 =  a.m0*b.m0  + a.m1*b.m4  + a.m2*b.m8   + a.m3*b.m12,
        .m1 =  a.m0*b.m1  + a.m1*b.m5  + a.m2*b.m9   + a.m3*b.m13,
        .m2 =  a.m0*b.m2  + a.m1*b.m6  + a.m2*b.m10  + a.m3*b.m14,
        .m3 =  a.m0*b.m3  + a.m1*b.m7  + a.m2*b.m11  + a.m3*b.m15,
        .m4 =  a.m4*b.m0  + a.m5*b.m4  + a.m6*b.m8   + a.m7*b.m12,
        .m5 =  a.m4*b.m1  + a.m5*b.m5  + a.m6*b.m9   + a.m7*b.m13,
        .m6 =  a.m4*b.m2  + a.m5*b.m6  + a.m6*b.m10  + a.m7*b.m14,
        .m7 =  a.m4*b.m3  + a.m5*b.m7  + a.m6*b.m11  + a.m7*b.m15,
        .m8 =  a.m8*b.m0  + a.m9*b.m4  + a.m10*b.m8  + a.m11*b.m12,
        .m9 =  a.m8*b.m1  + a.m9*b.m5  + a.m10*b.m9  + a.m11*b.m13,
        .m10 = a.m8*b.m2  + a.m9*b.m6  + a.m10*b.m10 + a.m11*b.m14,
        .m11 = a.m8*b.m3  + a.m9*b.m7  + a.m10*b.m11 + a.m11*b.m15,
        .m12 = a.m12*b.m0 + a.m13*b.m4 + a.m14*b.m8  + a.m15*b.m12,
        .m13 = a.m12*b.m1 + a.m13*b.m5 + a.m14*b.m9  + a.m15*b.m13,
        .m14 = a.m12*b.m2 + a.m13*b.m6 + a.m14*b.m10 + a.m15*b.m14,
        .m15 = a.m12*b.m3 + a.m13*b.m7 + a.m14*b.m11 + a.m15*b.m15,
    };
}

Vec4f mat4_multiply_vec4(Matrix4 *mat, Vec4f v) {
    Vec4f result = {
        .x = mat->m0 * v.x + mat->m4 * v.y + mat->m8 * v.z + mat->m12 * v.w,
        .y = mat->m1 * v.x + mat->m5 * v.y + mat->m9 * v.z + mat->m13 * v.w,
        .z = mat->m2 * v.x + mat->m6 * v.y + mat->m10 * v.z + mat->m14 * v.w,
        .w = mat->m3 * v.x + mat->m7 * v.y + mat->m11 * v.z + mat->m15 * v.w,
    };

    return result;
};
