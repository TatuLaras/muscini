#include "analyze.h"
#include "audio_device.h"
#include "scenes.h"
#include <raylib.h>
#include <stdio.h>

int main(int argc, char **argv) {
    char *scene = 0;
    if (argc < 2) {
        fprintf(stderr, "Usage: muscini [scene object file]\n");
        return 1;
    }

    scene = argv[1];
    if (!scene)
        return 1;

    // Audio context and device selection

    ma_context context;
    if (ma_context_init(NULL, 0, NULL, &context) != MA_SUCCESS) {
        printf("Failed to initialize context.\n");
        return -2;
    }

    ma_device_info device_info;
    if (audiodevice_get_interactive(&context, &device_info)) {
        fprintf(stderr, "ERROR: Getting device failed.\n");
        return 1;
    }

    analyze_init(&device_info.id);

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
