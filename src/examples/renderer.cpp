#include "../core/color_buffer.h"
#include "../mesh.h"
#include "../logger.h"

/*
 * The idea here is to render vertices that already have been projected
 *
 * Not remembering how real APIs work, I'd create a `render_mesh` function
 * here, and see how it goes. The passed mesh needs to have faces, vertices
 * and its attributes + references to textures.
 *
 * The function be mutate passed colour buffer.
 */
static ColorBuffer *color_buffer = nullptr;

void use_color_buffer(ColorBuffer *color_buffer) {
    color_buffer = color_buffer;
}

void draw_mesh(Mesh *mesh) {
    if (color_buffer == nullptr) {
        log_message(LogLevel::LOG_LEVEL_ERROR, "ColorBuffer must be initialized");
        exit(1);
    }
}
