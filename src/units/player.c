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
  result.stat_attack_speed = 1.2;
  result.stat_experience = 0.0;
  result.stat_experience_for_lvlup = 5.0;
  result.stat_piercing = 1.0;
  result.stat_damage = 10.0;
  result.stat_proj_count = 1.0;

  result.boost_attack_speed_percent = 1.0f;
  result.boost_piercing_percent = 1.0f;
  result.boost_damage_percent = 1.0f;
  result.boost_experience_percent = 1.0f;
  result.boost_movement_percent = 1.0f;

  result.invincibility_count = 0;
  result.direction_face = DIRECTION_DOWN;
  result.direction_side = DIRECTION_RIGHT;

  return result;
}

void player_level_up(Player *player, Game *game, LevelUpStat stat) {
  if (player->stat_experience < player->stat_experience_for_lvlup) {
    return;
  }

  player->stat_level += 1;
  player->stat_experience -= player->stat_experience_for_lvlup;
  if (player->stat_level < 20) {
    player->stat_experience_for_lvlup += 10.0;
  } else if (player->stat_level < 40) {
    player->stat_experience_for_lvlup += 13.0;
  } else {
    player->stat_experience_for_lvlup += 16.0;
  }

  switch (stat) {
  case LVLUP_ATK_SPD:
    player->boost_attack_speed_percent +=
        (double)(lvlup_values[LVLUP_ATK_SPD] / 100.0f);
    break;
  case LVLUP_PROJ_COUNT:
    player->stat_proj_count += (double)lvlup_values[LVLUP_PROJ_COUNT];
    break;
  case LVLUP_PIERCE:
    player->stat_piercing += (double)lvlup_values[LVLUP_PIERCE];
    break;
  case LVLUP_MOVEMENT:
    player->boost_movement_percent +=
        (double)(lvlup_values[LVLUP_MOVEMENT] / 100.0f);
    break;
  case LVLUP_EXP:
    player->boost_experience_percent +=
        (double)(lvlup_values[LVLUP_EXP] / 100.0f);
    break;
  case LVLUP_MAXHP:
    player->stat_max_hp += (double)(lvlup_values[LVLUP_MAXHP]);
    player->stat_hp += (double)(lvlup_values[LVLUP_MAXHP]);
    break;
  case LVLUP_DMG:
    player->boost_damage_percent += (double)(lvlup_values[LVLUP_DMG] / 100.0f);
    break;
  default:
    break;
  }
}

void player_free() { return; }

LevelUpStat player_get_random_stat() {
  size_t pool[LVLUP_COUNT + 1];

  pool[0] = 0;
  size_t pool_sum = 0;
  for (size_t i = 1; i <= LVLUP_COUNT; i++) {
    pool[i] = pool[i - 1] + lvlup_pool_values[i - 1];
    pool_sum += lvlup_pool_values[i - 1];
  }

  size_t random = rand() % pool_sum;

  for (size_t i = 1; i <= LVLUP_COUNT; i++) {
    if (random >= pool[i - 1] && random < pool[i])
      return i - 1;
  }

  // Wrong implementaion
  exit(-1);
}