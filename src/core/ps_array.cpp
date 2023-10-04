#include "ps_array.h"

void normalize(float *data, size_t size) {
    if (data == nullptr || size == 0) {
        // Invalid or empty buffer
        return;
    }

    // Find min and max values in the depth buffer
    float min_value = data[0];
    float max_value = data[0];

    for (size_t i = 1; i < size; ++i) {
        if (data[i] < min_value) {
            min_value = data[i];
        }
        if (data[i] > max_value) {
            max_value = data[i];
        }
    }

    // Check for a degenerate case
    if (min_value == max_value) {
        // All values are the same; just set them all to 0.5 or any constant value in [0,1]
        for (size_t i = 0; i < size; ++i) {
            data[i] = 0.5f;
        }
        return;
    }

    // Normalize the values
    float range = max_value - min_value;
    for (size_t i = 0; i < size; ++i) {
        data[i] = (data[i] - min_value) / range;
    }
}
