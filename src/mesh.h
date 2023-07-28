#ifndef __MESH_H__
#define __MESH_H__

#include <vector>
#include "tiny_math.h"

#define MESH_VERTICES_COUNT 8
#define MESH_FACES_COUNT 12

namespace tinyrenderer {

struct TriangleFace {
    int a;
    int b;
    int c;
};

extern Vec3f vertices[MESH_VERTICES_COUNT];
extern TriangleFace faces[MESH_FACES_COUNT];

struct Mesh {
    std::vector<Vec3f> vertices;
    std::vector<TriangleFace> faces;
};

}
#endif