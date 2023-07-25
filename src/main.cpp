#include <cstdio>
#include <iostream>
#include <vector>
#include <array>

#include "background_grid.h"
#include "perspective_projection.h"
#include "raylib.h"
#include "raymath.h"

#include "model.h"
#include "display.h"
#include "software_renderer.h"

const int window_width = 512;
const int window_height = 512;

const int target_render_size_x = 256;
const int target_render_size_y = 256;

const int unit = 100;

const float cube_halfsize = .5f;

const float front = -cube_halfsize;
const float back = cube_halfsize;

Vector3 vertices[8] = {
    Vector3 { -cube_halfsize, -cube_halfsize, front },
    Vector3 {  cube_halfsize, -cube_halfsize, front },
    Vector3 { -cube_halfsize,  cube_halfsize, front },
    Vector3 {  cube_halfsize,  cube_halfsize, front },

    Vector3 { -cube_halfsize, -cube_halfsize, back },
    Vector3 {  cube_halfsize, -cube_halfsize, back },
    Vector3 { -cube_halfsize,  cube_halfsize, back },
    Vector3 {  cube_halfsize,  cube_halfsize, back },
};

const float far_clipping_plane = 10000;
const float near_clipping_plane = .1;

Vector4 Vector4Transform(const Vector4 v, const Matrix mat) {
    Vector4 result;
    result.x = mat.m0*v.x + mat.m4*v.y + mat.m8*v.z + mat.m12*v.w;
    result.y = mat.m1*v.x + mat.m5*v.y + mat.m9*v.z + mat.m13*v.w;
    result.z = mat.m2*v.x + mat.m6*v.y + mat.m10*v.z + mat.m14*v.w;
    result.w = mat.m3*v.x + mat.m7*v.y + mat.m11*v.z + mat.m15*v.w;
    return result;
}

void render_with_shading(const int &delta, const tinyrenderer::Model *model, Vector3 &light_dir, Image diffuse_texture) {
    float *depth_buffer = new float[target_render_size_x*target_render_size_y];

    for (uint32_t i = 0; i < target_render_size_x * target_render_size_y; ++i) {
        depth_buffer[i] = -far_clipping_plane;
    }

    Matrix rotation = MatrixRotateY((float)delta / 5 * DEG2RAD);
    Matrix scale = MatrixScale(1.f, 1.f, 1.f);
    Matrix model_to_world = MatrixMultiply(scale, rotation);

    for (uint32_t i = 0; i < model->n_faces(); ++i) {
        Vector3 world_coords[3];
        Vector2 uv_coords[3];
        Vector3 vertices[3];

        std::vector<int> face_vertex_indices = model->face_vertices(i);
        std::vector<int> uvs = model->face_uvs(i);

        int half_width = floor(target_render_size_x / 2);
        int half_height = floor(target_render_size_y / 2);

        for (int j = 0; j < 3; j++) {
            world_coords[j] = Vector3Transform(
                model->vert(face_vertex_indices[j]),
                model_to_world
            );
        }

        for (int j = 0; j < 3; j++) {
            uv_coords[j] = model->uv_coords(uvs[j]);

            vertices[j].x = (world_coords[j].x + 1.) * (half_width);
            vertices[j].y = (world_coords[j].y + 1.) * (half_height);
            vertices[j].z = world_coords[j].z;
        }

        Vector3 triangle_normal = Vector3Normalize(
            Vector3CrossProduct(
                Vector3Subtract(world_coords[2], world_coords[0]),
                Vector3Subtract(world_coords[1], world_coords[0])
            )
        );

        float intensity = std::min(100.f, Vector3DotProduct(triangle_normal, light_dir));

        if (intensity > 0) {
            tinyrenderer::draw_triangle(
                vertices,
                uv_coords,
                diffuse_texture, 
                intensity,
                depth_buffer,
                target_render_size_x,
                target_render_size_y
            );
        }
    }

    delete[] depth_buffer;
}


namespace examples {
void render_vertices(Image &diffuse_texture) {
    float *depth_buffer = new float[target_render_size_x*target_render_size_y];

    for (uint32_t i = 0; i < target_render_size_x * target_render_size_y; ++i) {
        depth_buffer[i] = -far_clipping_plane;
    }

    Vector3 v0 = vertices[0];
    Vector3 v1 = vertices[1];
    Vector3 v2 = vertices[2];
    Vector3 v3 = vertices[3];
    Vector3 v4 = vertices[4];

    Vector3 t0[3] = {v0, v1, v2};
    Vector2 uv[3] = { {0, 0}, {0, 1}, {1, 1} };
    tinyrenderer::draw_triangle(t0, uv, diffuse_texture, 1, depth_buffer, target_render_size_x, target_render_size_y);

    delete[] depth_buffer;
}

void linear_transofrmations_example() {
    std::vector<std::array<int, 3>> indices;
    std::vector<Vector3> vrtxs;

    indices.push_back({0, 1, 2});
    indices.push_back({1, 2, 3});

    for (const Vector3 &vertex : vertices) {
        vrtxs.push_back(vertex);
    }
    tinyrenderer::draw_triangles(
        vrtxs, 
        indices,
        target_render_size_x,
        target_render_size_y,
        RAYWHITE
    );


    // .707 = cos(45deg) = sin(45deg)
    Matrix m = {
        1, -.707, 0,
        0,     1, 0,
        0,     0, 0,
    };

    vrtxs.clear();
    for (const Vector3 &vertex : vertices) {
        vrtxs.push_back(Vector3Transform(vertex, m));
    }

    tinyrenderer::draw_triangles(
        vrtxs, 
        indices,
        target_render_size_x,
        target_render_size_y,
        RED
    );


    m.m1 = .707;
    vrtxs.clear();
    for (const Vector3 &vertex : vertices) {
        vrtxs.push_back(Vector3Transform(vertex, m));
    }
    tinyrenderer::draw_triangles(
        vrtxs, 
        indices,
        target_render_size_x,
        target_render_size_y,
        GREEN
    );

    m.m0 = .707;
    m.m5 = .707;

    vrtxs.clear();
    for (const Vector3 &vertex : vertices) {
        vrtxs.push_back(Vector3Transform(vertex, m));
    }
    tinyrenderer::draw_triangles(
        vrtxs, 
        indices,
        target_render_size_x,
        target_render_size_y,
        BLUE
    );
}

/*
    element indices:

    0  4  8   12
    1  5  9   13
    2  6  10  14
    3  7  11  15
*/    
Matrix create_viewport_matrix(int y, int x, int width, int height) {
    Matrix m = {0};
    int depth = 1000;

    m.m12 = x + width/2.f;
    m.m9 = y + height/2.f;
    m.m14 = depth/2.f;

    m.m0 = width/2.f;
    m.m5 = height/2.f;
    m.m10 = depth/2.f;

    return m;
}

Matrix create_perspective(float fov, float a, float znear, float zfar) {
    Matrix m = Matrix();

    m.m0 = a * (1 / tan(fov/2));
    m.m5 = 1 / tan(fov/2);
    m.m10 = zfar / (zfar - znear);
    m.m14 = (-zfar * znear)/ (zfar - znear);
    m.m11 = 1.0;

    return m;
}

void shape_perspective() { 
    std::vector<std::array<int, 3>> indices;
    std::vector<Vector3> vrtxs;

    indices.push_back({0, 1, 2});
    indices.push_back({1, 2, 3});

    for (const Vector3 &vertex : vertices) {
        vrtxs.push_back(vertex);
    }
    tinyrenderer::draw_triangles(
        vrtxs, 
        indices,
        target_render_size_x,
        target_render_size_y,
        RAYWHITE
    );

    indices.push_back({4, 5, 6});
    indices.push_back({5, 6, 7});
    vrtxs.clear();

    Matrix world = MatrixTranslate(0, 0, 2);
    Matrix view = MatrixLookAt({0, 0, 0}, {0, 0, 1}, {0, 1, 0});
    Matrix projection = MatrixPerspective(45*DEG2RAD, 1.f, near_clipping_plane, far_clipping_plane);
    // Matrix view_projection = MatrixMultiply(MatrixMultiply(view, world), projection);
    Matrix view_projection = MatrixMultiply(projection, MatrixMultiply(view, world));

    for (const Vector3 &v_local: vertices) {
        // Vector3 point = Vector3Transform(vertex, MatrixMultiply(transform, projection));
        // Vector3 point = vertex;
        Vector4 point = Vector4Transform({
            v_local.x,
            v_local.y,
            v_local.z,
            1.0f
        }, view_projection);

        if (point.w != 0) {
            point.x /= point.w;
            point.y /= point.w;
            point.z /= point.w;
        }

        vrtxs.push_back({
            point.x,
            point.y,
            point.z,
        });
    }

    tinyrenderer::draw_triangles(
        vrtxs, 
        indices,
        target_render_size_x,
        target_render_size_y,
        YELLOW
    );

    tinyrenderer::draw_axis(target_render_size_x, target_render_size_y);
}

}

int main(int argc, char **argv) {
    using tinyrenderer::RendererS;
    InitWindow(window_width, window_height, "tinyrenderer");
    SetTargetFPS(60);
    int frame_counter = 0;

    tinyrenderer::Model *model = new tinyrenderer::Model("assets/head.obj");
    // tinyrenderer::Model *model = new tinyrenderer::Model("assets/cube.obj");

    // Image diffuse_img = LoadImage("assets/diffuse.png");
    Image diffuse_img = LoadImage("assets/grid.png");

    Vector3 light_dir = {0, 0, -1};

    RenderTexture2D render_texture = LoadRenderTexture(target_render_size_x, target_render_size_y);

    tinyrenderer::program::PerspectiveProjection example_a;
    tinyrenderer::program::BackgroundGrid bg_grid;
    uint32_t *color_buffer = 
        (uint32_t *)malloc(target_render_size_x * target_render_size_y * sizeof(unsigned int));

    while (!WindowShouldClose()) {
        BeginTextureMode(render_texture);
        ClearBackground(BLACK);

        // render_vertices(diffuse_img);
        // render_texture
        // render_with_shading(frame_counter, model, light_dir, diffuse_img);
        // examples::linear_transofrmations_example();
        // examples::shape_perspective();
        // example_a.run(near_clipping_plane, target_render_size_x, target_render_size_y);
        // tinyrenderer::program::BackgroundGrid
        bg_grid.run(color_buffer, target_render_size_x, target_render_size_y);

        EndTextureMode();

        BeginDrawing();

        ClearBackground(WHITE);
        DrawTexturePro(
            render_texture.texture,
            Rectangle{ 0, 0, static_cast<float>(render_texture.texture.width), static_cast<float>(render_texture.texture.height) },
            Rectangle{ 0, 0, static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight()) },
            Vector2{ 0, 0 },
            0,
            WHITE
        );

        frame_counter++;
        EndDrawing();
    }


    UnloadRenderTexture(render_texture);
    UnloadImage(diffuse_img);
    CloseWindow();

    delete model;
    free(color_buffer);

    return 0;
}
