#include "units.h"

Projectile projectile_create(Player player, Vector movement) {
  Projectile result = {0};

  const int size = 8;

  result.type = UNIT_PROJECTILE;
  Vector player_center = unit_get_centre(&player);
  Vector proj_size =
      (Vector){.x = (float)size * SCALE, .y = (float)size * SCALE};
  result.position =
      vector_add(player_center, vector_multiply(proj_size, -(float)0.5));
  result.stat_damage = player.stat_damage;
  result.stat_movespeed = 5.0;
  result.movement = movement;
  result.kills = 0;
  result.live_frames_last = 120;

  result.spritesheet_move = sm_get_spritesheet(SPRITE_PROJECTILE);
  result.spritesheet_explode = sm_get_spritesheet(SPRITE_PROJECTILE_EXPLODE);
  result.current_sprite = result.spritesheet_move.frames[0];

  return result;
}

void projectile_free(void) {}
