#ifndef __MESH_PARSER__
#define __MESH_PARSER__

#include "raylib.h"
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>

// TODO:
// - include as a submodule
// - create a .cpp file to solve duplication issue
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "mesh.h"
#include "core/tiny_math.h"

namespace pixelscape {

// TODO: do I need static here?
// TODO: Obsolete. I use tinyobjloader instead now
static void load_mesh(
    char* file_path,
    std::vector<Vec3f> *vertices, 
    std::vector<TinyFace> *faces
) {
    std::ifstream in(file_path);
    char trash;

    if (!in.is_open()) {
        printf("WARNING: Failed to open the file \n");
    }
    while (!in.eof()) {
        std::string line;
        std::getline(in, line);

        std::istringstream ss(line.c_str());

        if (line.compare(0, 2, "v ") == 0) {
            ss >> trash;

            Vec3f vertex;
            ss >> vertex.x;
            ss >> vertex.y;
            ss >> vertex.z;

            (*vertices).push_back(vertex);
        } else if (line.compare(0, 2, "f ") == 0) {
            int idx, idx_uv, idx_normal;
            std::vector<int> indices;
            std::string temp;

            ss >> trash;

            while (ss >> idx >> trash >> idx_uv >> trash >> idx_normal) {
                idx--;
                indices.push_back(idx);
            }

            TinyFace face {
                indices.at(0),
                indices.at(1),
                indices.at(2),
            };

            (*faces).push_back(face);
        }
    }

    in.close();
}

// TODO: reconsider static
// TODO: consider updating api:
// - Option 1. Pass Mesh * and update its fields
// - Option 2. Use index/vertex buffers instead of this
static void parse_mesh(
    char *filepath, 
    std::vector<Vertex> *vertices,
    std::vector<TinyFace> *faces
) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath)) {
        throw std::runtime_error(warn + err);
    }

    std::unordered_map<std::string, int> unique_vertices;
    std::vector<uint32_t> index_buffer = {};
    std::vector<float> vt = {};

    for (size_t i = 0; i < shapes.size(); i++) {
        tinyobj::shape_t *shape = &shapes[i];

        for (size_t j = 0; j < shape->mesh.indices.size(); j++) {
            auto& face = shape->mesh.indices[j]; // stores info from 'f a/b/c ...'

            Vertex vertex = {};

            vertex.position = {
                attrib.vertices[3 * face.vertex_index + 0],
                attrib.vertices[3 * face.vertex_index + 1],
                attrib.vertices[3 * face.vertex_index + 2]
            };

            vertex.texcoords = {
                attrib.texcoords[2 * face.texcoord_index + 0],
                // TODO: do I need -1 here?
                1.0f - attrib.texcoords[2 * face.texcoord_index + 1]
            };

            std::string key = std::to_string(face.vertex_index) + 
                std::to_string(face.texcoord_index);

            if (unique_vertices.count(key) == 0) {
                unique_vertices[key] = static_cast<uint32_t>(vertices->size());
                vertices->push_back(vertex);
            }

            index_buffer.push_back(unique_vertices[key]);
        }
    }

    for (size_t i = 2; i < index_buffer.size(); i += 3) {
        TinyFace face = {};

        Color random_color = {
            (unsigned char)(255 * random()),
            (unsigned char)(255 * random()),
            (unsigned char)(255 * random()),
            255};

        face.color = ColorToInt(random_color);

        for (int j = 0; j < 3; ++j) {
            face.indices[j] = index_buffer[i - 2 + j];
        }


        faces->push_back(face);
    }
}
}
#endif
