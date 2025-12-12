#ifndef GAME_H
#define GAME_H

#include "engine/engine.h"
#include "engine/input.h"
#include "engine/map.h"
#include "engine/types.h"
#include "state.h"
#include <SDL2/SDL_ttf.h>

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

extern const float lvlup_values[LVLUP_COUNT];
extern const size_t lvlup_pool_values[LVLUP_COUNT];

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
  // An engineering solution that perfectly solved the problem of storing menu
  // upgrade options.
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

Game *game_create(void);
void game_free(Game *game);
void game_update(Game *game, Input *input);

#endif
