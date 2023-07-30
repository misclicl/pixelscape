#ifndef __TINY_MATH__
#define __TINY_MATH__

#include <cmath>
#include <stdint.h>

namespace tinyrenderer {

template <typename T>
struct Vec2 {
    Vec2();
    Vec2(T &xx);
    Vec2(T xx, T yy);

    T x, y;

    T length();
    T dot(Vec2<T> &v);
    Vec2<T> &normalize();

    Vec2<T> operator+(const Vec2<T> &v) const { return Vec2<T>(x + v.x, y + v.y); }
    Vec2<T> operator-(const Vec2<T> &v) const { return Vec2<T>(x - v.x, y - v.y); }
    Vec2<T> operator*(const T &r) const { return Vec2<T>(x * r, y * r); }
};

typedef Vec2<float> Vec2f;

template <typename T>
struct Vec3 {
    Vec3();
    Vec3(const T &xx);
    Vec3(T xx, T yy, T zz);

    T x, y, z;

    T length();
    T dot(Vec3<T> &v);

    Vec3<T> &normalize();
    Vec3<T> cross(Vec3<T> &v);

    Vec3<T> operator+(Vec3<T> &v) { return Vec3<T>(x + v.x, y + v.y, z + v.z); }
    Vec3<T> operator-(Vec3<T> &v) { return Vec3<T>(x - v.x, y - v.y, z - v.z); }
    Vec3<T> operator*(T &r) { return Vec3<T>(x * r, y * r, z * r); }
    bool operator==(Vec3<T> &v) { return x == v.x && y == v.y && z == v.y; }

    // counter-clockwise when looking from positive to negative
    static Vec3<T> rotate_y(Vec3<T> *v, float angle);
    static Vec3<T> rotate_x(Vec3<T> *v, float angle);
    static Vec3<T> rotate_z(Vec3<T> *v, float angle);
};

typedef Vec3<float> Vec3f;

template <typename T>
struct Matrix44 {
public:
    Matrix44() {}
    const T *operator[](uint8_t i) const { return m[i]; }
    T *operator[](uint8_t i) { return m[i]; }
    // initialize the coefficients of the matrix with the coefficients of the identity matrix
    T m[4][4] = {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}};

    Matrix44 operator*(const Matrix44 &rhs) const {
        Matrix44 mult;
        for (uint8_t i = 0; i < 4; ++i) {
            for (uint8_t j = 0; j < 4; ++j) {
                mult[i][j] = m[i][0] * rhs[0][j] +
                             m[i][1] * rhs[1][j] +
                             m[i][2] * rhs[2][j] +
                             m[i][3] * rhs[3][j];
            }
        }

        return mult;
    }
};

typedef Matrix44<float> Matrix44f;

} // namespace tinyrenderer
#endif
