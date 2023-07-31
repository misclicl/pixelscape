#ifndef __RENDERING_WITH_SHADING__
#define __RENDERING_WITH_SHADING__

#include "raylib.h"
#include <stdint.h>

#include "../core/color_buffer.h"
#include "../model.h"

namespace tinyrenderer::RenderWithShading {
struct Program {
    void init();
    void run(
        ColorBuffer *color_buffer, 
        Model *model,
        Vector3 &light_dir,
        Image diffuse_texture
    );
    void cleanup();
};
} // namespace tinyrenderer::RenderWithShading
#endif

