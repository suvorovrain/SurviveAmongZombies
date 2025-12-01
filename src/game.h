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
} Game;

Game *game_create();
void game_free(Game *game);
void game_update(Game *game, Input *input);

#endif
