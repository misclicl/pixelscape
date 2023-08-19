#ifndef __RENDERING_WITH_SHADING__
#define __RENDERING_WITH_SHADING__

#include "raylib.h"
#include <stdint.h>

#include "../core/color_buffer.h"
#include "../model.h"

struct ProgramShading {
    void init();
    void run(
        ColorBuffer *color_buffer,
        TinyModel *model,
        Vector3 &light_dir,
        Image diffuse_texture
    );
    void cleanup();
};
#endif

