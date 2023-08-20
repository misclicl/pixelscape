#include "clipping.h"

void init_clipping_planes(
    Plane planes[6],
    float fov_horizontal,
    float fov_vertical,
    float z_near,
    float z_far
) {
    float half_fov_v = fov_vertical / 2;
    float half_fov_h = fov_horizontal / 2;

    Vec3f origin = {};

    planes[CLIPPING_PLANE_RIGHT] = {
        .position = origin,
        .normal = { -cos(half_fov_h), 0, -sin(half_fov_h) }
    };
    planes[CLIPPING_PLANE_LEFT] = {
        .position = origin,
        // TODO: mirror right plane?
        .normal = { cos(half_fov_h), 0, -sin(half_fov_h) }
    };

    planes[CLIPPING_PLANE_TOP] = {
        .position = origin,
        .normal = { 0, -cos(half_fov_v), -sin(half_fov_v) }
    };
    planes[CLIPPING_PLANE_BOTTOM] = {
        .position = origin,
        .normal = { 0, cos(half_fov_v), -sin(half_fov_v) }
    };

    planes[CLIPPING_PLANE_NEAR] = {
        .position = { 0.0f, 0.0f, z_near },
        .normal = { 0.0f, 0.0f, -1.0f}
    };
    planes[CLIPPING_PLANE_FAR] = {
        .position = { 0.0f, 0.0f, z_far },
        .normal = { 0.0f, 0.0f, 1.0f}
    };
}

static void clip_polygon_against_plane(TinyPolygon *polygon, Plane plane) {
    Vec3f in[POLYGON_MAX_VERTICES];
    size_t in_size = 0;

    Vec3f plane_point = plane.position;
    Vec3f plane_normal = plane.normal;

    Vec3f curr_vertex;
    Vec3f prev_vertex = polygon->vertices[polygon->vertex_count - 1];

    float dot_prev = 42;
    float dot_current;

    for (size_t i = 0; i < polygon->vertex_count; i++) {
        curr_vertex = polygon->vertices[i];
        // prev_vertex = polygon->vertices[i - 1];

        if (dot_prev == 42) {
            dot_prev = Vec3f::dot(prev_vertex - plane_point, plane_normal);
        }

        dot_current = Vec3f::dot(curr_vertex - plane_point, plane_normal);

        // SECTION: Intersection point
        if (dot_current * dot_prev < 0) {
            // I = dotQ_1 + t(dotQ_2 - dotQ_1)
            float t = dot_prev / (dot_prev - dot_current);
            Vec3f intersection_point = (curr_vertex - prev_vertex) * t + prev_vertex;

            in[in_size++] = intersection_point;
        }
        // SECTION_END

        if (dot_current > 0) { // current vertex happens to be inside
            in[in_size++] = curr_vertex;
        }

        dot_prev = dot_current;
        prev_vertex = curr_vertex;
    }

    // Write result to polygon
    for (size_t i = 0; i < in_size; i++) {
        polygon->vertices[i] = in[i];
    }

    polygon->vertex_count = in_size;
    // printf("%d\n", polygon->vertex_count);
}

void triangulate_polygon(
    TinyPolygon *polygon,
    TinyTriangle *triangles,
    size_t *triangle_count
) {
    if (polygon->vertex_count == 0) {
        return;
    }

    for (size_t i = 0; i < polygon->vertex_count - 2; i++) {
        size_t i0 = 0;
        size_t i1 = i + 1;
        size_t i2 = i + 2;

        triangles[i].points[0] = polygon->vertices[i0];
        triangles[i].points[1] = polygon->vertices[i1];
        triangles[i].points[2] = polygon->vertices[i2];

    }
    *triangle_count = polygon->vertex_count - 2;
}

TinyPolygon polygon_from_triangle(Vec3f a, Vec3f b, Vec3f c) {
    return {
        .vertices = { a, b, c },
        .vertex_count = 3
    };
}

void clip_polygon(TinyPolygon *polygon, Plane clipping_planes[6]) {
    clip_polygon_against_plane(polygon, clipping_planes[CLIPPING_PLANE_LEFT]);
    clip_polygon_against_plane(polygon, clipping_planes[CLIPPING_PLANE_RIGHT]);
    clip_polygon_against_plane(polygon, clipping_planes[CLIPPING_PLANE_TOP]);
    clip_polygon_against_plane(polygon, clipping_planes[CLIPPING_PLANE_BOTTOM]);
    clip_polygon_against_plane(polygon, clipping_planes[CLIPPING_PLANE_NEAR]);
    clip_polygon_against_plane(polygon, clipping_planes[CLIPPING_PLANE_FAR]);
}

