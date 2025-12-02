#ifndef _SCENE_COMMON
#define _SCENE_COMMON

#include "analyze.h"
#include <assert.h>
#include <raylib.h>
#include <stdint.h>

#define RESOURCE(path) "scene_src/" path

// Average relative amplitude of a range of frequencies (index).
// Useful for e.g. determing how much bass, mid-range or treble frequencies the
// audio contains.
static inline float sc_range_average(float *frequencies, uint16_t from_i,
                                     uint16_t to_i) {
    assert(from_i < FREQUENCY_COUNT);
    assert(to_i < FREQUENCY_COUNT);
    if (from_i >= FREQUENCY_COUNT || to_i >= FREQUENCY_COUNT)
        return 0;

    float sum = 0;
    const float multiplier = 1.0 / (to_i - from_i + 1);
    for (uint16_t i = from_i; i <= to_i; i++) {
        sum += frequencies[i] * multiplier;
    }
    return sum;
}

// Loudest relative amplitude in a range of frequencies (index).
static inline size_t
sc_range_loudest_frequency(float *frequencies, uint16_t from_i, uint16_t to_i) {
    assert(from_i < FREQUENCY_COUNT);
    assert(to_i < FREQUENCY_COUNT);
    if (from_i >= FREQUENCY_COUNT || to_i >= FREQUENCY_COUNT)
        return 0;

    float max = 0;
    size_t max_i = 0;
    for (uint16_t i = from_i; i <= to_i; i++) {
        if (max < frequencies[i] * i) {
            max = frequencies[i] * i;
            max_i = i;
        }
    }
    return max_i / (to_i - from_i);
}

// Takes a rolling average with window size of `window` of values in `new`, and
// writes that average into `out_value`. Basically a low-pass filter, useful for
// smoothing out jittery values.
static inline void sc_rolling_average(float *out_value, float new,
                                      float window) {
    *out_value = (*out_value) * (window - 1) / window + new / window;
}

// Instantly reacts to new values of `new`, but only reducing the value
// according to the decay time set by `time`, writing it to `out_value`.
static inline void sc_decay(float *out_value, float new, float time) {
    if (new > *out_value)
        *out_value = new;
    if (*out_value > 0.00001)
        *out_value -= (1.0 / time) * GetFrameTime();
}

#endif
