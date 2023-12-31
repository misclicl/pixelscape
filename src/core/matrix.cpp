#include <doctest/doctest.h>

#include "matrix.h"

// Following right-handed coordinate system here, similar to OpenGL
//
//                 ^ y
//                 |
//                 |
//                 |  / -z
//                 | /
//                 |/
//   -x -----------*------------> x
//                /|
//               / |
//              /  |
//           z /   |
//                 |
//                 | -y
//


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

Matrix4 mat4_get_perspective_projection(float aspect_ratio, float fov, float z_near, float z_far) {
    // | ar * (1/tan(theta/2))               0        0                 0 |
    // |                     0  1/tan(theta/2)        0                 0 |
    // |                     0               0  -lambda  -lambda * z_near |
    // |                     0               0       -1                 0 |
    float lambda = (z_far + z_near) / (z_far - z_near);
    float scale = (1 / tan(fov/2));
    Matrix4 out = {};

    out.m0 = aspect_ratio * scale;
    out.m5 = scale;
    out.m11 = -1;

    out.m10 = lambda;

    float depth_offset = (-2 * z_far * z_near) / (z_far - z_near);
    out.m14 = depth_offset;

    return out;
}

Matrix4 mat4_get_orthographic_projection(float left, float right, float bottom, float top, float z_near, float z_far) {
    Matrix4 out = {};

    float lr = 1 / (right - left);
    float bt = 1 / (top - bottom);
    float nf = 1 / (z_near - z_far);

    out.m0 = 2 * lr;
    out.m5 = 2 * bt;
    out.m10 = 2 * nf;

    out.m12 = -(right + left) * lr;
    out.m13 = -(top + bottom) * bt;
    out.m14 = -(z_far + z_near) * nf;
    out.m15 = 1.0f;

    return out;
}

Matrix4 mat4_look_at(Vec3f from, Vec3f to, Vec3f up_gl) {
    // | R_x  R_y  R_z  -dot(R, F) |
    // | U_x  U_y  U_z  -dot(U, F) |
    // | F_x  F_y  F_z  -dot(F, F) |
    // | T_x  T_y  T_z           1 |

    Vec3f forward = from - to;
    forward = forward.normalize();

    Vec3f right = Vec3f::cross(up_gl, forward);
    right = right.normalize();

    Vec3f up = Vec3f::cross(forward, right);
    Matrix4 out = mat4_get_identity();

    out.m0 = right.x;
    out.m1 = up.x;
    out.m2 = forward.x;

    out.m4 = right.y;
    out.m5 = up.y;
    out.m6 = forward.y;

    out.m8  = right.z;
    out.m9  = up.z;
    out.m10 = forward.z;

    out.m12 = -Vec3f::dot(right, from);
    out.m13 = -Vec3f::dot(up, from);
    out.m14 = -Vec3f::dot(forward, from);

    return out;
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

Vec4f mat4_multiply_vec4(Matrix4 mat, Vec4f v) {
    Vec4f result = {
        .x = mat.m0 * v.x + mat.m4 * v.y + mat.m8 * v.z + mat.m12 * v.w,
        .y = mat.m1 * v.x + mat.m5 * v.y + mat.m9 * v.z + mat.m13 * v.w,
        .z = mat.m2 * v.x + mat.m6 * v.y + mat.m10 * v.z + mat.m14 * v.w,
        .w = mat.m3 * v.x + mat.m7 * v.y + mat.m11 * v.z + mat.m15 * v.w,
    };

    return result;
};

Vec4f mat4_multiply_projection_vec4(Matrix4 mat, Vec4f vec) {
    Vec4f out = mat4_multiply_vec4(mat, vec);

    if (out.w != 0.f) {
        out.x /= out.w;
        out.y /= out.w;
        out.z /= out.w;
    }

    return out;
}

Matrix4 mat4_get_world(Vec3f scale, Vec3f rotation, Vec3f translation) {
    Matrix4 m_scale = mat4_get_scale(
        scale.x,
        scale.y,
        scale.z
    );

    Matrix4 m_translation = mat4_get_translation(
        translation.x,
        translation.y,
        translation.z
    );

    Matrix4 m_rotation = mat4_get_rotation(
        rotation.x,
        rotation.y,
        rotation.z
    );

    Matrix4 out = mat4_multiply(m_scale, mat4_multiply(m_rotation, m_translation));
    return out;
}

Matrix4 transpose_matrix(Matrix4 *other) {
    return {
        other->m0,  other->m1,  other->m2,  other->m3,
        other->m4,  other->m5,  other->m6,  other->m7,
        other->m8,  other->m9,  other->m10, other->m11,
        other->m12, other->m13, other->m14, other->m15
    };
};

Matrix4 inverse_matrix(Matrix4 *other) {
    float m0 = other->m0;
    float m1 = other->m1;
    float m2 = other->m2;
    float m3 = other->m3;
    float m4 = other->m4;
    float m5 = other->m5;
    float m6 = other->m6;
    float m7 = other->m7;
    float m8 = other->m8;
    float m9 = other->m9;
    float m10 = other->m10;
    float m11 = other->m11;
    float m12 = other->m12;
    float m13 = other->m13;
    float m14 = other->m14;
    float m15 = other->m15;

    Matrix4 inv;

    inv.m0  = m5 * m10 * m15 - m5 * m11 * m14 - m9 * m6 * m15 + m9 * m7 * m14 + m13 * m6 * m11 - m13 * m7 * m10;
    inv.m4  = -m4 * m10 * m15 + m4 * m11 * m14 + m8 * m6 * m15 - m8 * m7 * m14 - m12 * m6 * m11 + m12 * m7 * m10;
    inv.m8  = m4 * m9 * m15 - m4 * m11 * m13 - m8 * m5 * m15 + m8 * m7 * m13 + m12 * m5 * m11 - m12 * m7 * m9;
    inv.m12 = -m4 * m9 * m14 + m4 * m10 * m13 + m8 * m5 * m14 - m8 * m6 * m13 - m12 * m5 * m10 + m12 * m6 * m9;
    inv.m1  = -m1 * m10 * m15 + m1 * m11 * m14 + m9 * m2 * m15 - m9 * m3 * m14 - m13 * m2 * m11 + m13 * m3 * m10;
    inv.m5  = m0 * m10 * m15 - m0 * m11 * m14 - m8 * m2 * m15 + m8 * m3 * m14 + m12 * m2 * m11 - m12 * m3 * m10;
    inv.m9  = -m0 * m9 * m15 + m0 * m11 * m13 + m8 * m1 * m15 - m8 * m3 * m13 - m12 * m1 * m11 + m12 * m3 * m9;
    inv.m13 = m0 * m9 * m14 - m0 * m10 * m13 - m8 * m1 * m14 + m8 * m2 * m13 + m12 * m1 * m10 - m12 * m2 * m9;
    inv.m2  = m1 * m6 * m15 - m1 * m7 * m14 - m5 * m2 * m15 + m5 * m3 * m14 + m13 * m2 * m7 - m13 * m3 * m6;
    inv.m6  = -m0 * m6 * m15 + m0 * m7 * m14 + m4 * m2 * m15 - m4 * m3 * m14 - m12 * m2 * m7 + m12 * m3 * m6;
    inv.m10 = m0 * m5 * m15 - m0 * m7 * m13 - m4 * m1 * m15 + m4 * m3 * m13 + m12 * m1 * m7 - m12 * m3 * m5;
    inv.m14 = -m0 * m5 * m14 + m0 * m6 * m13 + m4 * m1 * m14 - m4 * m2 * m13 - m12 * m1 * m6 + m12 * m2 * m5;
    inv.m3  = -m1 * m6 * m11 + m1 * m7 * m10 + m5 * m2 * m11 - m5 * m3 * m10 - m9 * m2 * m7 + m9 * m3 * m6;
    inv.m7  = m0 * m6 * m11 - m0 * m7 * m10 - m4 * m2 * m11 + m4 * m3 * m10 + m8 * m2 * m7 - m8 * m3 * m6;
    inv.m11 = -m0 * m5 * m11 + m0 * m7 * m9 + m4 * m1 * m11 - m4 * m3 * m9 - m8 * m1 * m7 + m8 * m3 * m5;
    inv.m15 = m0 * m5 * m10 - m0 * m6 * m9 - m4 * m1 * m10 + m4 * m2 * m9 + m8 * m1 * m6 - m8 * m2 * m5;

    float det = m0 * inv.m0 + m1 * inv.m4 + m2 * inv.m8 + m3 * inv.m12;

    if (det == 0) {
        // Matrix is singular; cannot be inverted.
        // Here you can either return the matrix unchanged or handle this situation in another way.
        return *other;
    }

    det = 1.0 / det;

    for (int i = 0; i < 16; i++) {
        (&inv.m0)[i] *= det;
    }

    return inv;
}

// TODO: Try to write tests here:
TEST_CASE("mat4_look_at basic test") {
    Vec3f up{ 0, 1, 0 };
    Vec3f from{ 1, 1, 1 };
    Vec3f to{ 0, 0, 0 };

    // Matrix4 result = mat4_look_at(from, to, up);

    // Now assert that the resulting matrix has expected values.
    // CHECK(result.m0  == doctest::Approx(0.707107f));
    // CHECK(result.m4  == doctest::Approx(0.0f));
    // CHECK(result.m8  == doctest::Approx(-0.707107f));
    // CHECK(result.m12 == doctest::Approx(0.0f));
    //
    // CHECK(result.m1  == doctest::Approx(-0.408248));
    // CHECK(result.m5  == doctest::Approx(0.816497));
    // CHECK(result.m9  == doctest::Approx(-0.408248));
    // CHECK(result.m13 == doctest::Approx(0.0f));
    //
    // CHECK(result.m2  == doctest::Approx(0.57735f));
    // CHECK(result.m6  == doctest::Approx(0.57735f));
    // CHECK(result.m10 == doctest::Approx(0.57735f));
    // CHECK(result.m14 == doctest::Approx(0.f));
    //
    // CHECK(result.m3  == doctest::Approx(1.0f));
    // CHECK(result.m7  == doctest::Approx(1.0f));
    // CHECK(result.m11 == doctest::Approx(1.0f));
    // CHECK(result.m15 == doctest::Approx(1.0f));
}

