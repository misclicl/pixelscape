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
#define MAX_SHAPES_PER_MESH_COUNT 5
#include "tiny_obj_loader.h"

#include "tooling/logger.h"
#include "core/tiny_math.h"

struct PS_Shape {
    std::string name;
    std::vector<TinyFace> faces;
};

// TODO: consider updating api:
// - Option 1. Pass Mesh * and update its fields
// - Option 2. Use index/vertex buffers instead of this
static void parse_mesh(
    char *filepath,
    std::vector<TinyVertex> *vertices,
    PS_Shape *shapes,
    size_t *shapes_count
) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> mesh_shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &mesh_shapes, &materials, &warn, &err, filepath, "assets")) {
        throw std::runtime_error(warn + err);
    }

    /*
     *    v – unqiue vertex ID
     * { [vertex_key]: <position_index in vertices> }
     */
    std::unordered_map<std::string, int> unique_vertices;

    // Keeps position of vertices in vertex array, each 3 indices form a face
    std::vector<uint32_t> index_buffers[MAX_SHAPES_PER_MESH_COUNT] = {};
    std::vector<float> vt = {};

    size_t limit = mesh_shapes.size();
    if (limit > MAX_SHAPES_PER_MESH_COUNT) {
        log_message(LogLevel::LOG_LEVEL_WARN, "Going over the mesh limit");
        limit = MAX_SHAPES_PER_MESH_COUNT;
    }

    for (size_t i = 0; i < limit; i++) {
        tinyobj::shape_t *shape = &mesh_shapes[i];

        shapes[i].name = shape->name;

        for (size_t j = 0; j < shape->mesh.indices.size(); j++) {
            auto& face = shape->mesh.indices[j]; // stores info from 'f a/b/c ...'

            TinyVertex vertex = {};

            vertex.position = {
                attrib.vertices[3 * face.vertex_index + 0],
                attrib.vertices[3 * face.vertex_index + 1],
                attrib.vertices[3 * face.vertex_index + 2],
                1
            };

            vertex.texcoords = {
                attrib.texcoords[2 * face.texcoord_index + 0],
                1.0f - attrib.texcoords[2 * face.texcoord_index + 1]
            };

            vertex.normal = {
                attrib.normals[3 * face.normal_index + 0],
                attrib.normals[3 * face.normal_index + 1],
                attrib.normals[3 * face.normal_index + 2]
            };

            // Vertex index + Texcoord index
            std::string key = std::to_string(face.vertex_index) +
                std::to_string(face.texcoord_index);

            if (unique_vertices.count(key) == 0) {
                unique_vertices[key] = static_cast<uint32_t>(vertices->size());
                vertices->push_back(vertex);
            }

            index_buffers[i].push_back(unique_vertices[key]);

        }
    }


    for (size_t i = 0; i < limit; i++) {
        auto index_buffer = index_buffers[i];
        for (size_t j = 2; j < index_buffer.size(); j += 3) {
            TinyFace face = {};

            for (int k = 0; k < 3; ++k) {
                face.indices[k] = index_buffer[j - 2 + k];
            }

            shapes[i].faces.push_back(face);
        }
    }

    *shapes_count = limit;
}
#endif
