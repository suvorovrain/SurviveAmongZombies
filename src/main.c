#include "engine/engine.h"
#include "engine/input.h"
#include "game.h"
#include "sprite_manager/sprite_manager.h"
#include "stb_ds.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static void update(Input *input, void *user_data);

int main(void) {
  srand(time(NULL));

#ifdef OS_WINDOWS
  printf("Running on Windows\n");
#elif defined(OS_LINUX)
  printf("Running on Linux\n");
#elif defined(OS_MACOS)
  printf("Running on macOS\n");
#endif

  sm_init();
  Game *game = game_create();
  if (!game) {
    return 1;
  }

  while (engine_begin_frame(game->engine, update, game)) {
    engine_render(game->engine, &(game->batch));
    // printf("FPS: %d\n", (int)engine_get_fps(engine));
    engine_end_frame(game->engine);
  }

  // sm_free();
  // game_free(game);
  return 0;
}

static void update(Input *input, void *user_data) {
  Game *game = user_data;

  game_update(game, input);
}
