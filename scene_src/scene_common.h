#ifndef _SCENE_COMMON
#define _SCENE_COMMON

#include "analyze.h"
#include <assert.h>
#include <raylib.h>
#include <stdint.h>

#define RESOURCE(path) "scene_src/" path

static inline float sc_range_average(float *frequencies, uint16_t from,
                                     uint16_t to) {
    assert(from < FREQUENCY_COUNT);
    assert(to < FREQUENCY_COUNT);
    if (from >= FREQUENCY_COUNT || to >= FREQUENCY_COUNT)
        return 0;

    float sum = 0;
    const float multiplier = 1.0 / (to - from + 1);
    for (uint16_t i = from; i <= to; i++) {
        sum += frequencies[i] * multiplier;
    }
    return sum;
}

static inline size_t sc_range_loudest_frequency(float *frequencies,
                                                uint16_t from, uint16_t to) {
    assert(from < FREQUENCY_COUNT);
    assert(to < FREQUENCY_COUNT);
    if (from >= FREQUENCY_COUNT || to >= FREQUENCY_COUNT)
        return 0;

    float max = 0;
    size_t max_i = 0;
    for (uint16_t i = from; i <= to; i++) {
        if (max < frequencies[i] * i) {
            max = frequencies[i] * i;
            max_i = i;
        }
    }
    return max_i / (to - from);
}

static inline void sc_rolling_average(float *out_value, float new,
                                      float window) {
    *out_value = (*out_value) * (window - 1) / window + new / window;
}

static inline void sc_decay(float *out_value, float new, float time) {
    if (new > *out_value)
        *out_value = new;
    if (*out_value > 0.00001)
        *out_value -= (1.0 / time) * GetFrameTime();
}

#endif
