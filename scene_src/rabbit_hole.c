#include "scene_common.h"

#define FIREWATCH_IMPLEMENTATION
#include "analyze.h"
#include "firewatch.h"

#include <assert.h>
#include <raylib.h>
#include <raymath.h>
#include <stdint.h>
#include <stdio.h>

//  TODO: A lot of these frequency analysis should be refactored to a common
//  library.

#define MAX_DECAY_RATE 0.00001
#define AVERAGE_WINDOW 10
#define BG_AVERAGE_WINDOW 12
#define NORMAL_AVERAGE_WINDOW 3

static Shader shader = {0};
static Texture texture = {0};
static int loc_screen_width = 0;
static int loc_screen_height = 0;
static int loc_time = 0;
static int loc_beat = 0;
static int frequencies_locs[FREQUENCY_COUNT] = {0};

static inline void load_shader(const char *filepath, uint64_t cookie) {
    if (shader.id)
        UnloadShader(shader);

    shader = LoadShader(0, filepath);
    if (!shader.id) {
        fprintf(stderr, "Could not load shader.\n");
        return;
    }

    loc_screen_width = GetShaderLocation(shader, "screen_width");
    loc_screen_height = GetShaderLocation(shader, "screen_height");
    loc_time = GetShaderLocation(shader, "time");
    loc_beat = GetShaderLocation(shader, "progress");

    for (size_t i = 0; i < FREQUENCY_COUNT; i++) {
        frequencies_locs[i] =
            GetShaderLocation(shader, TextFormat("frequencies[%i]", i));
    }

    (void)cookie;
}

int scene_init(void) {
    texture = LoadTexture(RESOURCE("rabbit_hole/eye.png"));
    firewatch_new_file(RESOURCE("rabbit_hole/main.frag"), 0, &load_shader, 0);
    return 0;
}

void scene_deinit(void) {
    if (shader.id)
        UnloadShader(shader);

    if (texture.id)
        UnloadTexture(texture);

    firewatch_reset();
}

void scene_update(AudioMetrics *metrics) {
    firewatch_check();

    // Shader locations

    static int shader_width_height_updated = 0;
    if (IsWindowResized() || !shader_width_height_updated) {
        float height = GetScreenHeight();
        float width = GetScreenWidth();
        SetShaderValue(shader, loc_screen_width, &width, SHADER_UNIFORM_FLOAT);
        SetShaderValue(shader, loc_screen_height, &height,
                       SHADER_UNIFORM_FLOAT);
    }

    float time = GetTime();
    float time2 = GetTime() + 5;
    SetShaderValue(shader, loc_time, &time, SHADER_UNIFORM_FLOAT);
    SetShaderValue(shader, loc_beat, &time2, SHADER_UNIFORM_FLOAT);

    uint32_t screen_height = GetScreenHeight();
    uint32_t screen_width = GetScreenWidth();

    static float progress = 0;
    static float beat = 0;
    sc_rolling_average(&beat, metrics->beat * 10, 10);

    progress = fmod(progress + beat, 100);
    SetShaderValue(shader, loc_beat, &progress, SHADER_UNIFORM_FLOAT);

    for (uint16_t i = 0; i < FREQUENCY_COUNT; i++) {
        SetShaderValue(shader, frequencies_locs[i], metrics->frequencies + i,
                       SHADER_UNIFORM_FLOAT);
    }

    BeginDrawing();
    ClearBackground(BLACK);
    if (shader.id) {
        BeginShaderMode(shader);
        DrawTexturePro(
            texture,
            (Rectangle){.width = texture.width, .height = texture.height},
            (Rectangle){.width = screen_width, .height = screen_height},
            (Vector2){0}, 0, WHITE);
        EndShaderMode();
    }
    EndDrawing();
}
