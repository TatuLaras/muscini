#include "analyze.h"
#include "scene_common.h"
#include <assert.h>
#include <raylib.h>
#include <raymath.h>
#include <stdint.h>

#define MAX_DECAY_RATE 0.00005
#define AVERAGE_WINDOW 10
#define BG_AVERAGE_WINDOW 10
#define NORMAL_AVERAGE_WINDOW 2
#define LINE_WIDTH_LEFT 5
#define LINE_WIDTH_RIGHT 2
#define LINE_WIDTH_CUTOFF 140

int scene_init(void) {
    return 0;
}
void scene_deinit(void) {}

#define AVG_MAX_WINDOW 14
#define INTENSITY_WINDOW 10
void scene_update(AudioMetrics *metrics) {
    BeginDrawing();

    uint32_t screen_height = GetScreenHeight();
    uint32_t screen_width = GetScreenWidth();

    static float smooth_intensity = 0;
    sc_decay(&smooth_intensity, metrics->beat, 0.25);

    Color bg_color = (Color){smooth_intensity * 255, smooth_intensity * 255,
                             smooth_intensity * 255, 255};
    ClearBackground(bg_color);

    float min = 40;

    float horizontal = 0;
    uint16_t inspect = min;
    for (uint16_t i = 0; i < FREQUENCY_COUNT; i++) {
        uint8_t line_width =
            i < LINE_WIDTH_CUTOFF ? LINE_WIDTH_LEFT : LINE_WIDTH_RIGHT;
        horizontal += line_width;

        float freq_height = screen_height * metrics->frequencies[i];

        if (i == inspect)
            DrawLineEx((Vector2){horizontal, 0},
                       (Vector2){horizontal, screen_height}, line_width, BLACK);
        else
            DrawLineEx((Vector2){horizontal, screen_height - freq_height},
                       (Vector2){horizontal, screen_height}, line_width,
                       (Color){240, 20, 20, 255});

        float height = metrics->beat;

        DrawLineEx((Vector2){0, screen_height - screen_height * height * 32},
                   (Vector2){screen_width,
                             screen_height - screen_height * height * 32},
                   4, metrics->beat ? PINK : GREEN);
        DrawLineEx(
            (Vector2){0, screen_height - screen_height * 0.008 * 32},
            (Vector2){screen_width, screen_height - screen_height * 0.008 * 32},
            4, metrics->beat ? RED : WHITE);
    }

    EndDrawing();
}
