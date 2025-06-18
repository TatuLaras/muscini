#include "scenes.h"

#define FIREWATCH_IMPLEMENTATION
#include "firewatch.h"

#include <assert.h>
#include <dlfcn.h>
#include <stdio.h>

VEC_IMPLEMENT(Scene, SceneVector, scenevec)

static SceneVector scenes = {0};
static size_t current_scene = 0;

static inline void ensure_init(void) {}

static inline void *get_symbol(void *handle, const char *name) {
    void *symbol_func = dlsym(handle, name);

    char *error = dlerror();
    if (error) {
        fprintf(
            stderr,
            "WARNING: Error processing scene object file (%s) symbols: %s\n",
            name, error);
        return 0;
    }
    return symbol_func;
}

static inline void deinit_scene(Scene *scene) {
    if (scene->deinit)
        (*scene->deinit)();

    if (scene->dl_handle) {
        dlclose(scene->dl_handle);
        scene->dl_handle = 0;
    }
}

static inline void load_scene(const char *filepath, size_t scene_index) {
    assert(scene_index < scenes.data_used);

    Scene *scene = scenes.data + scene_index;
    deinit_scene(scene);

    void *handle = dlopen(filepath, RTLD_NOW);
    if (!handle) {
        fprintf(stderr, "WARNING: Error opening scene object file: %s\n",
                dlerror());
        return;
    }
    dlerror();

    // Get symbols from shared object
    SceneUpdateFunction scene_update_function =
        (SceneUpdateFunction)get_symbol(handle, "scene_update");
    SceneInitFunction scene_init_function =
        (SceneInitFunction)get_symbol(handle, "scene_init");
    SceneDeinitFunction scene_deinit_function =
        (SceneDeinitFunction)get_symbol(handle, "scene_deinit");

    if (!scene_update_function || !scene_init_function ||
        !scene_deinit_function) {
        return;
    }

    (*scene_init_function)();

    *scene = (Scene){
        .update = scene_update_function,
        .init = scene_init_function,
        .deinit = scene_deinit_function,
        .dl_handle = handle,
    };
}

void scenes_init(void) {
    if (!scenes.data)
        scenes = scenevec_init();
}

void scenes_deinit(void) {
    for (size_t i = 0; i < scenes.data_used; i++)
        deinit_scene(scenes.data + i);
    scenevec_free(&scenes);
}

size_t scenes_add(const char *filepath) {
    ensure_init();

    size_t scene_index = scenevec_append(&scenes, (Scene){0});
    firewatch_new_file(filepath, scene_index, &load_scene, 0);
    return scene_index;
}

void scenes_update_current(AudioMetrics *metrics) {
    assert(current_scene < scenes.data_used);

    firewatch_check();
    (*scenes.data[current_scene].update)(metrics);
}
