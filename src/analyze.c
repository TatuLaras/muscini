#include "analyze.h"
#include "fft.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#define MAXIMUM_VALUE_DECAY_RATE 0.00002
#define SMOOTHING_AVERAGING_WINDOW 2
#define SMOOTH_REALTIME_WINDOW 12
#define BEAT_TRESHOLD 0.008

static float samples_l[INPUT_SIZE] = {0};
static size_t samples_l_used = 0;
static int get_beat_from_midi = 0;
static int beat_received = 0;

static FFTTransformer *transformer = 0;

void analyze_feed_frames(float *frames, uint32_t frame_count,
                         uint8_t channels) {
    for (size_t i = 0; i < frame_count * channels; i += channels)
        samples_l[samples_l_used++ % INPUT_SIZE] = frames[i];
}

void analyze_init(void) {
    transformer = create_fft_transformer(INPUT_SIZE, FFT_SCALED_OUTPUT);
    assert(transformer);
}

void analyze_deinit(void) {
    free_fft_transformer(transformer);
}

// From fft.c
static inline float hanning(int i, int nn) {
    return (0.5 * (1.0 - cosf(2.0 * M_PI * (float)i / (float)(nn - 1))));
}

static inline void rolling_average(float *out_value, float new, float window) {
    *out_value = (*out_value) * (window - 1) / window + new / window;
}

void analyze_get_metrics(AudioMetrics *out_metrics) {
    assert(transformer);
    assert(out_metrics);

    static float temp_buffer[INPUT_SIZE] = {0};

    // A slowly decaying maximum value to normalize frequency data
    static float maximum = 0;
    maximum -= MAXIMUM_VALUE_DECAY_RATE;
    if (maximum < 0.001)
        maximum = 0.001;

    // Copy data while applying windowing function
    for (size_t i = 0; i < INPUT_SIZE; i++)
        temp_buffer[i] = samples_l[i] * hanning(i, INPUT_SIZE);

    static float smooth_realtime_maximum = 0;
    static float rapid_realtime_maximum = 0;
    float realtime_maximum = 0;

    fft_forward(transformer, temp_buffer);

    for (size_t i = 0; i < FREQUENCY_COUNT; i++) {
        assert((i * 2 + 1) < INPUT_SIZE);

        float cos_comp = temp_buffer[i * 2];
        float sin_comp = temp_buffer[i * 2 + 1];
        float mag = sqrt((cos_comp * cos_comp) + (sin_comp * sin_comp));

        out_metrics->frequencies[i] = (mag / maximum);

        if (maximum < mag)
            maximum = mag;
        if (i < 5 && realtime_maximum < mag)
            realtime_maximum = mag;
    }

    rolling_average(&smooth_realtime_maximum, realtime_maximum, 10);
    rolling_average(&rapid_realtime_maximum, realtime_maximum, 8);

    if (get_beat_from_midi) {
        out_metrics->beat = beat_received;
        beat_received = 0;
    } else {
        out_metrics->beat =
            ((rapid_realtime_maximum - smooth_realtime_maximum) / maximum) >
            BEAT_TRESHOLD;
    }
}

void analyze_trigger_beat(void) {
    beat_received = 1;
}

void analyze_set_beat_triggering_mode(int use_manual_triggering) {
    get_beat_from_midi = use_manual_triggering;
}
