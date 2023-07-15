#include <algorithm>
#include <iostream>
#include <vector>
#include <cmath>

#include "raylib.h"

#include "model.h"
#include "raymath.h"

const int window_width = 512;
const int window_height = 512;


void fill_image(Image &image, Color color) {
    Color *pixels = (Color *)image.data;
    for (int y = 0; y < image.height; y++) {
        for (int x = 0; x < image.width; x++) {
             pixels[y * image.width + x] = color;
        }
    }
}

bool is_top_left(Vector2 s, Vector2 e) {
    Vector2 diff = {e.x - s.x, e.y - s.y};

    return diff.y > 0 || diff.y == 0 && diff.x > 0;
}

void draw_pixel(int x, int y, Image &image, Color color) {
    Color *pixels = (Color *)image.data;
    pixels[y * image.width + x] = color;
}

void draw_line(Vector2 p0, Vector2 p1, Image &image, Color color) {
    int x0 = p0.x;
    int y0 = p0.y;
    int x1 = p1.x;
    int y1 = p1.y;
    
    bool isSteep = false;

    if (std::abs(x0 - x1) < std::abs(y0 - y1)) {
        isSteep = true;
        std::swap(x0, y0);
        std::swap(x1, y1);
    }

    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    int dx = x1 - x0;
    int dy = y1 - y0;

    float y_step = std::abs(dy/float(dx));
    float y_overflow = 0; 
    int y = y0; 

    for (int x = x0; x <= x1; x++) {
        if (isSteep) {
            draw_pixel(y, x, image, color);
        } else {
            draw_pixel(x, y, image, color);
        }

        y_overflow += y_step;

        // When yOverflow exceeds 0.5, we've accumulated enough
        // fractional yStep to adjust the y coordinate.
        if (y_overflow > .5) {
            y += (y1 > y0 ? 1 : -1);
            y_overflow -= 1.;
        }
    }
}

void draw_triangle_wireframe(Vector2 p0, Vector2 p1, Vector2 p2, Image &image, Color color) {
    draw_line(p0, p1, image, color);
    draw_line(p1, p2, image, color);
    draw_line(p2, p0, image, color);
}

std::pair<Vector2, Vector2> triangle_bb(Vector2 &p0, Vector2 &p1, Vector2 &p2, int canvasHeight, int canvasWidth) {
    float x_start = std::max(0.f, std::min(p0.x, std::min(p1.x, p2.x)));
    float x_end = std::min((float)canvasWidth, std::max(p0.x, std::max(p1.x, p2.x)));
    float y_start = std::max(0.f, std::min(p0.y, std::min(p1.y, p2.y)));
    float y_end = std::min((float)canvasHeight, std::max(p0.y, std::max(p1.y, p2.y)));

    return { Vector2{x_start, y_start}, Vector2{ x_end, y_end }};
}

bool is_inside_triangle(int x, int y, Vector2 &p0, Vector2 &p1, Vector2 &p2) {
    int bias0 = is_top_left(p0, p1) ? 0 : -1;
    int bias1 = is_top_left(p1, p2) ? 0 : -1;
    int bias2 = is_top_left(p2, p0) ? 0 : -1;

    Vector2 edge = Vector2Subtract(p0, p1);
    Vector2 p = Vector2Subtract(p0, {(float)x, (float)y});
    bool res = Vector3CrossProduct({edge.x, edge.y}, {p.x, p.y}).z + bias0 >= 0;

    edge = Vector2Subtract(p1, p2);
    p = Vector2Subtract(p1, {(float)x, (float)y});
    res = res && Vector3CrossProduct({edge.x, edge.y}, {p.x, p.y}).z + bias1 >= 0;

    edge = Vector2Subtract(p2, p0);
    p = Vector2Subtract(p2, {(float)x, (float)y});
    res = res && Vector3CrossProduct({edge.x, edge.y}, {p.x, p.y}).z + bias2 >= 0;

    return res;
}

// bool is_inside_triangle_barycentric(int x, int y, Vector2 &p0, Vector2 &p1, Vector2 &p2) {
//     return false;
// }

void draw_triangle(Vector2 &p0, Vector2 &p1, Vector2 &p2, Image &image, Color color) {
    std::pair<Vector2, Vector2> bb = triangle_bb(p0, p1, p2, image.height, image.width);
    Vector2 start = bb.first;
    Vector2 end = bb.second;
    
    
    for (int x = start.x; x <= end.x; ++x) {
        for (int y = start.y; y <= end.y; ++y) {
            if (is_inside_triangle(x, y, p0, p1, p2)) {
                draw_pixel(x, y, image, color);
            }
        }
    }
}

namespace chapters {
    void lines(Image &image) {
        draw_line(Vector2{10, 10}, Vector2{56, 34}, image, WHITE);
        draw_line(Vector2{20, 13}, Vector2{40, 80}, image, RED); 
        draw_line(Vector2{80, 40}, Vector2{13, 20}, image, RED);
    }

    void triangle_wireframe(Image &image) {
        Vector2 t0[3] = {Vector2 {10, 70}, Vector2 {50, 160}, Vector2 {70, 80}}; 
        Vector2 t1[3] = {Vector2 {180, 50}, Vector2 {150, 1}, Vector2 {70, 180}}; 
        Vector2 t2[3] = {Vector2 {180, 150}, Vector2 {120, 160}, Vector2{130, 180}}; 

        draw_triangle_wireframe(t0[0], t0[1], t0[2], image, RED);
        draw_triangle_wireframe(t1[0], t1[1], t1[2], image, WHITE); 
        draw_triangle_wireframe(t2[0], t2[1], t2[2], image, GREEN);
    }

    void triangles(Image &image) {
        Vector2 vertices[5] = {
            Vector2 { 40, 40 },
            Vector2 { 80, 40 },
            Vector2 { 40, 80 },
            Vector2 { 90, 90 },
            Vector2 { 75, 20 },
        };

        Vector2 v0 = vertices[0];
        Vector2 v1 = vertices[1];
        Vector2 v2 = vertices[2];
        Vector2 v3 = vertices[3];
        Vector2 v4 = vertices[4];

        draw_triangle(v0, v1, v2, image, Color{255, 0, 255, 255});
        draw_triangle(v3, v2, v1, image, Color{100, 25, 255, 255});
        draw_triangle(v4, v1, v0, image, Color{24, 0, 255, 255});
    }
}

void draw_model(tinyrenderer::Model *model, Image &image) {
    for (int i=0; i<model->nfaces(); i++) {
        std::vector<int> face = model->face(i);

        int half_width = floor(image.width / 2);
        int half_height = floor(image.height / 2);

        for (int j=0; j<3; j++) {
            Vector3 v0 = model->vert(face[j]);
            Vector3 v1 = model->vert(face[(j+1)%3]);

            float x0 = (v0.x + 1.) * (half_width);
            float y0 = (v0.y + 1.) * (half_height);
            float x1 = (v1.x + 1.) * (half_width);
            float y1 = (v1.y + 1.) * (half_height);

            draw_line(Vector2{ x0, y0 }, Vector2{ x1, y1 }, image, WHITE);
        }
    }
}

int main(int argc, char** argv) {
    InitWindow(window_width, window_height, "tinyrenderer");
    SetTargetFPS(60); // Set our game to run at 60 frames-per-second

    Image image = GenImageColor(128, 128, BLACK);
    Texture2D texture = LoadTextureFromImage(image);   // Convert Image to Texture2D
    tinyrenderer::Model *model = new tinyrenderer::Model("assets/african_head.obj");

    while (!WindowShouldClose()) {
        fill_image(image, BLACK);

        // chapters::lines(image);
        // chapters::triangle_wireframe(image);
        chapters::triangles(image);
        // draw_model(model, image);

        ImageFlipVertical(&image);
        
        UpdateTexture(texture, image.data);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Set the source rectangle to be the entire texture
        Rectangle srcRec = { 0.0f, 0.0f, (float)texture.width, (float)texture.height };
        // Set the destination rectangle to be the entire window
        Rectangle dstRec = { 0, 0, (float)window_width, (float)window_height };
        // Draw the texture scaled to the window size
        DrawTexturePro(texture, srcRec, dstRec, { 0.0f, 0.0f }, 0.0f, WHITE);

        EndDrawing();
    }

    UnloadTexture(texture);
    UnloadImage(image);

    CloseWindow();

    return 0;
}