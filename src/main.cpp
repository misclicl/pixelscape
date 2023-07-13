#include "tgaimage.h"
#include <iostream>

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

int main() {
    std::cout << "Drawing the image..." << std::endl;

    TGAImage image(100, 100, TGAImage::RGB);

    draw_line(10, 10, 56, 34, image, white);
    draw_line(20, 13, 40, 80, image, red); 
    draw_line(80, 40, 13, 20, image, red);

    image.flip_vertically();
    image.write_tga_file("output.tga");
    return 0;
}