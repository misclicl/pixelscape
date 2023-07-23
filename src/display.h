#include <cstdlib>
#include <vector>

#include "raylib.h"

namespace tinyrenderer {
void draw_line(Vector3 p0, Vector3 p1, Color color);

void draw_triangle_wireframe(Vector3 p0, Vector3 p1, Vector3 p2, Color color);

void draw_triangles(
    std::vector<Vector3> &vertices,
    std::vector<std::array<int, 3>> &t_indices, 
    int screen_width,
    int screen_heigth,
    Color color
);

void draw_triangle(
    const Vector3 *vertices,
    const Vector2 (&uv_coords)[3],
    Image &diffuse_texture,
    const float intencity, 
    float *zbuffer,

    int screen_width,
    int screen_height
);
}
