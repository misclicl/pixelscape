#ifndef __COLOR_BUFFER__
#define __COLOR_BUFFER__

#include <stdint.h>

namespace pixelscape {
struct ColorBuffer {
    int width;
    int height;
    uint32_t *pixels;

    void set_pixel(int x, int y, uint32_t color);
    void set_pixel(int i, uint32_t color);

    void clear(uint32_t color);
    void draw_to_texture();

private:
    uint32_t *get_buffer_pixel(int x, int y);
    uint32_t *get_buffer_pixel(int idx);
};
}; // namespace pixelscape
#endif
