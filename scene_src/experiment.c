#include "scene_common.h"

#define FIREWATCH_IMPLEMENTATION
#include "firewatch.h"

#include "analyze.h"

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
static int loc_screen_width = 0;
static int loc_screen_height = 0;
static int loc_time = 0;
static int loc_beat = 0;
static int frequencies_locs[FREQUENCY_COUNT] = {0};

static RenderTexture scene_render_target = {0};
static RenderTexture shader_render_target = {0};

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
    firewatch_new_file(RESOURCE("experiment/main.frag"), 0, &load_shader, 0);
    return 0;
}

void scene_deinit(void) {
    if (shader.id)
        UnloadShader(shader);

    if (scene_render_target.id)
        UnloadRenderTexture(scene_render_target);

    if (shader_render_target.id)
        UnloadRenderTexture(shader_render_target);

    firewatch_reset();
}

void scene_update(AudioMetrics *metrics) {
    firewatch_check();

    if (IsKeyPressed(KEY_SPACE))
        firewatch_reset();

    if (!scene_render_target.id || IsWindowResized()) {
        if (scene_render_target.id)
            UnloadRenderTexture(scene_render_target);
        scene_render_target =
            LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    }
    if (!shader_render_target.id || IsWindowResized()) {
        if (shader_render_target.id)
            UnloadRenderTexture(shader_render_target);
        shader_render_target =
            LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    }

    // Shader locations

    static float beat = 0;
    sc_decay(&beat, metrics->beat, 0.25);

    static int shader_width_height_updated = 0;
    if (IsWindowResized() || !shader_width_height_updated) {
        float height = GetScreenHeight();
        float width = GetScreenWidth();
        SetShaderValue(shader, loc_screen_width, &width, SHADER_UNIFORM_FLOAT);
        SetShaderValue(shader, loc_screen_height, &height,
                       SHADER_UNIFORM_FLOAT);
    }

    float time = GetTime();
    SetShaderValue(shader, loc_time, &time, SHADER_UNIFORM_FLOAT);

    uint32_t screen_height = GetScreenHeight();
    uint32_t screen_width = GetScreenWidth();

    static float progress = 0;

    progress = fmod(progress + beat, 100);
    SetShaderValue(shader, loc_beat, &progress, SHADER_UNIFORM_FLOAT);

    for (uint16_t i = 0; i < FREQUENCY_COUNT; i++) {
        SetShaderValue(shader, frequencies_locs[i], metrics->frequencies + i,
                       SHADER_UNIFORM_FLOAT);
    }

#define WINDOW 2
#define BAND_COUNT 10
    static float bands[BAND_COUNT] = {0};

    sc_rolling_average(bands + 0, sc_range_average(metrics->frequencies, 0, 6),
                       WINDOW);

    BeginTextureMode(scene_render_target);

    float white_width = 80;
    // DrawRectangle(0, 0, red_width * bands[0], red_width * bands[0], RED);
    if (beat > 0.6)
        DrawRectangle(white_width, screen_height * 0.5, white_width * beat,
                      white_width * beat, RED);
    DrawRectangle(0, screen_height * 0.5, white_width, white_width, WHITE);
    // DrawCircle(white_width, screen_height * 0.5, white_width * 0.5, WHITE);

    EndTextureMode();

    BeginTextureMode(shader_render_target);
    if (shader.id) {
        DrawTexturePro(
            scene_render_target.texture,
            (Rectangle){.width = scene_render_target.texture.width,
                        .height = scene_render_target.texture.height},
            (Rectangle){.width = screen_width, .height = screen_height},
            (Vector2){0}, 0, WHITE);
    }
    EndTextureMode();

    BeginTextureMode(scene_render_target);

    ClearBackground((Color){0});
    if (shader.id) {
        BeginShaderMode(shader);
        DrawTexturePro(
            shader_render_target.texture,
            (Rectangle){.width = shader_render_target.texture.width,
                        .height = shader_render_target.texture.height},
            (Rectangle){.width = screen_width, .height = screen_height},
            (Vector2){0}, 0, WHITE);
        EndShaderMode();
    }
    EndTextureMode();

    BeginDrawing();
    DrawTexturePro(scene_render_target.texture,
                   (Rectangle){.width = scene_render_target.texture.width,
                               .height = scene_render_target.texture.height},
                   (Rectangle){.width = screen_width, .height = screen_height},
                   (Vector2){0}, 0, WHITE);
    EndDrawing();
}
