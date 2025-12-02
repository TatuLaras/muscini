#include "pulseaudio_init.h"

#include "analyze.h"
#include "audio_device.h"
#include "miniaudio.h"

#include <stdio.h>

#define FORMAT ma_format_f32
#define CHANNELS 2
#define SAMPLE_RATE 48000

static ma_device audio_device;
static ma_context audio_context;

static void data_callback(ma_device *device_context, void *output,
                          const void *input, ma_uint32 frame_count) {
    analyze_feed_frames((float *)input, frame_count, CHANNELS);
    (void)device_context;
    (void)output;
}

static inline int get_audio_device_id(ma_context *context, int device_index,
                                      ma_device_id *out_device_id) {

    ma_device_info device_info;

    if (device_index >= 0) {
        if (audiodevice_get_by_index(context, device_index, &device_info))
            return 1;
    } else if (audiodevice_get_interactive(context, &device_info))
        return 1;

    printf("INFO: Using audio device %s.\n", device_info.name);
    if (out_device_id)
        *out_device_id = device_info.id;

    return 0;
}

int pulseaudio_init(int device_index) {
    if (ma_context_init(NULL, 0, NULL, &audio_context) != MA_SUCCESS) {
        printf("Failed to initialize context.\n");
        return 1;
    }

    ma_device_id device_id;
    if (get_audio_device_id(&audio_context, device_index, &device_id)) {
        ma_context_uninit(&audio_context);
        return 1;
    }
    ma_device_config deviceConfig =
        ma_device_config_init(ma_device_type_capture);
    deviceConfig.capture.format = FORMAT;
    deviceConfig.capture.channels = CHANNELS;
    deviceConfig.capture.pDeviceID = &device_id;
    deviceConfig.sampleRate = SAMPLE_RATE;
    deviceConfig.dataCallback = &data_callback;

    if (ma_device_init(0, &deviceConfig, &audio_device) != MA_SUCCESS) {
        fprintf(stderr, "ERROR: Failed to initialize capture device.\n");
        return 1;
    }

    if (ma_device_start(&audio_device) != MA_SUCCESS) {
        ma_device_uninit(&audio_device);
        fprintf(stderr, "ERROR: Failed to start device.\n");
        return 1;
    }

    return 0;
}

void pulseaudio_deinit(void) {
    ma_device_uninit(&audio_device);
    ma_context_uninit(&audio_context);
}
