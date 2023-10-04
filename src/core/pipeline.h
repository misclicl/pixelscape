#pragma once

#include "color_buffer.h"
#include "depth_buffer.h"

#include "camera.h"
#include "clipping.h"
#include "display.h"

void render_triangle(
    CameraType camera_type,
    ColorBuffer *color_buffer,
    DepthBuffer *depth_buffer,
    TinyTriangle *triangle,
    RendererState *renderer_state,
    Light *light,
    Image *diffuse_texture
);