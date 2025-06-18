#include "analyze.h"
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

static float max_frequencies[FREQ_FREQUENCY_COUNT] = {0};
static float average_frequencies[FREQ_FREQUENCY_COUNT] = {0};
static float normal_frequencies[FREQ_FREQUENCY_COUNT] = {0};

int scene_init(void) {
    return 0;
}
void scene_deinit(void) {}

void scene_update(AudioMetrics *metrics) {
    BeginDrawing();

    uint32_t screen_height = GetScreenHeight();

    static float maximum = 0;
    static float realtime_max_smooth = 0;
    float realtime_max = 0;
    maximum -= MAX_DECAY_RATE * 0.5;
    if (maximum < 0.001)
        maximum = 0.001;

    float val = realtime_max_smooth / maximum;
    Color bg_color = (Color){val * 70, val * 70, val * 70, 255};
    ClearBackground(bg_color);

    float horizontal = 0;
    for (uint16_t i = 0; i < FREQ_FREQUENCY_COUNT; i++) {
        uint8_t line_width =
            i < LINE_WIDTH_CUTOFF ? LINE_WIDTH_LEFT : LINE_WIDTH_RIGHT;
        horizontal += line_width;
        normal_frequencies[i] = normal_frequencies[i] *
                                    (NORMAL_AVERAGE_WINDOW - 1) /
                                    NORMAL_AVERAGE_WINDOW +
                                metrics->frequencies[i] / NORMAL_AVERAGE_WINDOW;

        if (realtime_max < normal_frequencies[i])
            realtime_max = normal_frequencies[i];

        if (max_frequencies[i] < normal_frequencies[i])
            max_frequencies[i] = normal_frequencies[i];
        if (maximum < average_frequencies[i])
            maximum = average_frequencies[i];

        max_frequencies[i] -= MAX_DECAY_RATE;

        float freq_height = screen_height * (normal_frequencies[i] / maximum);
        float max_freq_height = screen_height * (max_frequencies[i] / maximum);

        DrawLineEx(
            (Vector2){horizontal, screen_height - max_freq_height},
            (Vector2){horizontal, screen_height}, line_width,
            (Color){bg_color.r + 20, bg_color.g + 20, bg_color.b + 20, 255});
        DrawLineEx((Vector2){horizontal, screen_height - freq_height},
                   (Vector2){horizontal, screen_height}, line_width,
                   (Color){240, 20, 20, 255});
    }

    realtime_max_smooth =
        realtime_max_smooth * (BG_AVERAGE_WINDOW - 1) / BG_AVERAGE_WINDOW +
        realtime_max / BG_AVERAGE_WINDOW;

    horizontal = 0;
    for (uint16_t i = 0; i < FREQ_FREQUENCY_COUNT; i++) {
        uint8_t line_width =
            i < LINE_WIDTH_CUTOFF ? LINE_WIDTH_LEFT : LINE_WIDTH_RIGHT;
        horizontal += line_width;
        average_frequencies[i] =
            average_frequencies[i] * (AVERAGE_WINDOW - 1) / AVERAGE_WINDOW +
            metrics->frequencies[i] / AVERAGE_WINDOW;

        float avg_freq_height =
            screen_height * (average_frequencies[i] / maximum);
        float avg_next_freq_height;
        if (i == FREQ_FREQUENCY_COUNT - 1)
            avg_next_freq_height = avg_freq_height;
        else
            avg_next_freq_height =
                screen_height * (average_frequencies[i + 1] / maximum);

        DrawLineEx((Vector2){horizontal, screen_height - avg_freq_height},
                   (Vector2){horizontal + line_width,
                             screen_height - avg_next_freq_height},
                   2, WHITE);
    }

    EndDrawing();
}
