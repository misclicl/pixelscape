#pragma once

struct FSInput {
    int x;
    int y;
    float u;
    float v;
    float interpolated_w;
    float interpolated_world_space_pos_x;
    float interpolated_world_space_pos_y;
    float interpolated_world_space_pos_z;
    float interpolated_normal_x;
    float interpolated_normal_y;
    float interpolated_normal_z;
};