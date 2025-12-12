#include "../sprite_manager/sprite_manager.h"
#include "../state.h"
#include "engine/types.h"
#include "units.h"
#include <stdlib.h>

Enemy enemy_create(Vector position, EnemyType type_enemy) {
  Enemy result = {0};

  result.type = UNIT_ENEMY;
  result.type_enemy = type_enemy;
  result.position = position;
  result.movement = (Vector){(float)0.0, (float)0.0};
  result.state = ENEMY_IDLE;

  SpriteType type = SPRITE_SLIME;
  switch (type_enemy) {
  case ENEMY_SLIME:
    type = SPRITE_SLIME;
    break;
  case ENEMY_GOBLIN:
    type = SPRITE_GOBLIN;
    break;
  case ENEMY_IMP:
    type = SPRITE_IMP;
    break;
  default:
    exit(-1);
  }

  result.spritesheet_move = sm_get_spritesheet(type);
  result.current_sprite = result.spritesheet_move.frames[0];

  result.stat_movespeed = 1.5;

  switch (type_enemy) {
  case ENEMY_SLIME:
    result.stat_max_hp = 5.0;
    result.stat_hp = 5.0;
    result.stat_damage = 5.0;
    break;
  case ENEMY_IMP:
    result.stat_max_hp = 10.0;
    result.stat_hp = 10.0;
    result.stat_damage = 5.0;
    break;
  case ENEMY_GOBLIN:
    result.stat_max_hp = 15.0;
    result.stat_hp = 15.0;
    result.stat_damage = 10.0;
  default:
    break;
  }

  return result;
}

void enemy_free(void) {}
