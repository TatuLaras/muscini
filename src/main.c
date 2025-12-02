#include "analyze.h"
#include "clargs.h"
#include "jack_init.h"
#include "pulseaudio_init.h"
#include "scenes.h"

#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
    char *scene = 0;
    char *device_index = 0;
    int use_jack = 0;

    CLARG {
        help("Usage: muscini [file]\n\n\
Options:\n\
--help, -h\t\tPrint this message and exit.\n\
--jack\t\t\tStart as a JACK client.\n\
-d [index]\t\tSpecify a device to use for audio capture in non-JACK mode\n\n");

        flag(use_jack, "--jack");
        flag_value(device_index, "-d");

        file(scene);
    }

    if (!scene) {
        fprintf(stderr, "Usage: muscini [file]\n\
Use --help for more information.\n");
        return 1;
    }

    analyze_init();

    int result = 0;

    if (use_jack) {
        result = jack_init();
    } else {
        int device_i = -1;
        if (device_index)
            device_i = strtol(device_index, 0, 10);
        result = pulseaudio_init(device_i);
    }

    if (result) {
        fprintf(stderr, "ERROR: could not initialize audio source, exiting.\n");
        return 1;
    }

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

    if (use_jack)
        jack_deinit();
    else
        pulseaudio_deinit();

    scenes_deinit();
    analyze_deinit();
    CloseWindow();

    return 0;
}
