/*

----- Example visualization -----


To provide hot-reloading for external resources such as shaders I recommend
using firewatch inside this visualization.

#define FIREWATCH_IMPLEMENTATION
#include "firewatch.h"

An example of this can be found in scene_src/rabbit_hole.c

Also see firewatch README.md for usage information:
https://github.com/TatuLaras/firewatch?tab=readme-ov-file#load-files-from-current-thread

*/

// Visualizations are written using raylib.
// For more information on how to use raylib see https://www.raylib.com/
#include <raylib.h>

// The scene_common header file contains some utility functions that are common
// to many visualizations. See this file for more information on what is
// included.
#include "scene_common.h"

#define LINE_WIDTH 3

// For every visualization, the following three functions need to be defined.

// Runs when your visualization is loaded.
// Here you can initialize stuff that your visualization uses.
int scene_init(void) {
    return 0;
}

// Runs when your visualization is unloaded.
// Here you can initialize things that your visualization needs.
void scene_deinit(void) {}

/*
Runs each frame.
Draw your visualization here.

The param `metrics` contains the musical information that you can utilize in
your visualization.

typedef struct {
    // Relative amplitudes of frequencies in the signal.
    float frequencies[FREQUENCY_COUNT];
    // Will be 1.0 if a beat has just occurred, otherwise 0.0.
    float beat;
} AudioMetrics;

*/
void scene_update(AudioMetrics *metrics) {

    // Remember to draw between BeingDrawing() and EndDrawing()
    BeginDrawing();

    // Flash background with white on each "beat", with some fade out time.
    static float bg_whiteness = 0;
    sc_decay(&bg_whiteness, metrics->beat, 0.25);

    ClearBackground((Color){bg_whiteness * 255, bg_whiteness * 255,
                            bg_whiteness * 255, 255});

    // Draw vertical lines for each frequency

    uint32_t screen_height = GetScreenHeight();
    float horizontal = 0;

    for (uint16_t i = 0; i < FREQUENCY_COUNT; i++) {

        horizontal += LINE_WIDTH;

        float freq_height = screen_height * metrics->frequencies[i];
        DrawLineEx((Vector2){horizontal, screen_height - freq_height},
                   (Vector2){horizontal, screen_height}, LINE_WIDTH,
                   (Color){240, 20, 20, 255});
    }

    EndDrawing();
}
