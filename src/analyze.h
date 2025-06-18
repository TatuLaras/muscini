#ifndef _FREQ
#define _FREQ

#include "miniaudio.h"
#define FREQ_INPUT_SIZE 1024
#define FREQ_FREQUENCY_COUNT (FREQ_INPUT_SIZE / 2)

/*
Analyze frequency content of captured audio using fast fourier transform.
*/

typedef struct {
    float frequencies[FREQ_FREQUENCY_COUNT];
} AudioMetrics;

// Initializes this module and starts capturing audio data for analysis.
int analyze_init(const ma_device_id *device_id);
// Writes metrics of the playing audio such as frequency content into
// `out_metrics`.
void analyze_get_metrics(AudioMetrics *out_metrics);
// Gets the frequency in Hz associated with a given index of the `frequencies`
// table of AudioMetrics.
float analyze_get_frequency_value(size_t frequency_index);
// Frees this module.
void analyze_deinit(void);

#endif
