#include "units.h"

Enemy enemy_create(Vector position) {
  Enemy result = {0};

  result.type = UNIT_ENEMY;
  result.position = position;
  result.movement = (Vector){0.0, 0.0};
  result.state = ENEMY_IDLE;

  Sprite frame = load_sprite("assets/units/imp.png", 1.0f);
  if (frame.height == 0) {
    exit(-1);
  }
  Sprite *frames = calloc(sizeof(Sprite), 1);
  frames[0] = frame;
  result.spritesheet = (SpriteSheet){.frames = frames, .frames_count = 1};
  result.stat_movespeed = 1.5;
  result.stat_max_hp = 100.0;
  result.stat_hp = 100.0;
  result.stat_damage = 5.0;

  return result;
}

void enemy_free() { return; }