
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
#include <../vendor/GTA-VI/include/engine/coordinates.h>
#include <../vendor/GTA-VI/include/engine/engine.h>
#include <../vendor/GTA-VI/include/engine/input.h>
#include <../vendor/GTA-VI/include/engine/map.h>
#include <math.h>
#include <stdio.h>

static void update(Input *input, void *user_data);

#define MAP_WIDTH 50
#define MAP_HEIGHT 50
#define STATIC_OBJ_COUNT 100

GameObject **get_game_objects_from_state(GlobalState *state) {
  GameObject **objects =
      calloc(sizeof(GameObject *),
             state->enemies_count + state->projectiles_count + 1);

  GameObject *player = calloc(sizeof(GameObject), 1);
  player->position = state->player.position;
  player->cur_sprite = &state->player.spritesheet.frames[0];

  objects[0] = player;

  for (size_t i = 0; i < state->enemies_count; i++) {
    GameObject *enemy = calloc(sizeof(GameObject), 1);
    enemy->position = state->enemies[i].position;
    enemy->cur_sprite = &state->enemies[i].spritesheet.frames[0];

    objects[i + 1] = enemy;
  }

  for (size_t i = 0; i < state->projectiles_count; i++) {
    GameObject *projectile = calloc(sizeof(GameObject), 1);
    projectile->position = state->projectiles[i].position;
    projectile->cur_sprite = &state->projectiles[i].spritesheet.frames[0];

    objects[i + state->enemies_count + 1] = projectile;
  }

  return objects;
}

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

  GlobalState globalState = init_global_state();

  Engine *engine = engine_create(800, 600, "Survie Among Zombies");
  Map *map = map_create(MAP_WIDTH, MAP_HEIGHT);
  engine_set_map(engine, map);

  GameObject **objects = get_game_objects_from_state(&globalState);

  engine_set_player(engine, objects[0]);

  void **global_state_objects = calloc(sizeof(void *), 3);
  global_state_objects[0] = &globalState;
  global_state_objects[1] = &objects;
  global_state_objects[2] = &engine;

  while (engine_begin_frame(engine, update, global_state_objects)) {
    engine_render(engine, objects,
                  1 + globalState.enemies_count +
                      globalState.projectiles_count);
    // printf("FPS: %d\n", (int)engine_get_fps(engine));
    engine_end_frame(engine);
  }

  // game_free(game);
  return 0;
}

static void update(Input *input, void *user_data) {
  GlobalState *globalState = ((void **)(user_data))[0];
  GameObject ***objects = ((void **)(user_data))[1];
  Engine **engine = ((void **)(user_data))[2];

  make_step(globalState, *input);

  *objects = get_game_objects_from_state(globalState);
  engine_set_player(*engine, (*objects)[0]);
}
