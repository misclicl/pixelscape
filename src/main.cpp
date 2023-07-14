#include <iostream>
#include <vector>
#include <cmath>

#include "tgaimage.h"
#include "model.h"
#include "geometry.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
const TGAColor cyan = TGAColor(0, 255, 255, 255);

void draw_line(Vec2i p0, Vec2i p1, TGAImage &image, TGAColor color) {
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
            image.set(y, x, color);
        } else {
            image.set(x, y, color);
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

void draw_triangle(Vec2i p0, Vec2i p1, Vec2i p2, TGAImage &image, TGAColor color) {
    if (p0.y > p1.y) { std::swap(p0, p1); }
    if (p0.y > p2.y) { std::swap(p0, p2); }
    if (p1.y > p2.y) { std::swap(p1, p2); }

    int triangle_height = p2.y - p0.y;

    for (int i = 0; i < triangle_height; i++) {
        bool isUpperSegment = i > p1.y - p0.y || p1.y== p0.y; 
        int segment_height = isUpperSegment ? p2.y-p1.y : p1.y - p0.y; 
        float triangle_progress = (float)i / triangle_height; 
        float segment_progress  = (float)(i - (isUpperSegment ? p1.y- p0.y : 0)) / segment_height;

        Vec2i A = p0 + (p2 - p0) * triangle_progress; 
        Vec2i B = isUpperSegment ? p1 + (p2 - p1) * segment_progress : p0 + (p1 - p0)*segment_progress; 

        if (A.x > B.x) std::swap(A, B); 
        for (int j = A.x; j <= B.x; j++) { 
            image.set(j, p0.y + i, color);
        } 
    }
}

namespace tests {
    void lines() {
        std::cout << "Drawing the image..." << std::endl;

        TGAImage image(100, 100, TGAImage::RGB);

        draw_line(Vec2i(10, 10), Vec2i(56, 34), image, white);
        draw_line(Vec2i(20, 13), Vec2i(40, 80), image, red); 
        draw_line(Vec2i(80, 40), Vec2i(13, 20), image, red);

        image.flip_vertically();
        image.write_tga_file("output.tga");
    }

    void triangles() {
        std::cout << "Drawing the image..." << std::endl;
        int height = 256;
        int width = 256;

        TGAImage image(width, height, TGAImage::RGB);

        Vec2i t0[3] = {Vec2i(10, 70),   Vec2i(50, 160),  Vec2i(70, 80)}; 
        Vec2i t1[3] = {Vec2i(180, 50),  Vec2i(150, 1),   Vec2i(70, 180)}; 
        Vec2i t2[3] = {Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180)}; 

        draw_triangle(t0[0], t0[1], t0[2], image, red); 
        draw_triangle(t1[0], t1[1], t1[2], image, white); 
        draw_triangle(t2[0], t2[1], t2[2], image, green);

        image.flip_vertically();
        image.write_tga_file("output.tga");
    }

}

void render() {
    std::cout << "Drawing a model" << std::endl;
    const int width  = 800;
    const int height = 800;
    Model *model = new Model("assets/african_head.obj");

    TGAImage image(width, height, TGAImage::RGB);
    for (int i=0; i<model->nfaces(); i++) {
        std::vector<int> face = model->face(i);
        for (int j=0; j<3; j++) {
            Vec3f v0 = model->vert(face[j]);
            Vec3f v1 = model->vert(face[(j+1)%3]);
            int x0 = (v0.x+1.)*width/2.;
            int y0 = (v0.y+1.)*height/2.;
            int x1 = (v1.x+1.)*width/2.;
            int y1 = (v1.y+1.)*height/2.;
            draw_line(Vec2i(x0, y0), Vec2i(x1, y1), image, white);
        }
    }

    image.flip_vertically();
    image.write_tga_file("output.tga");
}

int main(int argc, char** argv) {
    if (argc == 1) {
        tests::triangles();
    } else {
        render();
    }
    return 0;
}