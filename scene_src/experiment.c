#include "scene_common.h"

#define FIREWATCH_IMPLEMENTATION
#include "firewatch.h"

#include "analyze.h"

#include <assert.h>
#include <raylib.h>
#include <raymath.h>
#include <stdint.h>

#define MAX_DECAY_RATE 0.00001
#define AVERAGE_WINDOW 10
#define BG_AVERAGE_WINDOW 12
#define NORMAL_AVERAGE_WINDOW 3

static Shader shader = {0};
static int loc_screen_width = 0;
static int loc_screen_height = 0;
static int loc_time = 0;
static int loc_beat = 0;

static RenderTexture scene_render_target = {0};
static RenderTexture shader_render_target = {0};

static inline void load_shader(const char *filepath, uint64_t cookie) {
    if (shader.id)
        UnloadShader(shader);

    shader = LoadShader(0, filepath);

    loc_screen_width = GetShaderLocation(shader, "screen_width");
    loc_screen_height = GetShaderLocation(shader, "screen_height");
    loc_time = GetShaderLocation(shader, "time");
    loc_beat = GetShaderLocation(shader, "beat");

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

    if (IsWindowResized()) {
        float height = GetScreenHeight();
        float width = GetScreenWidth();
        SetShaderValue(shader, loc_screen_width, &width, SHADER_UNIFORM_FLOAT);
        SetShaderValue(shader, loc_screen_height, &height,
                       SHADER_UNIFORM_FLOAT);
    }

    float time = GetTime();
    SetShaderValue(shader, loc_time, &time, SHADER_UNIFORM_FLOAT);

    static float beat = 0;
    sc_decay(&beat, metrics->beat, 1.5);
    SetShaderValue(shader, loc_beat, &beat, SHADER_UNIFORM_FLOAT);

    uint32_t screen_height = GetScreenHeight();
    uint32_t screen_width = GetScreenWidth();

    BeginTextureMode(scene_render_target);

    EndTextureMode();

    BeginTextureMode(shader_render_target);
    if (shader.id) {
        DrawTexturePro(
            scene_render_target.texture,
            (Rectangle){.width = scene_render_target.texture.width,
                        .height = scene_render_target.texture.height},
            (Rectangle){.width = screen_width,
                        .height = screen_height,
                        .x = sin(time * 10) * 100},
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
