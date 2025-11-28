
#include "game.h"
#include "static_objs.h"
#include "stb_ds.h"
#include <../vendor/GTA-VI/include/engine/coordinates.h>
#include <../vendor/GTA-VI/include/engine/engine.h>
#include <../vendor/GTA-VI/include/engine/input.h>
#include <../vendor/GTA-VI/include/engine/map.h>
#include <math.h>
#include <stb_ds.h>
#include <stdio.h>

static void update(Input *input, void *user_data);

int main(void) {
  printf("5 + 3 = %d\n", add(5, 3));

#ifdef OS_WINDOWS
  printf("Running on Windows\n");
#elif defined(OS_LINUX)
  printf("Running on Linux\n");
#elif defined(OS_MACOS)
  printf("Running on macOS\n");
#endif

  Game *game = game_create();
  if (!game) {
    return 1;
  }
  Engine *engine = game->engine;
  while (engine_begin_frame(engine, update, game)) {
    engine_render(engine, game->objects, arrlen(game->objects));
    printf("FPS: %d\n", (int)engine_get_fps(engine));
    engine_end_frame(engine);
  }

  game_free(game);
  return 0;
}

static void update(Input *input, void *user_data) {
  Game *game = (Game *)user_data;

  game_update(game, input);
}
