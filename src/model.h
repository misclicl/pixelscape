#ifndef __MODEL_H__
#define __MODEL_H__

#include "raylib.h"
#include <vector>

namespace pixelscape {
struct Face {
    std::vector<int> vertices;
    std::vector<int> uvs;
};

class Model {
private:
    std::vector<Vector3> verts_;
    std::vector<Face> faces_;
    std::vector<Vector2> uv_coords_;

public:
    Model(const char *filename);
    ~Model();

    int n_verts() const;
    int n_faces() const;
    int n_uv_coords() const;

    Vector3 vert(int idx) const;
    std::vector<int> face_vertices(int idx) const;
    std::vector<int> face_uvs(int idx) const;
    Vector2 uv_coords(int idx) const;
};
} // namespace pixelscape

#endif //__MODEL_H__
