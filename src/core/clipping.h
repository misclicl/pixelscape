#ifndef __CLIPPING__
#define __CLIPPING__

#include "tiny_math.h"
#include "mesh.h"

#define POLYGON_MAX_VERTICES 10
#define POLYGON_MAX_TRIANGLES 8

enum PlaneType {
    CLIPPING_PLANE_LEFT,
    CLIPPING_PLANE_RIGHT,
    CLIPPING_PLANE_TOP,
    CLIPPING_PLANE_BOTTOM,
    CLIPPING_PLANE_NEAR,
    CLIPPING_PLANE_FAR,
};

struct Plane {
    Vec3f position;
    Vec3f normal;
};

void init_clipping_planes_perspective(
    Plane planes[6],
    float fov_horizontal,
    float fov_vertical,
    float z_near,
    float z_far
);

void init_clipping_planes_orthographic(
    Plane planes[6],
    float left,
    float right,
    float top,
    float bottom,
    float z_near,
    float z_far
);

struct TinyPolygon {
    TinyVertex vertices[POLYGON_MAX_VERTICES];
    size_t vertex_count;
};

TinyPolygon polygon_from_triangle(
    Vec4f a, Vec4f b, Vec4f c,
    Vec2f a_uv, Vec2f b_uv, Vec2f c_uv,
    Vec3f a_norm, Vec3f b_norm, Vec3f c_norm
);
void clip_polygon(TinyPolygon *p, Plane planes[6]);
void triangulate_polygon(TinyPolygon *p, TinyTriangle *triangles, size_t *triangles_count);

#endif
