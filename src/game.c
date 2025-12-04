#include "game.h"
#include "state/state.h"
#include "static_objs.h"
#include "ui_manager/ui_manager.h"
#include "units/units.h"
#include <engine/coordinates.h>
#include <engine/engine.h>
#include <engine/input.h>
#include <engine/map.h>
#include <stb_ds.h>
#include <stdlib.h>

#define MAP_WIDTH 150
#define MAP_HEIGHT 150
#define STATIC_OBJ_COUNT 100

void game_free(Game *game);

GameObject **get_game_objects_from_state(GlobalState *state) {
  GameObject **objects =
      calloc(sizeof(GameObject *),
             state->enemies_count + state->projectiles_count + 1);

  GameObject *player = calloc(sizeof(GameObject), 1);
  player->position = state->player.position;
  player->cur_sprite = &state->player.current_sprite;

  objects[0] = player;

  for (size_t i = 0; i < state->enemies_count; i++) {
    GameObject *enemy = calloc(sizeof(GameObject), 1);
    enemy->position = state->enemies[i].position;
    enemy->cur_sprite = &state->enemies[i].current_sprite;

    objects[i + 1] = enemy;
  }

  for (size_t i = 0; i < state->projectiles_count; i++) {
    GameObject *projectile = calloc(sizeof(GameObject), 1);
    projectile->position = state->projectiles[i].position;
    projectile->cur_sprite = &state->projectiles[i].current_sprite;

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

  // Load fonts
  if (TTF_Init() != 0) {
    fprintf(stderr, "Failed to initialize TTF: %s\n", TTF_GetError());
    game_free(game);
    return NULL;
  }
  game->fonts = calloc(sizeof(TTF_Font *), FONT_COUNT);
  game->fonts[FONT_DEJAVU] = TTF_OpenFont("fonts/DejaVuSans.ttf", 20);
  game->fonts[FONT_PIXELOID_MONO] = TTF_OpenFont("fonts/PixeloidMono.ttf", 20);
  game->fonts[FONT_PIXELOID_SANS] = TTF_OpenFont("fonts/PixeloidSans.ttf", 20);
  game->fonts[FONT_PIXELOID_SANS_BOLD] =
      TTF_OpenFont("fonts/PixeloidSansBold.ttf", 120);
  game->fonts[FONT_LEVEL_MENU_OPTION] =
      TTF_OpenFont("fonts/PixeloidMono.ttf", 16);

  if (!game->fonts[FONT_DEJAVU] || !game->fonts[FONT_PIXELOID_MONO] ||
      !game->fonts[FONT_PIXELOID_SANS] ||
      !game->fonts[FONT_PIXELOID_SANS_BOLD] ||
      !game->fonts[FONT_LEVEL_MENU_OPTION]) {
    game_free(game);
    return NULL;
  }

  GlobalState globalState = init_global_state(map);
  GameObject **objects = get_game_objects_from_state(&globalState);

  engine_set_player(engine, objects[0]);
  game->player = objects[0];

  um_ui_init(game);
  um_ui_disable(UI_DEAD_BACKGROUND);
  um_ui_disable(UI_DEAD_SCREEN);
  um_ui_disable(UI_PAUSE);
  um_ui_disable(UI_LEVEL_MENU);
  um_ui_disable(UI_LEVEL_MENU_FIRST);
  um_ui_disable(UI_LEVEL_MENU_SECOND);
  um_ui_disable(UI_LEVEL_MENU_THIRD);
  game->batch = (RenderBatch){.obj_count = 1 + game->state.enemies_count +
                                           game->state.enemies_count,
                              .objs = objects,
                              .ui_count = um_ui_get_uis_count(),
                              .uis = um_ui_get_uis()};
  game->engine = engine;
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

  if (game->state.status == GAME_DEAD) {
    return;
  }

  if (game->state.status == GAME_PAUSE) {
    if (game->pause_frame != 0) {
      game->pause_frame -= 1;
      return;
    }

    if (input->p) {
      game->state.status = GAME_ALIVE;
      game->pause_frame = 20;
      um_ui_disable(UI_DEAD_BACKGROUND);
      um_ui_disable(UI_PAUSE);
      return;
    }

    return;
  }

  if (game->state.status == GAME_LEVEL_UP) {
    if (game->pause_frame != 0) {
      game->pause_frame -= 1;
      return;
    }

    if (input->z || input->x || input->c) {
      game->state.status = GAME_ALIVE;
      game->pause_frame = 0;
      um_ui_disable(UI_LEVEL_MENU);
      um_ui_disable(UI_LEVEL_MENU_FIRST);
      um_ui_disable(UI_LEVEL_MENU_SECOND);
      um_ui_disable(UI_LEVEL_MENU_THIRD);
    }

    if (input->z) {
      player_level_up(&game->state.player, game, game->level_menu_first);
    } else if (input->x) {
      player_level_up(&game->state.player, game, game->level_menu_second);
    } else if (input->c) {
      player_level_up(&game->state.player, game, game->level_menu_third);
    }

    return;
  }

  // TODO: pause after game_dead (i thinks it's reallllly hard to catch)
  if (input->p && game->pause_frame == 0) {
    game->state.status = GAME_PAUSE;
    game->pause_frame = 20;
    um_ui_enable(UI_PAUSE);
    um_ui_enable(UI_DEAD_BACKGROUND);
    game->batch.ui_count = um_ui_get_uis_count();
    game->batch.uis = um_ui_get_uis();
    return;
  }

  make_step(&game->state, *input, game);
  GameObject **objects = get_game_objects_from_state(&(game->state));
  game->batch.obj_count =
      1 + game->state.enemies_count + game->state.projectiles_count;
  game->batch.objs = objects;

  if (game->pause_frame != 0) {
    game->pause_frame -= 1;
  }

  if (game->state.status == GAME_DEAD) {
    um_ui_enable(UI_DEAD_BACKGROUND);
    um_ui_enable(UI_DEAD_SCREEN);
  }

  if (game->state.status == GAME_LEVEL_UP) {
    um_ui_enable(UI_LEVEL_MENU);
    um_ui_enable(UI_LEVEL_MENU_FIRST);
    um_ui_enable(UI_LEVEL_MENU_SECOND);
    um_ui_enable(UI_LEVEL_MENU_THIRD);
  }

  um_ui_update(game);
  game->batch.ui_count = um_ui_get_uis_count();
  game->batch.uis = um_ui_get_uis();

  engine_set_player(game->engine, game->batch.objs[0]);
}
