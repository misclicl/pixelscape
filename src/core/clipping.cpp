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
    TinyPolygon out;
    size_t size = 0;

    Vec3f plane_point = plane.position;
    Vec3f plane_normal = plane.normal;

    Vec4f prev_vertex = polygon->vertices[polygon->vertex_count - 1].position;
    Vec3f prev_normal = polygon->vertices[polygon->vertex_count - 1].normal;
    Vec2f prev_texcoords = polygon->vertices[polygon->vertex_count - 1].texcoords;

    float dot_prev = Vec3f::dot(vec3_from_vec4(prev_vertex) - plane_point, plane_normal);
    float dot_current;

    for (size_t i = 0; i < polygon->vertex_count; i++) {
        Vec4f curr_vertex = polygon->vertices[i].position;
        Vec3f curr_normal = polygon->vertices[i].normal;
        Vec2f curr_texcoords = polygon->vertices[i].texcoords;

        dot_current = Vec3f::dot(vec3_from_vec4(curr_vertex) - plane_point, plane_normal);

        // SECTION: Intersection point
        if (dot_current * dot_prev < 0) {
            // I = dotQ_1 + t(dotQ_2 - dotQ_1)
            float t = dot_prev / (dot_prev - dot_current);

            out.vertices[size++] = {
                (curr_vertex - prev_vertex) * t + prev_vertex,
                (curr_normal - prev_normal) * t + prev_normal,
                (curr_texcoords - prev_texcoords) * t + prev_texcoords,
            };
        }
        // SECTION_END

        if (dot_current > 0) { // current vertex happens to be inside
            out.vertices[size++] = {
                curr_vertex,
                curr_normal,
                curr_texcoords
            };
        }

        dot_prev = dot_current;
        prev_vertex = curr_vertex;
        prev_normal = curr_normal;
        prev_texcoords = curr_texcoords;
    }

    *polygon = out;
    polygon->vertex_count = size;
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

        triangles[i].vertices[0] = polygon->vertices[i0];
        triangles[i].vertices[1] = polygon->vertices[i1];
        triangles[i].vertices[2] = polygon->vertices[i2];
    }

    *triangle_count = polygon->vertex_count - 2;
}

TinyPolygon polygon_from_triangle(
    Vec4f a_pos, Vec4f b_pos, Vec4f c_pos,
    Vec2f a_uv, Vec2f b_uv, Vec2f c_uv,
    Vec3f a_norm, Vec3f b_norm, Vec3f c_norm
) {
    return {
        .vertices = {
            {
                .position = a_pos,
                .texcoords = a_uv,
                .normal = a_norm,
            }, {
                .position = b_pos,
                .texcoords = b_uv,
                .normal = b_norm,
            }, {
                .position = c_pos,
                .texcoords = c_uv,
                .normal = c_norm,
            }
        },
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

