#ifndef __TINY_MATH__
#define __TINY_MATH__

#include <cmath>
#include <stdint.h>

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

    Vec3<T> operator+(Vec3<T> &v) { return Vec3<T>(x + v.x, y + v.y, z + v.z); }
    Vec3<T> operator-(Vec3<T> &v) { return Vec3<T>(x - v.x, y - v.y, z - v.z); }
    Vec3<T> operator*(T r) { return Vec3<T>(x * r, y * r, z * r); }
    bool operator==(Vec3<T> &v) { return x == v.x && y == v.y && z == v.y; }

    // counter-clockwise when looking from positive to negative
    static Vec3<T> rotate_y(Vec3<T> *v, float angle);
    static Vec3<T> rotate_x(Vec3<T> *v, float angle);
    static Vec3<T> rotate_z(Vec3<T> *v, float angle);
    static Vec3<T> cross(Vec3<T> &a, Vec3<T> &b);
    static T dot(Vec3<T> &a, Vec3<T> &b);
};

typedef Vec3<float> Vec3f;

struct Vec4f {
    float x, y, z, w;
};

Vec4f vec4_from_vec3(Vec3f v);

#endif
