#ifndef _SCENE_LOADING
#define _SCENE_LOADING

#include "analyze.h"
#include "vec.h"

typedef void (*SceneUpdateFunction)(AudioMetrics *metrics);
typedef int (*SceneInitFunction)(void);
typedef void (*SceneDeinitFunction)(void);

typedef struct {
    SceneUpdateFunction update;
    SceneInitFunction init;
    SceneDeinitFunction deinit;
    void *dl_handle;
} Scene;

VEC_DECLARE(Scene, SceneVector, scenevec)

// Call this before any other functions in this unit.
void scenes_init(void);
// Deinitialize memory used by this unit.
void scenes_deinit(void);

// Calls the rendering / update function of the currently active and loaded
// scene.
void scenes_update_current(AudioMetrics *metrics);
// Adds and loads a new scene shared object file at `filepath` to the pool of
// scenes. Returns index / handle of the newly added scene, or 0 on failure.
size_t scenes_add(const char *filepath);

#endif
