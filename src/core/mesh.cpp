#include "./mesh.h"
#include "../loader_obj.h"

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

TinyMesh* ps_load_mesh(char *mesh_path, std::vector<std::string> textures) {
    std::vector<TinyVertex> vertices;
    PS_Shape shapes[MAX_SHAPES_PER_MESH_COUNT];
    size_t shape_count = 0;

    // TODO: the result is an array of meshes
    parse_mesh(mesh_path, &vertices, shapes, &shape_count);

    TinyMesh mesh;
    mesh.vertices = (TinyVertex *)malloc(vertices.size() * sizeof(TinyVertex));
    mesh.shapes = (PS_MeshShape *)malloc(shape_count * sizeof(PS_MeshShape));

    for (int i = 0; i < vertices.size(); i++) {
        mesh.vertices[i].position = vertices[i].position;
        mesh.vertices[i].texcoords = vertices[i].texcoords;
        mesh.vertices[i].normal = vertices[i].normal;
    }

    for (int i = 0; i < shape_count; i++) {
        auto face_count = shapes[i].faces.size();
        // TODO: cleaup
        mesh.shapes[i].faces = (TinyFace *)malloc(face_count * sizeof(TinyFace));

        for (int j = 0; j < face_count; j++) {
            mesh.shapes[i].faces[j] = shapes[i].faces[j];
        }

        mesh.shapes[i].face_count = face_count;
    }

    mesh.shape_count = shape_count;
    mesh.scale = { 1.0f, 1.0f, 1.0f };
    mesh.translation = { 0.f, 0.f, 0.f };

    // TODO: cleanup + re-use textures
    for (int i = 0; i < textures.size(); i++) {
        auto image = LoadImage(textures[i].c_str());
        mesh.textures.push_back(image);
    }

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

Vec3f get_triangle_normal(Vec4f vertices[3]) {
    auto a = vertices[0];
    auto b = vertices[1];
    auto c = vertices[2];
    Vec3f vec_ab = vec3_from_vec4(b - a);
    Vec3f vec_ac = vec3_from_vec4(c - a);

    return Vec3f::cross(vec_ab, vec_ac).normalize();
};

// TODO: cleanup vertices/faces for each mesh
// and UnloadImage(mesh.diffuse_texture);
