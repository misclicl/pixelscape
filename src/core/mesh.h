#pragma once

#include "tiny_color.h"
#include "tiny_math.h"
#include <vector>

#define CUBE_VERTICES_COUNT 8
#define CUBE_FACES_COUNT 12

// Stores indices
struct TinyFace {
    int indices[3] = {};
    TinyColor color;
};

// Stores values
struct TinyVertex {
    Vec4f position = {};
    Vec3f normal = {};
    Vec2f texcoords = {};

    bool operator==(TinyVertex other) const {
        return position == other.position &&
               texcoords == other.texcoords &&
               normal == other.normal;
    }
};

struct TinyTriangle {
    TinyVertex vertices[3];
};

extern Vec3f cube_vertices[CUBE_VERTICES_COUNT];
extern TinyFace cube_faces[CUBE_FACES_COUNT];

struct PS_MeshShape {
    TinyFace *faces;
    int face_count = 0;
    int vertex_count = 0;
};

struct TinyMesh {
    TinyVertex *vertices;
    PS_MeshShape *shapes;

    size_t shape_count;

    Vec3f rotation = {};
    Vec3f translation = {};
    Vec3f scale = {1, 1, 1};

    Image diffuse_texture;
    std::vector<Image> textures;
};

TinyMesh *ps_load_mesh(char *mesh_path, std::vector<std::string> texture_paths);
TinyMesh *ps_get_mesh_data();
size_t ps_get_mesh_count();

Vec3f get_triangle_normal(Vec4f vertices[3]);
