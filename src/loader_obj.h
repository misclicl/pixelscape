#ifndef __MESH_PARSER__
#define __MESH_PARSER__

#include <fstream>
#include <sstream>

// TODO:
// - include as a submodule
// - create a .cpp file to solve duplication issue
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "mesh.h"
#include "tiny_math.h"

namespace tinyrenderer {

// TODO: do I need static here?
static void load_mesh(
    std::vector<Vec3f> *vertices, 
    std::vector<TriangleFace> *faces
) {
    // std::ifstream in("assets/bunny-lr.obj");
    std::ifstream in("assets/cube.obj");
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

// TODO: reconsider inline
inline void parse_mesh(const char *filepath) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath)) {
        throw std::runtime_error(warn + err);
    }

    std::vector<Vec3f> vertices = {};
    std::vector<Vec3f> faces= {};

    // TODO: find a way to write to the mesh;
    Mesh *mesh = (Mesh *)malloc(sizeof(Mesh));

    for (size_t i = 0; i < shapes.size(); i++) {
        tinyobj::shape_t *shape = &shapes[i];
        for (size_t j = 0; j < shape->mesh.indices.size(); j++) {
            tinyobj::index_t *index = &shape->mesh.indices[j];

            Vec3f vertex;

            if (index->vertex_index) {
                vertex = {
                    attrib.vertices[3 * index->vertex_index + 0],
                    attrib.vertices[3 * index->vertex_index + 1],
                    attrib.vertices[3 * index->vertex_index + 2],
                };
            }

            vertices.push_back(vertex);
        }
    }
}
}
#endif
