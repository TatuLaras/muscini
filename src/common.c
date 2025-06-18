#include "common.h"

#include <math.h>
#include <raylib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

uint64_t max(uint64_t a, uint64_t b) {
    if (a > b)
        return a;
    return b;
}

uint64_t min(uint64_t a, uint64_t b) {
    if (a < b)
        return a;
    return b;
}

float maxf(float a, float b) {
    if (a > b)
        return a;
    return b;
}

float minf(float a, float b) {
    if (a < b)
        return a;
    return b;
}

float quantize(float value, float interval) {
    return roundf(value / interval) * interval;
}

Vector3 matrix_get_position(Matrix transform) {
    return (Vector3){transform.m12, transform.m13, transform.m14};
}

void matrix_set_position(Matrix *transform, Vector3 position) {
    transform->m12 = position.x;
    transform->m13 = position.y;
    transform->m14 = position.z;
}

Matrix matrix_strip_position(Matrix transform) {
    transform.m12 = 0;
    transform.m13 = 0;
    transform.m14 = 0;
    return transform;
}

void strip_filename(char *filepath, size_t n) {
    size_t last_slash = 0;
    size_t length = 0;
    while (filepath[length] || length >= n) {
        if (filepath[length] == DIR_SEPARATOR)
            last_slash = length;
        length++;
    }

    if (last_slash == length)
        return;

    filepath[last_slash + 1] = 0;
}

int has_suffix(const char *string, const char *suffix) {
    size_t string_len = strlen(string);
    size_t suffix_len = strlen(suffix);
    if (string_len < suffix_len) {
        return 0;
    }
    return memcmp(string + (string_len - suffix_len), suffix, suffix_len) == 0;
}
