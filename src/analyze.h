#ifndef _FREQ
#define _FREQ

#include "miniaudio.h"
#include <stdint.h>
#define INPUT_SIZE 1024
#define FREQUENCY_COUNT 512 // (INPUT_SIZE / 2)

/*
Analyze frequency content of captured audio using fast fourier transform.
*/

typedef struct {
    // Relative amplitudes of frequencies in the signal.
    float frequencies[FREQUENCY_COUNT];
    // Will be 1.0 if a beat has just occurred, otherwise 0.0.
    float beat;
} AudioMetrics;

typedef enum {
    BEAT_DETECTION_AUDIO = 0,
    BEAT_DETECTION_MIDI,
} BeatDetectMode;

// Initializes this module and starts capturing audio data for analysis.
void analyze_init(void);
// Writes metrics of the playing audio such as frequency content into
// `out_metrics`.
void analyze_get_metrics(AudioMetrics *out_metrics);
// Frees this module.
void analyze_deinit(void);
// Feeds `frames` to analyze metrics from (see analyze_get_metrics()).
void analyze_feed_frames(float *frames, uint32_t frame_count, uint8_t channels);
// Manually trigger a beat metric to be pulsed. Requires manual triggering mode
// to be enabled via analyze_beat_set_manual_triggering_mode().
void analyze_trigger_beat(void);
// Set whether or not the beat metric should be manually triggered via
// analyze_trigger_beat() instead of infering from audio frequency data.
void analyze_set_beat_triggering_mode(int use_manual_triggering);

#endif
