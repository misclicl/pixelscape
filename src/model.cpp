#include "model.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

pixelscape::Model::Model(const char *filename) : verts_(), faces_(), uv_coords_() {
    std::ifstream in;
    in.open(filename, std::ifstream::in);
    if (in.fail())
        return;
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
            pixelscape::Face face;

            int idx, idx_uv, itrash;

            iss >> trash;
            while (iss >> idx >> trash >> idx_uv >> trash >> itrash) {
                idx--; // in wavefront obj all indices start at 1, not zero
                idx_uv--;

                face.vertices.push_back(idx);
                face.uvs.push_back(idx_uv);
            }

            faces_.push_back(face);
        } else if (!line.compare(0, 3, "vt ")) {
            // write uv coords
            iss >> trash;
            iss >> trash;
            Vector2 uv;
            iss >> uv.x;
            iss >> uv.y;

            uv_coords_.push_back(uv);
        }
    }
}

pixelscape::Model::~Model() {
}

int pixelscape::Model::Model::n_verts() const {
    return (int)verts_.size();
}

int pixelscape::Model::Model::n_faces() const {
    return (int)faces_.size();
}

int pixelscape::Model::Model::n_uv_coords() const {
    return (int)uv_coords_.size();
}

std::vector<int> pixelscape::Model::Model::face_vertices(int i) const {
    return faces_[i].vertices;
}

std::vector<int> pixelscape::Model::Model::face_uvs(int i) const {
    return faces_[i].uvs;
}

Vector3 pixelscape::Model::Model::vert(int i) const {
    return verts_[i];
}

Vector2 pixelscape::Model::Model::uv_coords(int i) const {
    return uv_coords_[i];
}
