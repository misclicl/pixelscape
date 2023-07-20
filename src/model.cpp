#include "model.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

tinyrenderer::Model::Model(const char *filename) : verts_(), faces_() {
    std::ifstream in;
    in.open (filename, std::ifstream::in);
    if (in.fail()) return;
    std::string line;
    while (!in.eof()) {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;
        if (!line.compare(0, 2, "v ")) {
            iss >> trash;
            Vector3 v;
            iss >> v.x;
            iss >> v.y;
            iss >> v.z;
            verts_.push_back(v);
        } else if (!line.compare(0, 2, "f ")) {
            std::vector<int> f;
            int itrash, idx;
            iss >> trash;
            while (iss >> idx >> trash >> itrash >> trash >> itrash) {
                idx--; // in wavefront obj all indices start at 1, not zero
                f.push_back(idx);
            }
            faces_.push_back(f);
        }
    }
}

tinyrenderer::Model::~Model() {
}

int tinyrenderer::Model::Model::nverts() const {
    return (int)verts_.size();
}

int tinyrenderer::Model::Model::nfaces() const {
    return (int)faces_.size();
}

std::vector<int> tinyrenderer::Model::Model::face(int idx) const {
    return faces_[idx];
}

Vector3 tinyrenderer::Model::Model::vert(int i) const {
    return verts_[i];
}
