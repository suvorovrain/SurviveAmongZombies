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
  result.stat_experience = 0.0;
  result.stat_experience_for_lvlup = 5.0;
  result.stat_piercing = 1.0;
  result.stat_damage = 30.0;
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
        (double)(lvlup_atk_speed_percent / 100.0f);
    break;
  case LVLUP_PROJ_COUNT:
    player->stat_proj_count += (double)lvlup_proj_count;
    break;
  case LVLUP_PIERCE:
    player->stat_piercing += (double)lvlup_pierce_count;
    break;
  case LVLUP_MOVEMENT:
    player->boost_movement_percent += (double)(lvlup_movement_percent / 100.0f);
    break;
  case LVLUP_EXP:
    player->boost_experience_percent += (double)(lvlup_exp_percent / 100.0f);
    break;
  case LVLUP_MAXHP:
    player->stat_max_hp += (double)(lvlup_maxhp_count);
    player->stat_hp += (double)(lvlup_maxhp_count);
    break;
  case LVLUP_DMG:
    player->boost_damage_percent += (double)(lvlup_dmg_percent / 100.0f);
    break;
  default:
    break;
  }
}

void player_free() { return; }