#ifndef GAME_H
#define GAME_H

#include "engine/engine.h"
#include "engine/types.h"
#include "state.h"
#include "static_objs.h"

typedef enum {
  LVLUP_ATK_SPD = 0,
  LVLUP_PROJ_COUNT,
  LVLUP_PIERCE,
  LVLUP_MOVEMENT,
  LVLUP_EXP,
  LVLUP_MAXHP,
  LVLUP_DMG,
  LVLUP_COUNT
} LevelUpStat;

#define lvlup_atk_speed_percent (10.0f)
#define lvlup_proj_count (0.5f)
#define lvlup_pierce_count (0.5f)
#define lvlup_movement_percent (10.0f)
#define lvlup_exp_percent (10.0f)
#define lvlup_maxhp_count (10.0f)
#define lvlup_dmg_percent (10.0f)

typedef struct Game {
  GlobalState state;
  UIElement *uis;
  size_t ui_count;
  GameObject *player;
  RenderBatch batch; // All objects and UI elements to render
  TTF_Font **fonts;

  Map *map;
  Engine *engine;
  size_t pause_frame; // when pause button was pressed (20 frames gap)
  // костыль)
  LevelUpStat level_menu_first;
  LevelUpStat level_menu_second;
  LevelUpStat level_menu_third;
} Game;

enum {
  FONT_DEJAVU = 0,
  FONT_PIXELOID_MONO,
  FONT_PIXELOID_SANS,
  FONT_PIXELOID_SANS_BOLD,
  FONT_LEVEL_MENU_OPTION,
  FONT_COUNT
};

Game *game_create();
void game_free(Game *game);
void game_update(Game *game, Input *input);

#endif
