#include "units.h"

Enemy enemy_create(Vector position) {
  Enemy result = {0};

  result.type = UNIT_ENEMY;
  result.position = position;
  result.movement = (Vector){0.0, 0.0};
  result.state = ENEMY_IDLE;

  Sprite *frames =
      load_spritesheet_frames("assets/units/imp_d.png", 16, 15, 1, SCALE);
  result.spritesheet = (SpriteSheet){.frames = frames, .frames_count = 1};
  result.stat_movespeed = 1.5;
  result.stat_max_hp = 100.0;
  result.stat_hp = 100.0;
  result.stat_damage = 5.0;

  return result;
}

void enemy_free() { return; }