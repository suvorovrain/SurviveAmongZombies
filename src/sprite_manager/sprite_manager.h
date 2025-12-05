#pragma once

#include "../state.h"
#include "../units/units.h"

typedef enum {
  SPRITE_GOBLIN = 0,
  SPRITE_SLIME,
  SPRITE_PLAYER,
  SPRITE_IMP,
  SPRITE_PROJECTILE,
  SPRITE_PROJECTILE_EXPLODE,
  SPRITE_LEVEL_MENU,
  SPRITE_BLUE_CRYSTAL,
  SPRITE_GREEN_CRYSTAL,
  SPRITE_STATIC_BUSH,
  SPRITE_STATIC_CACTUS_LONG,
  SPRITE_STATIC_CACTUS_SHORT,
  SPRITE_STATIC_ROCK,
  SPRITE_COUNT
} SpriteType;

// load all sprites to memory
void sm_init();
// get spritesheet by key
SpriteSheet sm_get_spritesheet(SpriteType type);
// get sprite by key
Sprite sm_get_sprite(SpriteType type);
Sprite sm_get_rotated_sprite(SpriteType type, double angle);
Sprite *sm_get_sprite_pointer(SpriteType type);
// delete all sprite from memory
void sm_free();