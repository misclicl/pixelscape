#include <stdint.h>

namespace tinyrenderer {
struct RendererS {
    int width;
    int height;
    uint32_t* color_buffer;

    void draw_triangle();
    void draw_model();
};
};
