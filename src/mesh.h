#ifndef __MESH_H__
#define __MESH_H__

#include "core/tiny_color.h"
#include "core/tiny_math.h"
#include <vector>

#define CUBE_VERTICES_COUNT 8
#define CUBE_FACES_COUNT 12

namespace tinyrenderer {

struct TinyFace {
    int indices[3] = {};
    TinyColor color;
    float avg_depth = 0;
};

extern Vec3f cube_vertices[CUBE_VERTICES_COUNT];
extern TinyFace cube_faces[CUBE_FACES_COUNT];

struct Vertex {
    Vec3f *position = {};
    Vec3f *normals = {};
    Vec2f *uv = {};

    bool operator==(const Vertex &other) const {
        return *position == *(other.position);
    }
};

struct Mesh {
    Vertex *vertices;
    TinyFace *faces;

    int face_count = 0;
    int vertex_count = 0;
    Vec3f rotation = {};
};

} // namespace tinyrenderer

#endif
