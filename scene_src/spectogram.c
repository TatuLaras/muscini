#include "analyze.h"
#include <assert.h>
#include <raylib.h>
#include <raymath.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_WIDTH 2

static uint32_t row = 0;
static uint8_t hold = 0;
static uint8_t slice_mode = 0;
static Image image = {
    .width = FREQUENCY_COUNT,
    .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,
    .mipmaps = 1,
};
static Texture texture = {0};
static float max = 0;
static float cut_height = 0;
static uint8_t slice_line_width = 2;

static float frozen_frequencies[FREQUENCY_COUNT] = {0};
static float frozen_max = 0;

int scene_init(void) {
    return 0;
}
void scene_deinit(void) {
    if (image.data)
        free(image.data);
}

static inline uint32_t intensity_color(float intensity) {
    intensity = intensity / max;
    intensity = fmax(0.0, intensity);
    intensity = fmin(1.0, intensity);
    uint8_t red = 0xff * intensity;
    uint8_t green = 0xff * intensity;
    uint8_t blue = 0xff * intensity;
    return red | green << 8 | blue << 16 | 0xff << 24;
}

void scene_update(AudioMetrics *metrics) {
    uint32_t screen_height = GetScreenHeight();
    uint32_t screen_width = GetScreenWidth();

    if (!image.data || IsWindowResized()) {
        if (image.data)
            free(image.data);
        image.data = (uint32_t *)calloc(1, FREQUENCY_COUNT * screen_width *
                                               sizeof(uint32_t));
        image.height = screen_width - 1;

        if (texture.id)
            UnloadTexture(texture);
        texture = LoadTextureFromImage(image);
        assert((texture.width == image.width));
        assert((texture.height == image.height));
    }

    if (IsKeyPressed(KEY_SPACE)) {
        hold = !hold;
        frozen_max = max;
        memcpy(frozen_frequencies, metrics->frequencies,
               FREQUENCY_COUNT * sizeof(float));
    }

    if (IsKeyPressed(KEY_C)) {
        Vector2 mouse = GetMousePosition();
        if (cut_height > 0)
            cut_height = 0;
        else
            cut_height = ((mouse.y) / screen_height) * FREQUENCY_COUNT;
    }

    if (IsKeyPressed(KEY_S))
        slice_mode = !slice_mode;

    if (IsKeyPressed(45)) // + on finnish layout
        slice_line_width++;
    if (IsKeyPressed(47)) // - on finnish layout
        slice_line_width--;
    if (IsKeyPressed(KEY_ZERO) &&
        (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)))
        slice_line_width = 2;

    if (slice_line_width == 0)
        slice_line_width = 1;

    if (!hold) {
        max = 0;
        for (uint32_t i = 0; i < FREQUENCY_COUNT; i++)
            if (max < metrics->frequencies[i])
                max = metrics->frequencies[i];

        for (uint32_t x = 0; x < FREQUENCY_COUNT; x++) {
            ((uint32_t *)image.data)[row * FREQUENCY_COUNT + x] =
                intensity_color(metrics->frequencies[x]);
        }
        row = (row + 1) % (image.height);
        UpdateTexture(texture, image.data);
    }

    BeginDrawing();

    ClearBackground(BLACK);

    for (uint16_t i = 0; i < FREQUENCY_COUNT; i++) {
        if (slice_mode) {
            float *slice;
            float maximum = max;
            if (hold) {
                slice = frozen_frequencies;
                maximum = frozen_max;
            } else {
                slice = metrics->frequencies;
            }

            DrawLineEx(
                (Vector2){i * slice_line_width,
                          screen_height - screen_height * (slice[i] / maximum)},
                (Vector2){i * slice_line_width, screen_height},
                slice_line_width, WHITE);
            continue;
        }

        Rectangle src = {
            .width = texture.width - cut_height,
            .height = texture.height,
        };
        Rectangle dest = {
            .width = screen_height,
            .height = screen_width,
            .y = screen_height,
            .x = screen_width - row,
        };

        Rectangle src_left = {
            .width = texture.width - cut_height,
            .height = texture.height - row,
            .y = row,
        };
        Rectangle dest_left = {
            .width = screen_height,
            .height = screen_width - row,
            .y = screen_height,
            .x = 0,
        };

        DrawTexturePro(texture, src, dest, (Vector2){0}, -90, WHITE);
        DrawTexturePro(texture, src_left, dest_left, (Vector2){0}, -90, WHITE);
    }

    EndDrawing();
}
