#include "units.h"

Player player_create(Vector position) {
  Player result = {0};

  result.type = UNIT_PLAYER;
  result.position = position;
  result.movement = (Vector){0.0, 0.0};
  result.state = PLAYER_IDLE;

  result.spritesheet_move = sm_get_spritesheet(SPRITE_PLAYER);
  result.current_sprite = result.spritesheet_move.frames[0];

  result.stat_movespeed = 3.0;
  result.stat_max_hp = 100.0;
  result.stat_hp = 100.0;
  result.stat_level = 1.0;
  result.stat_attack_speed = 0.75;
  result.stat_experience = 1.0;
  result.stat_piercing = 1;
  result.stat_damage = 30.0;
  result.stat_proj_count = 2.0;

  result.boost_attack_speed_percent = 1.0f;
  result.boost_piercing_percent = 1.0f;

  result.invincibility_count = 0;
  result.direction_face = DIRECTION_DOWN;
  result.direction_side = DIRECTION_RIGHT;

  return result;
}

void player_level_up(Player *player, GlobalState *state) {
  if (player->stat_experience < 1000.0) {
    return;
  }

  player->stat_level += 1;
  player->stat_experience -= 1000.0;
  // player->stat_proj_count += 0.5;
  player->boost_attack_speed_percent += 0.1f;
  player->boost_piercing_percent += 0.1f;
  player->stat_piercing *= 1.01;
}

void player_free() { return; }