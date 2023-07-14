#include <iostream>
#include <vector>
#include <cmath>

#include "raylib.h"

#include "model.h"
#include "raymath.h"

const int windowWidth = 450;
const int windowHeight = 450;

void draw_pixel(int x, int y, Image &image, Color color) {
    Color *pixels = (Color *)image.data;
    pixels[y * windowWidth + x] = color;
}

void fill_image(Image &image, Color color) {
    Color *pixels = (Color *)image.data;
    for (int y = 0; y < windowHeight; y++) {
        for (int x = 0; x < windowWidth; x++) {
             pixels[y * windowWidth + x] = color;
        }
    }
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

    float yStep = std::abs(dy/float(dx));
    float yOverflow = 0; 
    int y = y0; 

    for (int x = x0; x <= x1; x++) {
        if (isSteep) {
            draw_pixel(y, x, image, color);
        } else {
            draw_pixel(x, y, image, color);
        }

        yOverflow += yStep;

        // When yOverflow exceeds 0.5, we've accumulated enough
        // fractional yStep to adjust the y coordinate.
        if (yOverflow > .5) {
            y += (y1 > y0 ? 1 : -1);
            yOverflow -= 1.;
        }
    }
}

void draw_triangle_wireframe(Vector2 p0, Vector2 p1, Vector2 p2, Image &image, Color color) {
    draw_line(p0, p1, image, color);
    draw_line(p1, p2, image, color);
    draw_line(p2, p0, image, color);
}

void draw_triangle(Vector2 p0, Vector2 p1, Vector2 p2, Image &image, Color color) {
    if (p0.y > p1.y) { std::swap(p0, p1); }
    if (p0.y > p2.y) { std::swap(p0, p2); }
    if (p1.y > p2.y) { std::swap(p1, p2); }

    int triangle_height = p2.y - p0.y;

    for (int i = 0; i < triangle_height; i++) {
        bool isUpperSegment = i > p1.y - p0.y || p1.y== p0.y; 
        int segment_height = isUpperSegment ? p2.y-p1.y : p1.y - p0.y; 
        float triangle_progress = (float)i / triangle_height; 
        float segment_progress  = (float)(i - (isUpperSegment ? p1.y- p0.y : 0)) / segment_height;

        Vector2 A = Vector2Add(p0, Vector2Scale(Vector2Subtract(p2, p0), triangle_progress));
        Vector2 B = isUpperSegment ?
            Vector2Add(p1, Vector2Scale(Vector2Subtract(p2, p1), segment_progress)) :
            Vector2Add(p0, Vector2Scale(Vector2Subtract(p1, p0), segment_progress));

        if (A.x > B.x) std::swap(A, B); 
        for (int j = A.x; j <= B.x; j++) { 
            draw_pixel(j, p0.y + i, image, color);
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
        Vector2 t0[3] = {Vector2 {10, 70}, Vector2 {50, 160}, Vector2 {70, 80}}; 
        Vector2 t1[3] = {Vector2 {180, 50}, Vector2 {150, 1}, Vector2 {70, 180}}; 
        Vector2 t2[3] = {Vector2 {180, 150}, Vector2 {120, 160}, Vector2{130, 180}}; 

        draw_triangle(t0[0], t0[1], t0[2], image, RED); 
        draw_triangle(t1[0], t1[1], t1[2], image, WHITE); 
        draw_triangle(t2[0], t2[1], t2[2], image, GREEN); 
    }

}

void draw_model(tinyrenderer::Model *model, Image &image) {
    for (int i=0; i<model->nfaces(); i++) {
        std::vector<int> face = model->face(i);
        for (int j=0; j<3; j++) {
            Vector3 v0 = model->vert(face[j]);
            Vector3 v1 = model->vert(face[(j+1)%3]);
            float x0 = (v0.x+1.)*windowWidth/2.;
            float y0 = (v0.y+1.)*windowHeight/2.;
            float x1 = (v1.x+1.)*windowWidth/2.;
            float y1 = (v1.y+1.)*windowHeight/2.;

            draw_line(Vector2{x0, y0}, Vector2{ x1, y1 }, image, WHITE);
        }
    }
}

int main(int argc, char** argv) {
    InitWindow(windowWidth, windowHeight, "tinyrenderer");
    SetTargetFPS(60); // Set our game to run at 60 frames-per-second

    Image image = GenImageColor(windowWidth, windowHeight, RAYWHITE);
    Texture2D texture = LoadTextureFromImage(image);   // Convert Image to Texture2D
    tinyrenderer::Model *model = new tinyrenderer::Model("assets/african_head.obj");

    while (!WindowShouldClose()) {
        fill_image(image, BLACK);

        // chapters::lines(image);
        // chapters::triangle_wireframe(image);
        // chapters::triangles(image);
        draw_model(model, image);

        ImageFlipVertical(&image);
        
        UpdateTexture(texture, image.data);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawTexture(texture, windowWidth/2 - texture.width/2, windowHeight/2 - texture.height/2, WHITE);

        EndDrawing();
    }

    UnloadTexture(texture);
    UnloadImage(image);

    CloseWindow();

    return 0;
}