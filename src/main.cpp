#include <iostream>
#include <vector>
#include <cmath>

#include "tgaimage.h"
#include "model.h"
#include "geometry.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);

void draw_line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
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

void test() {
    std::cout << "Drawing the image..." << std::endl;

    TGAImage image(100, 100, TGAImage::RGB);

    draw_line(10, 10, 56, 34, image, white);
    draw_line(20, 13, 40, 80, image, red); 
    draw_line(80, 40, 13, 20, image, red);

    image.flip_vertically();
    image.write_tga_file("output.tga");
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
            draw_line(x0, y0, x1, y1, image, white);
        }
    }

    image.flip_vertically();
    image.write_tga_file("output.tga");
}

int main(int argc, char** argv) {
    if (argc > 1) {
        render();
    } else {
        test();
    }
    return 0;
}