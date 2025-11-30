
#include "engine/coordinates.h"
#include "engine/engine.h"
#include "engine/input.h"
#include "engine/map.h"
#include "game.h"
#include "state/state.h"
#include "static_objs.h"
#include "stb_ds.h"
#include "units/units.h"
#include "utils/utils.h"
#include <engine/coordinates.h>
#include <engine/engine.h>
#include <engine/input.h>
#include <engine/map.h>
#include <math.h>
#include <stdio.h>

static void update(Input *input, void *user_data);

int main(void) {
#ifdef OS_WINDOWS
  printf("Running on Windows\n");
#elif defined(OS_LINUX)
  printf("Running on Linux\n");
#elif defined(OS_MACOS)
  printf("Running on macOS\n");
#endif

  // Game *game = game_create();
  // if (!game) {
  //   return 1;
  // }

  Game *game = game_create();

  while (engine_begin_frame(game->engine, update, game)) {
    engine_render(game->engine, &(game->batch));
    // printf("FPS: %d\n", (int)engine_get_fps(engine));
    engine_end_frame(game->engine);
  }

  // game_free(game);
  return 0;
}

static void update(Input *input, void *user_data) {
  Game *game = user_data;

  game_update(game, input);
}
