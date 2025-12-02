#include "units.h"

Enemy enemy_create(Vector position) {
  Enemy result = {0};

  result.type = UNIT_ENEMY;
  result.position = position;
  result.movement = (Vector){0.0, 0.0};
  result.state = ENEMY_IDLE;

  size_t random = rand() % 3;

  SpriteType type;
  switch (random) {
  case 0:
    type = SPRITE_GOBLIN;
    break;
  case 1:
    type = SPRITE_SLIME;
    break;
  case 2:
    type = SPRITE_IMP;
    break;
  }

  result.spritesheet_move = sm_get_spritesheet(type);
  result.current_sprite = result.spritesheet_move.frames[0];

  result.stat_movespeed = 1.5;
  result.stat_max_hp = 100.0;
  result.stat_hp = 100.0;
  result.stat_damage = 5.0;

  return result;
}

void enemy_free() { return; }