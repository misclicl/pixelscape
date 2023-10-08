#ifndef __COLOR_BUFFER__
#define __COLOR_BUFFER__

#include <raylib.h>
#include <stdint.h>

struct ColorBuffer {
    int width;
    int height;
    Color *pixels;

    void set_pixel(int x, int y, Color color);
    void set_pixel(int i, Color color);

    void clear(Color color);
    void draw_to_texture();

private:
    Color *get_buffer_pixel(int x, int y);
    Color *get_buffer_pixel(int idx);
};
#endif
