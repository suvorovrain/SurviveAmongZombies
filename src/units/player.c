#include "units.h"

Player player_create(Vector position) {
  Player result = {0};

  result.type = UNIT_PLAYER;
  result.position = position;
  result.movement = (Vector){0.0, 0.0};
  result.state = PLAYER_IDLE;

  Sprite *frames =
      load_spritesheet_frames("assets/units/player.png", 16, 15, 16, SCALE);
  result.spritesheet = (SpriteSheet){.frames = frames, .frames_count = 16};
  Sprite *frames_move =
      load_spritesheet_frames("assets/units/player.png", 16, 15, 16, SCALE);
  result.spritesheet_move = (SpriteSheet){.frames = frames, .frames_count = 16};
  result.stat_movespeed = 3.0;
  result.stat_max_hp = 100.0;
  result.stat_hp = 100.0;
  result.stat_level = 1.0;
  result.stat_attack_speed = 0.75;
  result.stat_experience = 1.0;
  result.stat_piercing = 1;
  result.stat_damage = 30.0;
  result.stat_proj_count = 2.0;

  result.invincibility_count = 0;
  result.direction_face = DIRECTION_DOWN;
  result.direction_side = DIRECTION_RIGHT;

  return result;
}

void player_level_up(Player *player, GlobalState *state) {
  printf("EXP %lf\n", player->stat_experience);
  printf("KILLS %ld\n", state->kills);
  printf("level %lf\n", state->player.stat_level);

  if (player->stat_experience < 1000.0) {
    return;
  }

  player->stat_level += 1;
  player->stat_experience -= 1000.0;
  player->stat_proj_count += 1;
  // player->stat_attack_speed -= (player->stat_attack_speed * 0.1);
}

void player_free() { return; }