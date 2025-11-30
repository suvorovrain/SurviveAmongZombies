#include "units.h"

Player player_create(Vector position) {
  Player result = {0};

  result.type = UNIT_PLAYER;
  result.position = position;
  result.movement = (Vector){0.0, 0.0};
  result.state = PLAYER_IDLE;

  Sprite *frames =
      load_spritesheet_frames("assets/units/player.png", 16, 15, 1, SCALE);
  result.spritesheet = (SpriteSheet){.frames = frames, .frames_count = 1};
  result.stat_movespeed = 3.0;
  result.stat_max_hp = 100.0;
  result.stat_hp = 100.0;
  result.stat_level = 1.0;
  result.stat_attack_speed = 0.75;
  result.stat_experience = 1.0;
  result.stat_piercing = 3;
  result.stat_damage = 100.0;
  result.stat_proj_count = 9.0;

  result.invincibility_count = 0;

  return result;
}

void player_free() { return; }