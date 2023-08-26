#include "raylib.h"

#include "../core/mesh.h"
#include "../core/tiny_math.h"
#include "../core/display.h"

#include "../core/color_buffer.h"

void draw_debug_quad(ColorBuffer *color_buffer, Image *diffuse_texture, float *depth_buffer);