#ifndef __MODEL_H__
#define __MODEL_H__

#include "raylib.h"
#include <vector>

namespace tinyrenderer {
class Model {
private:
    std::vector<Vector3> verts_;
    std::vector<std::vector<int>> faces_;

public:
    Model(const char *filename);
    ~Model();
    int nverts() const;
    int nfaces() const;
    Vector3 vert(int i) const;
    std::vector<int> face(int idx) const;
};
} // namespace tinyrenderer

#endif //__MODEL_H__
