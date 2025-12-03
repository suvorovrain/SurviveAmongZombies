#ifndef GAME_H
#define GAME_H

#include "engine/engine.h"
#include "engine/types.h"
#include "state.h"
#include "static_objs.h"

typedef struct Game {
  GlobalState state;
  UIElement *uis;
  size_t ui_count;
  GameObject *player;
  RenderBatch batch; // All objects and UI elements to render
  TTF_Font **fonts;

  Map *map;
  Engine *engine;
  size_t pause_frame // when pause button was pressed (20 frames gap)
} Game;

enum {
  FONT_DEJAVU = 0,
  FONT_PIXELOID_MONO,
  FONT_PIXELOID_SANS,
  FONT_PIXELOID_SANS_BOLD,
  FONT_COUNT
};

Game *game_create();
void game_free(Game *game);
void game_update(Game *game, Input *input);

#endif
