#include "units.h"

Enemy enemy_create(Vector position) {
  Enemy result = {0};

  result.type = UNIT_ENEMY;
  result.position = position;
  result.movement = (Vector){0.0, 0.0};
  result.state = ENEMY_IDLE;

  size_t random = rand() % 3;

  char *path;
  int height;
  switch (random) {
  case 0:
    path = "assets/units/imp.png";
    height = 15;
    break;
  case 1:
    path = "assets/units/slime.png";
    height = 13;
    break;
  case 2:
    path = "assets/units/goblin.png";
    height = 16;
    break;
  default:
    break;
  }

  Sprite *frames = load_spritesheet_frames(path, 16, height, 1, SCALE);
  result.spritesheet = (SpriteSheet){.frames = frames, .frames_count = 1};
  Sprite *frames_move = load_spritesheet_frames(path, 16, height, 8, SCALE);
  result.spritesheet_move =
      (SpriteSheet){.frames = frames_move, .frames_count = 8};
  result.stat_movespeed = 1.5;
  result.stat_max_hp = 100.0;
  result.stat_hp = 100.0;
  result.stat_damage = 5.0;

  return result;
}

void enemy_free() { return; }