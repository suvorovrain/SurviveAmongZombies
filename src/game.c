#include "game.h"
#include "dyn_objs.h"
#include "state/state.h"
#include "static_objs.h"
#include <engine/coordinates.h>
#include <engine/engine.h>
#include <engine/input.h>
#include <engine/map.h>
#include <stb_ds.h>
#include <stdlib.h>

#define MAP_WIDTH 40
#define MAP_HEIGHT 40
#define STATIC_OBJ_COUNT 100

void game_free(Game *game);

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

Game *game_create() {
  Game *game = calloc(1, sizeof(Game));
  if (!game) {
    return NULL;
  }

  Engine *engine = engine_create(800, 600, "Survie Among Zombies");
  game->engine = engine;
  if (!engine) {
    game_free(game);
    return NULL;
  }

  TilesInfo ti = {0};
  ti.tile_sprites = calloc(1, sizeof(Sprite));
  ti.tile_sprites[0] = load_sprite("assets/static/grass.png", 4.0f);
  printf("%p\n", ti.tile_sprites[0]);
  ti.sprite_count = 1;
  ti.tiles = calloc(MAP_WIDTH * MAP_HEIGHT, sizeof(uint32_t));
  ti.sides_height = 32;

  Map *map = map_create(MAP_WIDTH, MAP_HEIGHT, ti);
  if (!map) {
    game_free(game);
    map_free(map);
    return NULL;
  };
  engine_set_map(engine, map);

  GlobalState globalState = init_global_state(map);
  GameObject **objects = get_game_objects_from_state(&globalState);

  engine_set_player(engine, objects[0]);

  game->batch = (RenderBatch){.obj_count = 1 + game->state.enemies_count +
                                           game->state.enemies_count,
                              .ui_count = 0,
                              .uis = NULL};
  game->engine = engine;
  game->fonts = NULL;
  game->map = map;
  game->state = globalState;

  return game;
}

void game_free(Game *game) {
  if (!game)
    return;

  free(game);
}

void game_update(Game *game, Input *input) {
  if (!game || !input)
    return;

  make_step(&game->state, *input);
  GameObject **objects = get_game_objects_from_state(&(game->state));
  game->batch = (RenderBatch){.obj_count = 1 + game->state.enemies_count +
                                           game->state.projectiles_count,
                              .objs = objects,
                              .ui_count = 0,
                              .uis = NULL};
  engine_set_player(game->engine, game->batch.objs[0]);
}
