#include "analyze.h"
#include "audio_device.h"
#include "scenes.h"
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int main(int argc, char **argv) {
    char *scene = 0;
    int device_index = -1;

    for (int i = 1; i < argc; i++) {
        if (!strncmp(argv[i], "-d", 2) && argv[i][2] != 0) {
            device_index = strtol(argv[i] + 2, 0, 10);
        }
        scene = argv[1];
    }

    if (!scene) {
        fprintf(stderr, "Usage: muscini [scene object file]\n");
        return 1;
    }

    ma_context context;
    if (ma_context_init(NULL, 0, NULL, &context) != MA_SUCCESS) {
        printf("Failed to initialize context.\n");
        return 1;
    }

    ma_device_id device_id;
    if (get_audio_device_id(&context, device_index, &device_id)) {
        ma_context_uninit(&context);
        return 1;
    }

    analyze_init(&device_id);

    scenes_init();

    SetTraceLogLevel(LOG_WARNING);

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 450, "Muscini");
    SetTargetFPS(60);

    scenes_add(scene);

    AudioMetrics metrics = {0};

    while (!WindowShouldClose()) {
        analyze_get_metrics(&metrics);
        scenes_update_current(&metrics);
    }

    scenes_deinit();
    analyze_deinit();
    ma_context_uninit(&context);
    CloseWindow();

    return 0;
}
