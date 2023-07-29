#ifndef __MESH_H__
#define __MESH_H__

#include "tiny_math.h"
#include <vector>

#define CUBE_VERTICES_COUNT 8
#define CUBE_FACES_COUNT 12

namespace tinyrenderer {

struct TriangleFace {
    int a;
    int b;
    int c;
};

extern Vec3f cube_vertices[CUBE_VERTICES_COUNT];
extern TriangleFace cube_faces[CUBE_FACES_COUNT];

struct Vertex;

struct Mesh {
    Vec3f *vertices;
    TriangleFace *faces;

    int face_count = 0;
    int vertex_count = 0;
    Vec3f rotation = {};
};

} // namespace tinyrenderer
#endif