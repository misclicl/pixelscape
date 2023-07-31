#include "tiny_math.h"

namespace tinyrenderer {
// SECTION: Vector3
template <typename T> Vec3<T>::Vec3() : x(T(0)), y(T(0)), z(T(0)) {}

template <typename T> Vec3<T>::Vec3(const T &xx) : x(xx), y(xx), z(xx) {}

template <typename T> Vec3<T>::Vec3(T xx, T yy, T zz) : x(xx), y(yy), z(zz) {}

template <typename T> T Vec3<T>::length() {
    return sqrt(x * x + y * y + z * z);
}

template <typename T>
T Vec3<T>::dot(Vec3<T> &v) {
    return x * v.x + y * v.y + z * v.z;
}

template <typename T>
Vec3<T> &Vec3<T>::normalize() {
    T len_squared = dot(*this);
    if (len_squared > 0) {
        T invLen = 1 / sqrt(len_squared);
        x *= invLen, y *= invLen, z *= invLen;
    }
    return *this;
}

template <typename T>
Vec3<T> Vec3<T>::cross(Vec3<T> &a, Vec3<T> &b) {
    return Vec3<T>(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

template <typename T>
Vec3<T> Vec3<T>::rotate_y(Vec3<T> *v, float angle) {
    return Vec3<T>{
        v->z * sin(angle) + v->x * cos(angle),
        v->y,
        v->z * cos(angle) - v->x * sin(angle)
    };
}

template <typename T>
Vec3<T> Vec3<T>::rotate_x(Vec3<T> *v, float angle) {
    return Vec3<T>{
        v->x,
        v->y * cos(angle) - v->z * sin(angle),
        v->y * sin(angle) + v->z * cos(angle)
    };
}

template <typename T>
Vec3<T> Vec3<T>::rotate_z(Vec3<T> *v, float angle) {
    return Vec3<T>{
        v->x * cos(angle) - v->y * sin(angle),
        v->x * sin(angle) + v->y * cos(angle),
        v->z
    };
}

template struct Vec3<float>;
// SECTION_END

// SECTION: Vec2

template <typename T> Vec2<T>::Vec2() : x(T(0)), y(T(0)) {};
template <typename T> Vec2<T>::Vec2(T &xx) : x(xx), y(xx) {};
template <typename T> Vec2<T>::Vec2(T xx, T yy) : x(xx), y(yy) {};

template <typename T> T Vec2<T>::length() {
    return sqrt(x * x + y * y);
}

template <typename T>
T Vec2<T>::dot(Vec2<T> &v) {
    return x * v.x + y * v.y;
}

template <typename T>
Vec2<T> &Vec2<T>::normalize() {
    T len_squared = dot(*this);
    if (len_squared > 0) {
        T invLen = 1 / sqrt(len_squared);
        x *= invLen, y *= invLen;
    }
    return *this;
}

template struct Vec2<float>;

} // namespace tinyrenderer

