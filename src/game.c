#include "game.h"
#include "state.h"
#include "state/state.h"
#include "ui_manager/ui_manager.h"
#include "units/units.h"
#include <SDL2/SDL_ttf.h>
#include <engine/engine.h>
#include <engine/input.h>
#include <engine/map.h>
#include <engine/types.h>
#include <stb_ds.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define MAP_WIDTH 150
#define MAP_HEIGHT 150
#define STATIC_OBJ_COUNT 100

const float lvlup_values[LVLUP_COUNT] = {
    [LVLUP_ATK_SPD] = 10.0F,  [LVLUP_PROJ_COUNT] = 1.0F, [LVLUP_PIERCE] = 1.0F,
    [LVLUP_MOVEMENT] = 10.0F, [LVLUP_EXP] = 10.0F,       [LVLUP_MAXHP] = 10.0F,
    [LVLUP_DMG] = 10.0F};

const size_t lvlup_pool_values[LVLUP_COUNT] = {
    [LVLUP_ATK_SPD] = 100,  [LVLUP_PROJ_COUNT] = 10, [LVLUP_PIERCE] = 10,
    [LVLUP_MOVEMENT] = 100, [LVLUP_EXP] = 100,       [LVLUP_MAXHP] = 100,
    [LVLUP_DMG] = 100};

GameObject **get_game_objects_from_state(GlobalState *state) {
  GameObject **objects = (GameObject **)calloc(
      state->enemies_count + state->projectiles_count +
          state->exp_crystal_count + state->static_objects_count + 1,
      sizeof(GameObject *));

  GameObject *player = calloc(1, sizeof(GameObject));
  player->position = state->player.position;
  player->cur_sprite = &state->player.current_sprite;

  objects[0] = player;

  for (size_t i = 0; i < state->enemies_count; i++) {
    GameObject *enemy = calloc(1, sizeof(GameObject));
    enemy->position = state->enemies[i].position;
    enemy->cur_sprite = &state->enemies[i].current_sprite;

    objects[i + 1] = enemy;
  }

  for (size_t i = 0; i < state->projectiles_count; i++) {
    GameObject *projectile = calloc(1, sizeof(GameObject));
    projectile->position = state->projectiles[i].position;
    projectile->cur_sprite = &state->projectiles[i].current_sprite;

    objects[i + state->enemies_count + 1] = projectile;
  }

  for (size_t i = 0; i < state->exp_crystal_count; i++) {
    GameObject *exp_crystal = calloc(1, sizeof(GameObject));
    exp_crystal->position = state->exp_crystal[i].position;
    exp_crystal->cur_sprite = &state->exp_crystal[i].current_sprite;

    objects[i + state->enemies_count + state->projectiles_count + 1] =
        exp_crystal;
  }

  for (size_t i = 0; i < state->static_objects_count; i++) {
    GameObject *static_obj = calloc(1, sizeof(GameObject));
    static_obj->position = state->static_objects[i].position;
    static_obj->cur_sprite = state->static_objects[i].cur_sprite;

    objects[i + state->enemies_count + state->projectiles_count +
            state->exp_crystal_count + 1] = static_obj;
  }

  return objects;
}

Game *game_create(void) {
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
  ti.tile_sprites[0] = load_sprite("assets/static/grass.png", 4.0F);
  ti.sprite_count = 1;
  ti.tiles = calloc((unsigned long)(MAP_WIDTH)*MAP_HEIGHT, sizeof(uint32_t));
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
  game->fonts = (TTF_Font **)calloc(FONT_COUNT, sizeof(TTF_Font *));
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
                                           game->state.enemies_count +
                                           game->state.exp_crystal_count +
                                           game->state.static_objects_count,
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
      player_level_up(&game->state.player, game->level_menu_first);
    } else if (input->x) {
      player_level_up(&game->state.player, game->level_menu_second);
    } else if (input->c) {
      player_level_up(&game->state.player, game->level_menu_third);
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

  for (size_t i = 0; i < game->batch.obj_count; i++) {
    free(game->batch.objs[i]);
  }
  free((void *)game->batch.objs);

  GameObject **objects = get_game_objects_from_state(&(game->state));
  game->batch.obj_count =
      1 + game->state.enemies_count + game->state.projectiles_count +
      game->state.exp_crystal_count + game->state.static_objects_count;
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
