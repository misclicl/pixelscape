#include "mesh.h"
#include "loader_obj.h"

#define MAX_MESHES_COUNT 100

/*
 *                                |
 *                                |/
 *          0 -------- 1     x ---*----
 *         /|         /|         /|
 *        / |        / |        z |
 *       3----------2  |          y
 *       |  |       |  |
 *       |  4-------|--5
 *       | /        | /
 *       |/         |/
 *       7----------6
 *
 */

Vec3f cube_vertices[CUBE_VERTICES_COUNT] = {
    // # 0
    { 1, -1, -1}, // 0
    {-1, -1, -1}, // 1
    {-1, -1,  1}, // 2
    { 1, -1,  1}, // 3

    { 1,  1, -1}, // 4
    {-1,  1, -1}, // 5
    {-1,  1,  1}, // 6
    { 1,  1,  1}, // 7
};

TinyFace cube_faces[CUBE_FACES_COUNT] = {
    // Using OpenGL order
    // top
    {0, 1, 2},
    {0, 2, 3},
    // bottom
    {7, 6, 5},
    {7, 5, 4},
    // front
    {3, 2, 6},
    {3, 6, 7},
    // back
    {1, 0, 4},
    {1, 4, 5},
    // left
    {0, 3, 7},
    {0, 7, 4},
    // right
    {2, 1, 5},
    {2, 5, 6}
};


static TinyMesh meshes[MAX_MESHES_COUNT];
static size_t mesh_count = 0;

TinyMesh* ps_load_mesh(char *mesh_path, char *texture_path) {
    std::vector<TinyVertex> vertices;
    std::vector<TinyFace> faces;

    parse_mesh(mesh_path, &vertices, &faces);

    TinyMesh mesh;
    mesh.vertices = (TinyVertex *)malloc(vertices.size() * sizeof(TinyVertex));
    mesh.faces = (TinyFace *)malloc(faces.size() * sizeof(TinyFace));

    for (int i = 0; i < vertices.size(); i++) {
        mesh.vertices[i].position = vertices[i].position;
        mesh.vertices[i].texcoords = vertices[i].texcoords;
    }

    for (int i = 0; i < faces.size(); i++) {
        mesh.faces[i] = faces[i];
    }

    mesh.face_count = faces.size();
    mesh.vertex_count = vertices.size();
    mesh.scale = { 1.0f, 1.0f, 1.0f };
    mesh.translation = { 0.f, 0.f, 0.f };

    mesh.diffuse_texture = LoadImage(texture_path);

    // Write to array
    meshes[mesh_count++] = mesh;

    return &meshes[mesh_count - 1];
}

TinyMesh *ps_get_mesh_data() {
    return meshes;
}

size_t ps_get_mesh_count() {
    return mesh_count;
}

// TODO: cleanup vertices/faces for each mesh
// and UnloadImage(mesh.diffuse_texture);
