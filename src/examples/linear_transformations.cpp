#include <vector>
#include <array>

#include "raylib.h"
#include "raymath.h"

#include "../core/display.h"

namespace tinyrenderer {

float cube_halfsize = 50;
// FIXME: put somewhere else. it's in global space
Vector3 vertx[8] = {
    Vector3 { -cube_halfsize, -cube_halfsize, 0},
    Vector3 {  cube_halfsize, -cube_halfsize, 0},
    Vector3 { -cube_halfsize,  cube_halfsize, 0},
    Vector3 {  cube_halfsize,  cube_halfsize, 0},
};

void linear_transformations(ColorBuffer *color_buffer) {
    std::vector<std::array<int, 3>> indices;
    std::vector<Vector3> v_out;

    indices.push_back({0, 1, 2});
    indices.push_back({1, 2, 3});
    for (const Vector3 &vertex : vertx) {
        v_out.push_back(vertex);
    }
    // draw_triangles(
    //     color_buffer,
    //     v_out, 
    //     indices,
    //     0xFFFFFFFF
    // );

    // .707 = cos(45deg) = sin(45deg)
    Matrix m = {
        1, -.707, 0, 0,
        0,     1, 0, 0,
        0,     0, 1, 0,
        0,     0, 0, 0,
    };

    v_out.clear();
    for (const Vector3 &vertex : vertx) {
        v_out.push_back(Vector3Transform(vertex, m));
    }
    // tinyrenderer::draw_triangles(
    //     color_buffer,
    //     v_out, 
    //     indices,
    //     0xFF0000FF
    // );

    m.m1 = .707;
    v_out.clear();
    for (const Vector3 &vertex : vertx) {
        v_out.push_back(Vector3Transform(vertex, m));
    }
    // tinyrenderer::draw_triangles(
    //     color_buffer,
    //     v_out, 
    //     indices,
    //     0x00FF00FF
    // );

    m.m0 = .707;
    m.m5 = .707;
    v_out.clear();
    for (const Vector3 &vertex : vertx) {
        v_out.push_back(Vector3Transform(vertex, m));
    }
    // tinyrenderer::draw_triangles(
    //     color_buffer,
    //     v_out,
    //     indices,
    //     0x0000FFFF
    // );
}
}

