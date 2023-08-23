#ifndef __TINY_MATH__
#define __TINY_MATH__

#include <cmath>
#include <stdint.h>

template <typename T>
struct Vec2 {
    Vec2() : x(0), y(0) {};
    Vec2(T xx) : x(xx), y(0) {};
    Vec2(T xx, T yy) : x(xx), y(yy) {};

    T x, y;
    // Vec2();
    // Vec2(T &xx);
    // Vec2(T xx, T yy);
    //
    // T x, y;

    T length();
    T dot(Vec2<T> &v);
    Vec2<T> &normalize();

    Vec2<T> operator+(Vec2<T> &v) { return {x + v.x, y + v.y}; }
    Vec2<T> operator-(Vec2<T> &v) { return {x - v.x, y - v.y}; }
    Vec2<T> operator-() { return {- x, -y }; }
    Vec2<T> operator*(T r) { return {x * r, y * r}; }
    bool operator==(Vec2<T> other) { return x == other.x && y == other.y; }
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
    Vec3<T> operator-() { return Vec3<T>(- x, -y, -z); }
    Vec3<T> operator*(T r) { return Vec3<T>(x * r, y * r, z * r); }
    bool operator==(Vec3<T> v) { return x == v.x && y == v.y && z == v.y; }

    // counter-clockwise when looking from positive to negative
    static Vec3<T> rotate_y(Vec3<T> *v, float angle);
    static Vec3<T> rotate_x(Vec3<T> *v, float angle);
    static Vec3<T> rotate_z(Vec3<T> *v, float angle);
    static Vec3<T> cross(Vec3<T> &a, Vec3<T> &b);
    // static T dot(Vec3<T> &a, Vec3<T> &b);
    static T dot(Vec3<T> a, Vec3<T> b);
};

typedef Vec3<float> Vec3f;

// NOTE: I should probably remove templates from Vec3 and Vec2
// as I never use them for anything but floats. All it does is
// adds complexity
struct Vec4f {
    float x, y, z, w;

    float length();
    Vec4f operator+(Vec4f &v) { return {x + v.x, y + v.y, z + v.z, w + v.w}; }
    Vec4f operator-(Vec4f &v) { return {x - v.x, y - v.y, z - v.z, w - v.w}; }
    Vec4f operator-() { return {- x, -y, -z, -w}; }
    Vec4f operator*(float r) { return {x * r, y * r, z * r, w * r}; }
    bool operator==(Vec4f v) { return x == v.x && y == v.y && z == v.y && w == v.w; }

    Vec4f &normalize();
};

Vec4f vec4_from_vec3(Vec3f v);
Vec4f vec4_from_vec3(Vec3f v, bool is_point);
Vec3f vec3_from_vec4(Vec4f v);

#endif
