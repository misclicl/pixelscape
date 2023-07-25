#include <array>
#include <cmath>

#include "perspective_projection.h"

#include "raylib.h"
#include "display.h"
#include "tiny_math.h"

// Reference:
// https://www.scratchapixel.com/lessons/3d-basic-rendering/rasterization-practical-implementation/projection-stage.html
// if link's broken, there's a local copy

Vector3 to_screen_space(Vector3 &v, float near, float fov, float aspect) {
    Vector3 out;

    // float tanHalfFov = tan(fov / 2.0f);
    
    // out.x = v.x / (-v.z * tanHalfFov * aspect);
    // out.y = v.y / (-v.z * tanHalfFov);
    out.x = near * v.x / -v.z;
    out.y = near * v.y / -v.z;
    out.z = -v.z;
    return out;
}

Vector3 to_ndc_space(Vector3 &v) {
    Vector3 out;
    tinyrenderer::Vec3f v1;

    float r = 1;
    float l = -r;
    float t = 1;
    float b = -t;

    out.x = 2 * v.x / (r - l) - (r + l) / (r - l);
    out.y = 2 * v.y / (t - b) - (t + b) / (t - b);
    out.z = v.z;

    return out;
}

Vector3 to_raster(Vector3 &v, int width, int height) {
    Vector3 out;

    out.x = (v.x + 1) / 2 * width;
    out.y = (1 - v.y) / 2 * height;
    out.z = -v.z;
    return out;
}

void view_2_raster(Vector3 &v, float near, int width, int height) {
    // Vector3 v_screen = to_screen_space(v, near);
    Vector3 v_screen = to_screen_space(v, near, 30.f, 1.f);
    Vector3 v_ndc = to_ndc_space(v_screen);
    Vector3 v_raster = to_raster(v_ndc, width, height);
    v = v_raster;
}

void tinyrenderer::program::PerspectiveProjection::run(float near, int width, int height) {
    float half_size = 10.f;
    float front = -15.f;
    float far = front - half_size * 2;
   
    std::array<Vector3, 8> vs = {
        // near left
        Vector3 { -half_size, -half_size, -front },
        Vector3 { -half_size, half_size, -front },
        // near right
        Vector3 { half_size, -half_size, -front },
        Vector3 { half_size, half_size, -front },
        // far left
        Vector3 { -half_size, -half_size, -far },
        Vector3 { -half_size, half_size, -far },
        // far right
        Vector3 { half_size, -half_size, -far },
        Vector3 { half_size, half_size, -far }
    };

    for (auto &v: vs) {
        view_2_raster(v, near, width, height);
    }

    // near left
    tinyrenderer::draw_line(vs[0], vs[1], PINK);
    // near top
    tinyrenderer::draw_line(vs[1], vs[3], PINK);
    // near right
    tinyrenderer::draw_line(vs[2], vs[3], PINK);
    // near bottom
    tinyrenderer::draw_line(vs[0], vs[2], PINK);


    
    tinyrenderer::draw_line(vs[4], vs[5], PINK);
    tinyrenderer::draw_line(vs[5], vs[7], PINK);
    tinyrenderer::draw_line(vs[6], vs[7], PINK);
    tinyrenderer::draw_line(vs[4], vs[6], PINK);

    tinyrenderer::draw_axis(width, height);
}
