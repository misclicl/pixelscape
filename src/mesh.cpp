#include "mesh.h"

namespace tinyrenderer {

/*
 *                                  |
 *                                  |/
 *            0 -------- 1     x ---|----
 *           /|         /|         /|
 *          / |        / |        z |
 *         3----------2  |          |
 *         |  |       |  |          y
 *         |  4-------|--5
 *         | /        | /
 *         |/         |/
 *         7----------6
 *
 */

Vec3f cube_vertices[CUBE_VERTICES_COUNT] = {
    // # 0
    { 1, -1, -1}, // 0
    {-1, -1, -1}, // 1
    {-1, -1,  1}, // 2
    { 1, -1,  1}, // 3

    { 1,  1, -1}, // 4
    {-1,  1, -1}, // 5
    {-1,  1,  1}, // 6
    { 1,  1,  1}, // 7
};

TriangleFace cube_faces[CUBE_FACES_COUNT] = {
    // Using OpenGL order
    // top
    {0, 1, 2},
    {0, 2, 3},
    // bottom
    {7, 6, 5},
    {7, 5, 4},
    // front
    {3, 2, 6},
    {3, 6, 7},
    // back
    {1, 0, 4},
    {1, 4, 5},
    // left
    {0, 3, 7},
    {0, 7, 4},
    // right
    {2, 1, 5},
    {2, 5, 6}};
} // namespace tinyrenderer
