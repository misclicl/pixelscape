#ifndef __MESH_PARSER__
#define __MESH_PARSER__

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
#include "tiny_math.h"

namespace tinyrenderer {

// TODO: do I need static here?
// TODO: Obsolete. I use tinyobjloader instead now
static void load_mesh(
    char* file_path,
    std::vector<Vec3f> *vertices, 
    std::vector<TriangleFace> *faces
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

            // while (ss >> temp)
            while (ss >> idx >> trash >> idx_uv >> trash >> idx_normal) {
                idx--;
                indices.push_back(idx);
            }
            // while (ss >> idx >> trash >> trash >> idx_normal) {  // Update this line
            //     idx--;
            //     indices.push_back(idx);
            // }

            TriangleFace face {
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
    std::vector<Vec3f> *vertices,
    std::vector<TriangleFace> *faces
) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath)) {
        throw std::runtime_error(warn + err);
    }

    std::unordered_map<std::string, int> unique_vertices;

    // std::vector<Vec3f> vertex_buffer = {};
    std::vector<uint32_t> index_buffer = {};

    for (size_t i = 0; i < shapes.size(); i++) {
        tinyobj::shape_t *shape = &shapes[i];
        for (size_t j = 0; j < shape->mesh.indices.size(); j++) {
            auto& face = shape->mesh.indices[j]; // stores info from 'f a/b/c ...'

            Vec3f vertex = {
                attrib.vertices[3 * face.vertex_index + 0],
                attrib.vertices[3 * face.vertex_index + 1],
                attrib.vertices[3 * face.vertex_index + 2]
            };

            std::string key = std::to_string(face.vertex_index);

            if (unique_vertices.count(key) == 0) {
                unique_vertices[key] = static_cast<uint32_t>(vertices->size());
                // vertex_buffer.push_back(vertex);
                vertices->push_back(vertex);
            }

            index_buffer.push_back(unique_vertices[key]);
        }
    }

    for (size_t i = 2; i < index_buffer.size(); i += 3) {
        TriangleFace face = {};

        face.a = index_buffer[i - 2];
        face.b = index_buffer[i - 1];
        face.c = index_buffer[i];
    
        faces->push_back(face);
    }
}
}
#endif
