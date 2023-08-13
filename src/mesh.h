#ifndef __MESH_H__
#define __MESH_H__

#include "core/tiny_color.h"
#include "core/tiny_math.h"
#include <vector>

#define CUBE_VERTICES_COUNT 8
#define CUBE_FACES_COUNT 12

namespace pixelscape {

// Stores indices
struct TinyFace {
    int indices[3] = {};

    TinyColor color;
};

extern Vec3f cube_vertices[CUBE_VERTICES_COUNT];
extern TinyFace cube_faces[CUBE_FACES_COUNT];

// Stores values
struct Vertex {
    Vec3f position = {};
    Vec3f normals = {};
    Vec2f texcoords = {};

    bool operator==(Vertex other) const {
        return position == (other.position) &&
               texcoords == other.texcoords;
    }
};

struct Mesh {
    Vertex *vertices;
    TinyFace *faces;

    int face_count = 0;
    int vertex_count = 0;

    Vec3f rotation = {};
    Vec3f translation = {};
    Vec3f scale = {1, 1, 1};

    Image diffuse_texture;
};

} // namespace pixelscape

#endif
